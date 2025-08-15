#ifndef _POOL_ALLOCATOR_SUPPORT__
#define _POOL_ALLOCATOR_SUPPORT__

#include <cstdlib>
#include <vector>
#include <mutex>

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
    
    /// Initial number of pools.
    int poolSize;
    
    /// Number of objects per pool.
    int poolCount;
    
};

template<typename T> class PoolAllocator {
    
public:
    
    T* operator[] (unsigned int const i) {return m_activeList[i];}
    
    PoolAllocator() :
        m_poolSz(80),
        m_poolCount(1)
    {
        this->allocate();
    }
    PoolAllocator(CustomAllocator customAllocator) :
        m_poolSz(customAllocator.poolSize),
        m_poolCount(1)
    {
        if (customAllocator.poolCount < 1) {customAllocator.poolCount=1;}
        
        for (int i=0; i < customAllocator.poolCount; i++) 
            this->allocate();
        
    }
    ~PoolAllocator() {
        // Iterate the pool list
        int poolListSz = m_pool.size();
        for (int i=0; i < poolListSz; i++) {
            
            // Get the next pair
            std::pair<T*, std::vector<bool>>* poolPair = &m_pool[i];
            T*    poolPtr = poolPair ->first;
            std::vector<bool> boolPtr = poolPair ->second;
            
#ifdef ENABLE_LEAK_DETECTION__
    #ifdef ENABLE_CONSOLE_DEBUG__
            
            // Iterate the pool
            int poolSz = this->m_poolSz;
            for (int f=0; f < poolSz; f++) {
                
                if (boolPtr[f]) {
                    T& poolRef = *poolPair ->first;
                    
                    std::cout << " [Leak detected]  < " << &poolPtr[f] << " >  pool #" << i << "  pool *";
                    std::cout << &poolRef << std::endl;
                }
                
            }
            
    #endif
#endif
            // Deallocate the pool pair
            std::free(poolPtr);
        }
    }
    
    /// Reserves an object and returns its pointer.
    T* Create(void) {
        std::lock_guard<std::mutex> lock(mux);
        
        // Ensure we have at least one free slot
        if (m_free.empty()) {
            this->allocate();
            if (m_pool.empty() || m_pool.back().first == nullptr) 
                return nullptr;
        }
        
        // Pop a free (pool,slot) location
        std::pair<int,int> loc = m_free.back();
        m_free.pop_back();
        int poolIdx = loc.first;
        int slotIdx = loc.second;
        
        T* poolPtr = m_pool[poolIdx].first;
        std::vector<bool>& used = m_pool[poolIdx].second;
        
        // Invariant: slot should be free; but be defensive
        int guard = 0;
        while (used[slotIdx]) {
            if (m_free.empty()) {
                this->allocate();
                if (m_pool.empty() || m_pool.back().first == nullptr)
                    return nullptr;
            }
            loc = m_free.back(); m_free.pop_back();
            poolIdx = loc.first; slotIdx = loc.second;
            poolPtr = m_pool[poolIdx].first;
            used = m_pool[poolIdx].second;
            if (++guard > 4) break; // shouldn't happen
        }
        
#ifdef ENABLE_CONSOLE_DEBUG__
    #ifdef ENABLE_DEBUG_ON_CONSTRUCT__
        std::cout << "  Constructed :: " << &poolPtr[slotIdx] << "   Pool * " << &poolPtr[0] << std::endl;
    #endif
#endif
        construct(poolPtr[slotIdx]);
        used[slotIdx] = true;
        m_activeList.push_back(&poolPtr[slotIdx]);
        return &poolPtr[slotIdx];
    }
    
    /// Frees an object.
    bool Destroy(T* objectPtr) {
        std::lock_guard<std::mutex> lock(mux);
        
        // Find owning pool by pointer range
        int poolIdx = -1;
        int poolListSz = static_cast<int>(m_pool.size());
        for (int i = 0; i < poolListSz; ++i) {
            T* base = m_pool[i].first;
            if (objectPtr >= base && objectPtr < (base + m_poolSz)) {
                poolIdx = i;
                break;
            }
        }
        if (poolIdx < 0) return false; // not ours
        
        T* base = m_pool[poolIdx].first;
        int slotIdx = static_cast<int>(objectPtr - base);
        std::vector<bool>& used = m_pool[poolIdx].second;
        
        if (!used[slotIdx]) return false; // double free or invalid
        
#ifdef ENABLE_CONSOLE_DEBUG__
    #ifdef ENABLE_DEBUG_ON_DESTRUCT__
        std::cout << "  Destructed :: " << &base[slotIdx] << "   Pool * " << &base[0] << std::endl;
    #endif
#endif
        destruct(base[slotIdx]);
        used[slotIdx] = false;
        
        // Remove from active list
        int ListSize = static_cast<int>(m_activeList.size());
        for (int a = 0; a < ListSize; ++a) {
            if (m_activeList[a] == objectPtr) {
                m_activeList.erase(m_activeList.begin() + a);
                break;
            }
        }

        // Return slot to free-list
        m_free.emplace_back(poolIdx, slotIdx);
        return true;
    }
    
    /// Returns the number of active objects.
    unsigned int Size(void) {
        std::lock_guard<std::mutex> lock(mux);
        return m_activeList.size();
    }
    
    /// Returns the number of used memory locations.
    int GetObjectCount(void) {
        std::lock_guard<std::mutex> lock(mux);
        
        int ObjectCount = 0;
        
        // Iterate the pool list
        int poolListSz = m_pool.size();
        for (int i=0; i < poolListSz; i++) {
            
            // Get the next pool and free list pair
            std::vector<bool> boolPtr = m_pool[i].second;
            
            // Iterate the pool its self
            int poolSz = this->m_poolSz;
            for (int f=0; f < poolSz; f++) {
                // Check if the object is active
                if (boolPtr[f]) ObjectCount++;
            }
        }
        return ObjectCount;
    }
    
    /// Returns the number of unused memory locations.
    int GetFreeCount(void) {
        std::lock_guard<std::mutex> lock(mux);
        
        int FreeCount = 0;
        
        // Iterate the pool list
        int poolListSz = m_pool.size();
        for (int i=0; i < poolListSz; i++) {
            
            // Get the next pool and free list pair
            std::vector<bool> boolPtr = m_pool[i].second;
            
            // Iterate the pool its self
            int poolSz = this->m_poolSz;
            for (int a=0; a < poolSz; a++) {
                // Check if the object is inactive
                if (!boolPtr[a]) FreeCount++;
            }
        }
        return FreeCount;
    }
    
    /// Debug output to console. Must #define ENABLE_CONSOLE_DEBUG__
    void Debug(void) {
        std::lock_guard<std::mutex> lock(mux);
        
#ifdef  ENABLE_CONSOLE_DEBUG__
        
    #ifdef  ENABLE_DEBUG_DETAILS__
        
        std::cout << std::endl << std::endl << std::endl;
        
        std::cout << "  Memory offset    State" << std::endl << std::endl;
        
        // Iterate the pool list
        int poolListSz = m_pool.size();
        for (int i=0; i < poolListSz; i++) {
            
            // Get the next pool and free list
            T* poolPtr = m_pool[i].first;
            std::vector<bool> boolPtr = m_pool[i].second;
            
            // Iterate the pool objects
            int poolSz = this->m_poolSz;
            for (int f=0; f < poolSz; f++) {
                
                std::cout << "  " << &poolPtr[f];
                
                if (boolPtr[f]) std::cout << "         " << "Reserved";
                
                std::cout << std::endl;
                
            }
            
        }
    #endif
        
        std::cout << std::endl << std::endl << " === Memory statistical analysis === " << std::endl << std::endl;
        
        std::string spcStr = "  ";
        
        std::cout << spcStr << this->getObjectCount() << " - Used memory locations" << std::endl;
        std::cout << spcStr << this->getFreeCount() << " - Unused memory locations" << std::endl;
        
        std::cout << std::endl;
        
        std::cout << spcStr << spcStr << m_poolSz << " - Pool size" << std::endl;
        std::cout << spcStr << spcStr << m_poolCount << " - Total pools" << std::endl;
        
        std::cout << std::endl;
        
        std::cout << spcStr << spcStr << m_poolSz * sizeof(T) <<      " - Size of a pool in bytes" << std::endl;
        std::cout << spcStr << spcStr << m_poolSz * sizeof(T) * m_poolCount << " - Total size of all pools in bytes" << std::endl;
        
        std::cout << std::endl<< std::endl;
        
#endif
        
    }
    
    /// Forces a memory dump. Must #define ENABLE_CRASH_DUMP__
    void ForceCrashDump(const char* FileName) {
        std::lock_guard<std::mutex> lock(mux);
        
#ifdef  ENABLE_CRASH_DUMP__
        
        std::string crashDumpFile = "crashdump.txt";
        std::fstream fDump;
        fDump.open(FileName, std::fstream::trunc | std::fstream::out);
        
        // Create the file if it does not exist
        if (!fDump.is_open()) fDump << "" << std::endl;
        
        fDump << "  Allocation crash report" << std::endl << std::endl;
        
        // Iterate the pool list
        int poolListSz = m_pool.size();
        for (int i=0; i < poolListSz; i++) {
            
            // Get the next pair
            std::pair<T*, std::vector<bool>>* poolPair = &m_pool[i];
            T*    poolPtr = poolPair ->first;
            std::vector<bool> boolPtr = poolPair ->second;
            
            // Log the pool pointer
            T& poolRef = *poolPair ->first;
            fDump << "pool[" << i << "] " << poolPair ->first << std::endl;
            
            // Iterate the pool
            int poolSz = this->m_poolSz;
            for (int f=0; f < poolSz; f++) {
                
                if (boolPtr[f]) {
                    
                    // Log the object pointer
                    T* poolRef = &poolPtr[f];
                    fDump << " " << f << " " << poolRef << std::endl;
                    
                }
                
            }
            
        }
        
        fDump.close();
        
#endif
        
    }
    
private:
    
    std::mutex mux;
    
    std::vector< std::pair<T*, std::vector<bool>> > m_pool;
    std::vector< T* > m_activeList;
    std::vector< std::pair<int,int> > m_free; // (poolIdx, slotIdx) free-list
    
    int m_poolSz;
    int m_poolCount;
    
    void construct(T& objectRef) {new (&objectRef) T();}
    void destruct(T& objectRef) {objectRef.~T();}
    
    std::pair<T*, std::vector<bool>> allocate(void) {
        
        // Allocate a pool and a state list pair
        T* poolPtr = (T*) std::malloc(m_poolSz * sizeof(T));
        std::vector<bool> boolList;
        
        // Check the pool allocation
        if (poolPtr == nullptr) return std::make_pair(nullptr, boolList);
        
        // Initiate all objects as inactive
        boolList.resize(m_poolSz, false);
        
        // Add the pair to the list
        std::pair<T*, std::vector<bool>> poolPair = std::make_pair(poolPtr, boolList);
        m_pool.push_back(poolPair);
        
        // Update pool count
        m_poolCount++;
        
        // Push all slots of this pool into the free-list
        int idx = static_cast<int>(m_pool.size()) - 1;
        for (int f = 0; f < m_poolSz; ++f) {
            m_free.emplace_back(idx, f);
        }
        
        return poolPair;
    }
    
    void deallocate(void) {
        
        // Deallocate the last pool pair
        std::pair<T*, std::vector<bool>>& lastPoolPair = m_pool.back();
        std::free(lastPoolPair.first);
        
        // Remove the pair from the list
        m_pool.erase( m_pool.end() );
        
        m_poolCount--;
    }
    
};

#endif

