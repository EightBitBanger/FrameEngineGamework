#ifndef _POOL_ALLOCATOR_SUPPORT__
#define _POOL_ALLOCATOR_SUPPORT__

#include <cstdlib>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <cstdint>

#define  ENABLE_CONSOLE_DEBUG__
#define  ENABLE_DEBUG_DETAILS__
//#define  ENABLE_LEAK_DETECTION__
//#define  ENABLE_DEBUG_ON_CONSTRUCT__
//#define  ENABLE_DEBUG_ON_DESTRUCT__
//#define  ENABLE_DEBUG_PAUSE__
//#define  ENABLE_CRASH_DUMP__

#ifdef  ENABLE_CONSOLE_DEBUG__
 #include <iostream>
#endif

#ifdef  ENABLE_CRASH_DUMP__
 #include <fstream>
 #include <string>
#endif

struct CustomAllocator {
    int poolSize;   // objects per pool
    int poolCount;  // initial number of pools
};

template<typename T>
class PoolAllocator {
public:
    T* operator[](unsigned int const i) { return m_activeList[i]; }

    PoolAllocator()
        : m_poolSz(80), m_poolCount(0)
    {
        allocatePool();
    }

    PoolAllocator(CustomAllocator customAllocator)
        : m_poolSz(customAllocator.poolSize > 0 ? customAllocator.poolSize : 80),
          m_poolCount(0)
    {
        int count = (customAllocator.poolCount < 1) ? 1 : customAllocator.poolCount;
        for (int i = 0; i < count; ++i) allocatePool();
    }

    ~PoolAllocator() {
        std::lock_guard<std::mutex> lock(mux);

#ifdef ENABLE_LEAK_DETECTION__
    #ifdef ENABLE_CONSOLE_DEBUG__
        for (size_t i = 0; i < m_pools.size(); ++i) {
            auto& p = m_pools[i];
            for (int s = 0; s < m_poolSz; ++s) {
                if (p.isUsed(s)) {
                    std::cout << " [Leak detected] <" << (&p.data[s]) << "> pool #" << i
                              << " pool *" << static_cast<void*>(p.data) << std::endl;
                }
            }
        }
    #endif
#endif

        for (auto& p : m_pools) {
            // Call destructors for any still-live objects (safety; cheap)
            for (int s = 0; s < m_poolSz; ++s) {
                if (p.isUsed(s)) destruct(p.data[s]);
            }
            std::free(p.data);
        }
    }

    /// Reserves an object and returns its pointer.
    T* Create(void) {
        std::lock_guard<std::mutex> lock(mux);

        // Fast path: try last pool first
        int poolIdx = findPoolWithFree();
        if (poolIdx < 0) {
            if (!allocatePool()) return nullptr;
            poolIdx = static_cast<int>(m_pools.size()) - 1;
        }

        Pool& pool = m_pools[poolIdx];
        int slotIdx = pool.popFree();
        if (slotIdx < 0) return nullptr; // should not happen

        T* obj = &pool.data[slotIdx];
#ifdef ENABLE_CONSOLE_DEBUG__
    #ifdef ENABLE_DEBUG_ON_CONSTRUCT__
        std::cout << "  Constructed :: " << obj
                  << "   Pool * " << static_cast<void*>(pool.data) << std::endl;
    #endif
#endif
        construct(*obj);

#ifdef ENABLE_LEAK_DETECTION__
        pool.markUsed(slotIdx, true);
#endif

        // Track active objects; keep O(1) delete using back index
        int activeIdx = static_cast<int>(m_activeList.size());
        m_activeList.push_back(obj);
        m_activeIndex[obj] = activeIdx;

        return obj;
    }

    /// Frees an object.
    bool Destroy(T* objectPtr) {
        std::lock_guard<std::mutex> lock(mux);
        if (!objectPtr) return false;

        // Validate that this pointer is currently active (O(1) membership)
        auto it = m_activeIndex.find(objectPtr);
        if (it == m_activeIndex.end()) return false;

        // Locate pool by address range (few pools => trivial)
        int poolIdx = findOwningPool(objectPtr);
        if (poolIdx < 0) return false;

        Pool& pool = m_pools[poolIdx];
        int slotIdx = static_cast<int>(objectPtr - pool.data);

#ifdef ENABLE_CONSOLE_DEBUG__
    #ifdef ENABLE_DEBUG_ON_DESTRUCT__
        std::cout << "  Destructed :: " << objectPtr
                  << "   Pool * " << static_cast<void*>(pool.data) << std::endl;
    #endif
#endif
        destruct(*objectPtr);
#ifdef ENABLE_LEAK_DETECTION__
        pool.markUsed(slotIdx, false);
#endif
        pool.pushFree(slotIdx);

        // Remove from active list in O(1): swap-erase
        int idx = it->second;
        int lastIdx = static_cast<int>(m_activeList.size()) - 1;
        if (idx != lastIdx) {
            T* moved = m_activeList[lastIdx];
            m_activeList[idx] = moved;
            m_activeIndex[moved] = idx;
        }
        m_activeList.pop_back();
        m_activeIndex.erase(it);

        return true;
    }

    /// Returns the number of active objects.
    unsigned int Size(void) {
        std::lock_guard<std::mutex> lock(mux);
        return static_cast<unsigned int>(m_activeList.size());
    }

    /// Returns the number of used memory locations.
    int GetObjectCount(void) {
        std::lock_guard<std::mutex> lock(mux);
        return static_cast<int>(m_activeList.size());
    }

    /// Returns the number of unused memory locations.
    int GetFreeCount(void) {
        std::lock_guard<std::mutex> lock(mux);
        return static_cast<int>(m_pools.size()) * m_poolSz - static_cast<int>(m_activeList.size());
    }

    /// Debug output to console. Must #define ENABLE_CONSOLE_DEBUG__
    void Debug(void) {
        std::lock_guard<std::mutex> lock(mux);

#ifdef  ENABLE_CONSOLE_DEBUG__

    #ifdef  ENABLE_DEBUG_DETAILS__
        std::cout << "\n\n\n";
        std::cout << "  Memory offset    State\n\n";

        for (size_t i = 0; i < m_pools.size(); ++i) {
            Pool& pool = m_pools[i];
            for (int f = 0; f < m_poolSz; ++f) {
                std::cout << "  " << &pool.data[f];
                bool used = false;
            #ifdef ENABLE_LEAK_DETECTION__
                used = pool.isUsed(f);
            #else
                // If leak detection is off, approximate by checking membership
                used = (m_activeIndex.find(&pool.data[f]) != m_activeIndex.end());
            #endif
                if (used) std::cout << "         " << "Reserved";
                std::cout << std::endl;
            }
        }
    #endif

        std::cout << "\n\n === Memory statistical analysis === \n\n";
        std::string spcStr = "  ";
        std::cout << spcStr << GetObjectCount() << " - Used memory locations\n";
        std::cout << spcStr << GetFreeCount()   << " - Unused memory locations\n\n";
        std::cout << spcStr << spcStr << m_poolSz   << " - Pool size\n";
        std::cout << spcStr << spcStr << m_poolCount << " - Total pools\n\n";
        std::cout << spcStr << spcStr << (m_poolSz * (int)sizeof(T))
                  << " - Size of a pool in bytes\n";
        std::cout << spcStr << spcStr << (m_poolSz * (int)sizeof(T) * m_poolCount)
                  << " - Total size of all pools in bytes\n\n";
#endif
    }

    /// Forces a memory dump. Must #define ENABLE_CRASH_DUMP__
    void ForceCrashDump(const char* FileName) {
        std::lock_guard<std::mutex> lock(mux);

#ifdef  ENABLE_CRASH_DUMP__
        std::string crashDumpFile = "crashdump.txt";
        std::fstream fDump;
        fDump.open(FileName, std::fstream::trunc | std::fstream::out);
        if (!fDump.is_open()) fDump << "" << std::endl;

        fDump << "  Allocation crash report" << std::endl << std::endl;

        for (size_t i = 0; i < m_pools.size(); ++i) {
            Pool& pool = m_pools[i];
            fDump << "pool[" << i << "] " << static_cast<void*>(pool.data) << std::endl;

            for (int s = 0; s < m_poolSz; ++s) {
                bool used = false;
            #ifdef ENABLE_LEAK_DETECTION__
                used = pool.isUsed(s);
            #else
                used = (m_activeIndex.find(&pool.data[s]) != m_activeIndex.end());
            #endif
                if (used) {
                    fDump << " " << s << " " << &pool.data[s] << std::endl;
                }
            }
        }
        fDump.close();
#endif
    }

private:
    std::mutex mux;

    struct Pool {
        T* data = nullptr;
        std::vector<uint32_t> next;   // intrusive free list next indices
        int freeHead = -1;            // index of first free slot, -1 when none
        int usedCount = 0;

    #ifdef ENABLE_LEAK_DETECTION__
        // A tiny, cache-friendly usage bitmap (byte per slot).
        std::vector<uint8_t> usedBits;
        void markUsed(int idx, bool v) { usedBits[(size_t)idx] = v ? 1u : 0u; }
        bool isUsed(int idx) const { return usedBits[(size_t)idx] != 0u; }
    #else
        void markUsed(int, bool) {}
        bool isUsed(int) const { return false; }
    #endif

        int popFree() {
            if (freeHead < 0) return -1;
            int idx = freeHead;
            freeHead = (next[(size_t)idx] == UINT32_MAX) ? -1 : (int)next[(size_t)idx];
            ++usedCount;
            return idx;
        }
        void pushFree(int idx) {
            next[(size_t)idx] = (freeHead < 0) ? UINT32_MAX : (uint32_t)freeHead;
            freeHead = idx;
            --usedCount;
        }
        bool hasFree() const { return freeHead >= 0; }
        bool full(int poolSz) const { return usedCount == poolSz; }
    };

    std::vector<Pool> m_pools;
    std::vector<T*> m_activeList;
    std::unordered_map<T*, int> m_activeIndex;

    int m_poolSz;
    int m_poolCount;

    void construct(T& objectRef) { new (&objectRef) T(); }
    void destruct(T& objectRef) { objectRef.~T(); }

    // Find the pool that owns a given object pointer (small vector => cheap)
    int findOwningPool(T* ptr) const {
        for (int i = 0, n = static_cast<int>(m_pools.size()); i < n; ++i) {
            T* base = m_pools[(size_t)i].data;
            if (ptr >= base && ptr < base + m_poolSz) return i;
        }
        return -1;
    }

    // Find a pool with at least one free slot; try the newest pool first
    int findPoolWithFree() const {
        for (int i = static_cast<int>(m_pools.size()) - 1; i >= 0; --i) {
            if (m_pools[(size_t)i].hasFree()) return i;
        }
        return -1;
    }

    // Allocate a new pool; returns true on success.
    bool allocatePool() {
        Pool p;

        // Allocate raw storage
        p.data = (T*)std::malloc((size_t)m_poolSz * sizeof(T));
        if (!p.data) return false;

        p.next.resize((size_t)m_poolSz);
        for (int i = 0; i < m_poolSz - 1; ++i) p.next[(size_t)i] = (uint32_t)(i + 1);
        p.next[(size_t)(m_poolSz - 1)] = UINT32_MAX;
        p.freeHead = 0;
        p.usedCount = 0;

    #ifdef ENABLE_LEAK_DETECTION__
        p.usedBits.assign((size_t)m_poolSz, 0u);
    #endif

        m_pools.push_back(std::move(p));
        ++m_poolCount;
        return true;
    }

    // Backward-compat shim for Debug() which referenced lower-case names.
    int getObjectCount() { return GetObjectCount(); }
    int getFreeCount()   { return GetFreeCount(); }
};

#endif
