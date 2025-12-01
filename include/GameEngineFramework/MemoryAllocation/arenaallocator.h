#ifndef _ARENA_ALLOCATOR_SUPPORT__
#define _ARENA_ALLOCATOR_SUPPORT__

#include <cstdlib>
#include <vector>
#include <mutex>

#define  ENABLE_CONSOLE_DEBUG__
// #define  ENABLE_DEBUG_DETAILS__
// #define  ENABLE_LEAK_DETECTION__

// #define  ENABLE_DEBUG_ON_CONSTRUCT__
// #define  ENABLE_DEBUG_ON_DESTRUCT__
// #define  ENABLE_DEBUG_PAUSE__

// #define  ENABLE_CRASH_DUMP__

#ifdef  ENABLE_CONSOLE_DEBUG__
 #include <iostream>
#endif

#ifdef  ENABLE_CRASH_DUMP__
 #include <fstream>
 #include <string>
#endif

// Reuse your CustomAllocator shape for API symmetry.
// poolSize   -> objects per block
// poolCount  -> initial blocks
struct CustomAllocator {
    int poolSize;
    int poolCount;
};

template<typename T> class ArenaAllocator {
    
public:
    
    T* operator[] (unsigned int const i) { return m_activeList[i]; }
    
    ArenaAllocator() :
        m_blockSz(80),
        m_blockCount(0)
    {
        this->allocateBlock(m_blockSz);
    }
    
    explicit ArenaAllocator(CustomAllocator customAllocator) :
        m_blockSz(customAllocator.poolSize > 0 ? customAllocator.poolSize : 80),
        m_blockCount(0)
    {
        int initialBlocks = customAllocator.poolCount < 1 ? 1 : customAllocator.poolCount;
        for (int i = 0; i < initialBlocks; ++i) {
            this->allocateBlock(m_blockSz);
        }
    }
    
    ~ArenaAllocator() {
        // Run remaining destructors then free all blocks.
        this->Reset(); // calls destructors for still-active objects
        int n = static_cast<int>(m_blocks.size());
        for (int i = 0; i < n; ++i) {
            std::free(m_blocks[i].data);
        }
        m_blocks.clear();
        m_blockCount = 0;
    }
    
    // Construct a single T with default constructor
    T* Create(void) {
        std::lock_guard<std::mutex> lock(mux);

        Block* blk = this->ensureSpaceFor(1);
        if (blk == NULL) return NULL;

        T* ptr = &blk->data[blk->used];
        construct(*ptr);
        blk->used += 1;
        m_activeList.push_back(ptr);
        
#ifdef ENABLE_CONSOLE_DEBUG__
    #ifdef ENABLE_DEBUG_ON_CONSTRUCT__
        std::cout << "  Constructed :: " << ptr << "   Block * " << blk->data << std::endl;
    #endif
#endif
        return ptr;
    }
    
    // Construct a single T with arguments
    template<typename A0>
    T* Create(A0 a0) {
        std::lock_guard<std::mutex> lock(mux);
        
        Block* blk = this->ensureSpaceFor(1);
        if (blk == NULL) return NULL;
        
        T* ptr = &blk->data[blk->used];
        construct(*ptr, a0);
        blk->used += 1;
        m_activeList.push_back(ptr);
        
#ifdef ENABLE_CONSOLE_DEBUG__
    #ifdef ENABLE_DEBUG_ON_CONSTRUCT__
        std::cout << "  Constructed :: " << ptr << "   Block * " << blk->data << std::endl;
    #endif
#endif
        return ptr;
    }
    
    template<typename A0, typename A1>
    T* Create(A0 a0, A1 a1) {
        std::lock_guard<std::mutex> lock(mux);
        
        Block* blk = this->ensureSpaceFor(1);
        if (blk == NULL) return NULL;
        
        T* ptr = &blk->data[blk->used];
        construct(*ptr, a0, a1);
        blk->used += 1;
        m_activeList.push_back(ptr);
        
#ifdef ENABLE_CONSOLE_DEBUG__
    #ifdef ENABLE_DEBUG_ON_CONSTRUCT__
        std::cout << "  Constructed :: " << ptr << "   Block * " << blk->data << std::endl;
    #endif
#endif
        return ptr;
    }
    
    // Create N default-constructed Ts contiguously. Returns pointer to first.
    T* CreateN(int count) {
        std::lock_guard<std::mutex> lock(mux);
        if (count <= 0) return NULL;
        
        Block* blk = this->ensureSpaceFor(count);
        if (blk == NULL) return NULL;
        
        T* base = &blk->data[blk->used];
        for (int i = 0; i < count; ++i) {
            construct(base[i]);
            m_activeList.push_back(&base[i]);
        }
        blk->used += count;
        
#ifdef ENABLE_CONSOLE_DEBUG__
    #ifdef ENABLE_DEBUG_ON_CONSTRUCT__
        std::cout << "  Constructed (N=" << count << ") :: " << base
                  << " .. " << (&base[count-1]) << "   Block * " << blk->data << std::endl;
    #endif
#endif
        return base;
    }
    
    // Runs destructor for a single object (space is not reclaimed).
    bool Destroy(T* objectPtr) {
        std::lock_guard<std::mutex> lock(mux);
        if (objectPtr == NULL) return false;
        
        // Verify pointer lies within any block range.
        if (!this->owns(objectPtr)) return false;
        
        // If already removed, ignore; else remove from active list.
        int listSize = static_cast<int>(m_activeList.size());
        for (int i = 0; i < listSize; ++i) {
            if (m_activeList[i] == objectPtr) {
        
#ifdef ENABLE_CONSOLE_DEBUG__
    #ifdef ENABLE_DEBUG_ON_DESTRUCT__
                std::cout << "  Destructed :: " << objectPtr << std::endl;
    #endif
#endif
                destruct(*objectPtr);
                m_activeList.erase(m_activeList.begin() + i);
                return true;
            }
        }
        // Not tracked (double free or already destroyed)
        return false;
    }
    
    // Destroys all live objects and keeps memory for reuse.
    void Reset(void) {
        std::lock_guard<std::mutex> lock(mux);
        
        // Call destructors for all still-active objects.
        int n = static_cast<int>(m_activeList.size());
        for (int i = 0; i < n; ++i) {
            if (m_activeList[i] != NULL) {
                destruct(*m_activeList[i]);
            }
        }
        m_activeList.clear();
        
        // Reset bumps
        int bcount = static_cast<int>(m_blocks.size());
        for (int b = 0; b < bcount; ++b) {
            m_blocks[b].used = 0;
        }
    }
    
    // Releases all memory back to the OS.
    void DestroyAll(void) {
        std::lock_guard<std::mutex> lock(mux);
        
        this->Reset(); // run dtors
        int n = static_cast<int>(m_blocks.size());
        for (int i = 0; i < n; ++i) {
            std::free(m_blocks[i].data);
        }
        m_blocks.clear();
        m_blockCount = 0;
        
        // Keep one fresh block so allocator remains usable.
        this->allocateBlock(m_blockSz);
    }
    
    // Stats
    unsigned int Size(void) {
        std::lock_guard<std::mutex> lock(mux);
        return static_cast<unsigned int>(m_activeList.size());
    }
    
    int GetObjectCount(void) {
        std::lock_guard<std::mutex> lock(mux);
        return static_cast<int>(m_activeList.size());
    }
    
    int GetFreeCount(void) {
        std::lock_guard<std::mutex> lock(mux);
        int freeCount = 0;
        int bcount = static_cast<int>(m_blocks.size());
        for (int i = 0; i < bcount; ++i) {
            freeCount += (m_blocks[i].capacity - m_blocks[i].used);
        }
        return freeCount;
    }
    
    void Debug(void) {
        std::lock_guard<std::mutex> lock(mux);
        
#ifdef ENABLE_CONSOLE_DEBUG__

    #ifdef ENABLE_DEBUG_DETAILS__
        std::cout << std::endl << std::endl;
        std::cout << "  Block base        used/capacity" << std::endl << std::endl;
        
        int bcount = static_cast<int>(m_blocks.size());
        for (int i = 0; i < bcount; ++i) {
            std::cout << "  " << static_cast<void*>(m_blocks[i].data)
                      << "     " << m_blocks[i].used << "/" << m_blocks[i].capacity
                      << std::endl;
        }
        std::cout << std::endl;
    #endif
        
        std::cout << std::endl << " === Arena statistical analysis === " << std::endl << std::endl;
        
        std::string spcStr = "  ";
        std::cout << spcStr << this->GetObjectCount() << " - Live objects (tracked)" << std::endl;
        std::cout << spcStr << this->GetFreeCount()   << " - Unused slots (across blocks)" << std::endl;
        
        std::cout << std::endl;
        std::cout << spcStr << spcStr << m_blockSz     << " - Block size (objects)" << std::endl;
        std::cout << spcStr << spcStr << m_blockCount  << " - Total blocks" << std::endl;
        
        std::cout << std::endl;
        std::cout << spcStr << spcStr << (static_cast<long long>(m_blockSz) * sizeof(T))
                << " - Size of a block in bytes" << std::endl;
        
        long long totalCap = 0;
        int bcount = static_cast<int>(m_blocks.size());
        for (int i = 0; i < bcount; ++i) totalCap += m_blocks[i].capacity;
        std::cout << spcStr << spcStr << (totalCap * static_cast<long long>(sizeof(T)))
                << " - Total size of all blocks in bytes" << std::endl << std::endl;
        
#endif
    }
    
    void ForceCrashDump(const char* FileName) {
        std::lock_guard<std::mutex> lock(mux);
        
#ifdef ENABLE_CRASH_DUMP__
        std::fstream fDump;
        fDump.open(FileName, std::fstream::trunc | std::fstream::out);
        if (!fDump.is_open()) fDump << "" << std::endl;
        
        fDump << "  Arena crash report" << std::endl << std::endl;
        
        int bcount = static_cast<int>(m_blocks.size());
        for (int i = 0; i < bcount; ++i) {
            fDump << "block[" << i << "] " << static_cast<void*>(m_blocks[i].data)
                  << " used=" << m_blocks[i].used << " cap=" << m_blocks[i].capacity << std::endl;
        }
        
        int n = static_cast<int>(m_activeList.size());
        for (int a = 0; a < n; ++a) {
            fDump << "live[" << a << "] " << static_cast<void*>(m_activeList[a]) << std::endl;
        }
        fDump.close();
#endif
    }
    
private:
    struct Block {
        T*  data;
        int used;
        int capacity;
    };
    
    std::mutex mux;
    std::vector<Block> m_blocks;
    std::vector<T*>    m_activeList;
    
    int m_blockSz;     // default objects per block
    int m_blockCount;  // number of allocated blocks
    
    // Placement-new helpers
    void construct(T& objectRef) { new (&objectRef) T(); }
    template<typename A0> void construct(T& objectRef, A0 a0) { new (&objectRef) T(a0); }
    template<typename A0, typename A1> void construct(T& objectRef, A0 a0, A1 a1) { new (&objectRef) T(a0, a1); }
    void destruct(T& objectRef) { objectRef.~T(); }
    
    // Finds a block with at least 'need' free slots, allocating if necessary.
    Block* ensureSpaceFor(int need) {
        int bcount = static_cast<int>(m_blocks.size());
        if (bcount > 0) {
            Block* last = &m_blocks[bcount - 1];
            if ((last->capacity - last->used) >= need) {
                return last;
            }
        }
        // Not enough room — allocate a new block. If 'need' exceeds default,
        // allocate an oversized block to fit exactly.
        int cap = (need > m_blockSz) ? need : m_blockSz;
        return this->allocateBlock(cap);
    }
    
    // Allocates a new block with 'cap' objects.
    Block* allocateBlock(int cap) {
        T* ptr = reinterpret_cast<T*>(std::malloc(static_cast<size_t>(cap) * sizeof(T)));
        if (ptr == NULL) return NULL;
        
        Block blk;
        blk.data = ptr;
        blk.used = 0;
        blk.capacity = cap;
        
        m_blocks.push_back(blk);
        m_blockCount += 1;
        
        return &m_blocks.back();
    }
    
    bool owns(T* p) const {
        int bcount = static_cast<int>(m_blocks.size());
        for (int i = 0; i < bcount; ++i) {
            T* base = m_blocks[i].data;
            T* end  = base + m_blocks[i].capacity;
            if (p >= base && p < end) return true;
        }
        return false;
    }
};

#endif
