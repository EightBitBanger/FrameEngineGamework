//
// Custom allocator pool class
//
// Author: David Penning

#ifndef _POOL_ALLOCATOR_SUPPORT__
#define _POOL_ALLOCATOR_SUPPORT__

#include <GameEngineFramework/configuration.h>

#include <cstdlib>
#include <vector>

//
// Configuration

#define  ENABLE_CONSOLE_DEBUG__
#define  ENABLE_DEBUG_DETAILS__
//#define  ENABLE_LEAK_DETECTION__

//#define  ENABLE_DEBUG_ON_CONSTRUCT__
//#define  ENABLE_DEBUG_ON_DESTRUCT__
//#define  ENABLE_DEBUG_PAUSE__

#define  ENABLE_CRASH_DUMP__


#ifdef  ENABLE_CONSOLE_DEBUG__
 #include <iostream>
#endif

#ifdef  ENABLE_CRASH_DUMP__
 #include <fstream>
 #include <string>
#endif

struct ENGINE_API CustomAllocator {
    
    /** Initial number of pools.*/
    int poolSize;
    
    /** Number of objects per pool.*/
    int poolCount;
    
};

template<typename T> class ENGINE_API PoolAllocator {
    
    std::vector< std::pair<T*, std::vector<bool>> > m_pool;
    std::vector< T* > m_activeList;
    
    int m_poolSz;
    int m_poolCount;
    
    void construct(T& objectRef) {new (&objectRef) T();}
    void destruct(T& objectRef) {
        
        objectRef.~T();
        
    }
    
    std::pair<T*, std::vector<bool>> allocate(void) {
        
        // Allocate a pool and a state list pair
        T*    poolPtr = (T*)   malloc(m_poolSz * sizeof(T));
        std::vector<bool> boolList;
        
        // Check the pool allocation
        if (poolPtr == nullptr) return std::make_pair(nullptr, boolList);
        
        // Initiate all objects as inactive
        boolList.resize(m_poolSz);
        for (int i=0; i < m_poolSz; i++) boolList.push_back(false);
        
        // Add the pair to the list
        std::pair<T*, std::vector<bool>> poolPair = std::make_pair(poolPtr, boolList);
        m_pool.push_back( poolPair );
        
        m_poolCount++;
        
        return poolPair;
    }
    
    void deallocate(void) {
        
        // Deallocate the last pool pair
        std::pair<T*, std::vector<bool>>& lastPoolPair = *m_pool.end();
        std::free(lastPoolPair.first);
        
        // Remove the pair from the list
        m_pool.erase( m_pool.end() );
        
        m_poolCount--;
    }
    
public:
    
    T* operator[] (unsigned int const i) {return m_activeList[i];}
    
    PoolAllocator() {
        
        m_poolSz           = 1024;
        m_poolCount        = 1;
        
        this ->allocate();
        return;
    }
    PoolAllocator(CustomAllocator customAllocator) {
        
        m_poolSz           = customAllocator.poolSize;
        m_poolCount        = 0;
        
        if (customAllocator.poolCount < 1) {customAllocator.poolCount=1;}
        
        for (int i=0; i < customAllocator.poolCount; i++) 
            this ->allocate();
        return;
    }
    PoolAllocator(unsigned int poolSize, unsigned int poolCount) {
        
        m_poolSz           = poolSize;
        m_poolCount        = poolCount;
        
        for (int i=0; i < poolCount; i++) 
            this ->allocate();
        return;
    }
    ~PoolAllocator() {
        
        Clear();
        
        return;
    }
    
    /** Reserves an object and returns its pointer.*/
    T* Create(void) {
        
        T* objectPtr = nullptr;
        
        // Iterate the pool list
        int poolListSz = m_pool.size();
        for (int i=0; i < poolListSz; i++) {
            
            // Get the next pool and free list
            T*    poolPtr = m_pool[i].first;
            std::vector<bool>& boolPtr = m_pool[i].second;
            
            // Iterate the pool objects
            int poolSz = this ->m_poolSz;
            for (int f=0; f < poolSz; f++) {
                
                // Check if the object is available
                if (!boolPtr[f]) {
                    
#ifdef ENABLE_CONSOLE_DEBUG__
    #ifdef ENABLE_DEBUG_ON_CONSTRUCT__
                    
                    std::cout << "  Constructed :: " << &poolPtr[f] << "   Pool * " << &poolPtr[0] << "\n";
                    
    #endif
#endif
                    
                    // Call the constructor
                    T& objectRef = poolPtr[f];
                    construct(objectRef);
                    
                    // Mark the object as active
                    boolPtr[f] = true;
                    
                    m_activeList.push_back(&poolPtr[f]);
                    
                    return &poolPtr[f];
                }
                
            }
            
        }
        
        // All pools are full, allocate a new pool
        if (objectPtr == nullptr) {
            
            this ->allocate();
            
            std::pair<T*, std::vector<bool>>& poolPair = m_pool[m_pool.size()-1];
            
            T& poolPtr = *poolPair.first;
            std::vector<bool>& boolPtr = poolPair.second;
            
#ifdef ENABLE_CONSOLE_DEBUG__
    #ifdef ENABLE_DEBUG_ON_CONSTRUCT__
            
            std::cout << "  Constructed :: " << &poolPtr << "   Pool * " << &poolPtr << "\n";
            
    #endif
#endif
            
            // Call the constructor
            new (&poolPtr) T();
            
            // Mark the first object as active
            boolPtr[0] = true;
            
            m_activeList.push_back(&poolPtr);
            
            objectPtr = &poolPtr;
        }
        
        return objectPtr;
    }
    
    /** Frees an object.*/
    bool Destroy(T* objectPtr) {
        
        // Iterate the list of pools
        int poolListSz = m_pool.size();
        for (int i=0; i < poolListSz; i++) {
            
            // Get the next pool and state list
            T*    poolPtr = m_pool[i].first;
            std::vector<bool>& boolPtr = m_pool[i].second;
            
            // Iterate the pool its self
            int poolSz = this ->m_poolSz;
            for (int f=0; f < poolSz; f++) {
                
                // Check matching pointers
                if (objectPtr == &poolPtr[f]) {
                    
                    // Check if the object is active
                    if (boolPtr[f]) {
                        
#ifdef ENABLE_CONSOLE_DEBUG__
    #ifdef ENABLE_DEBUG_ON_DESTRUCT__
                        T& poolRef = *poolPtr;
                        
                        std::cout << "  Destructed :: " << &poolPtr[f] << "   Pool * " << &poolRef << "\n";
                        
    #endif
#endif
                        
                        // Explicitly call the destructor
                        destruct(poolPtr[f]);
                        
                        // Mark the object as inactive
                        boolPtr[f] = false;
                        
                        // Object is active, add to list
                        int ListSize = m_activeList.size();
                        for (int a=0; a < ListSize; a++) {
                            
                            if (m_activeList[a] == objectPtr) {m_activeList.erase( m_activeList.begin() + a ); break;}
                            
                        }
                        
                        return true;
                    } else return false;
                    
                }
                
            }
            
        }
        
        return false;
    }
    
    /** Clears the entire pool of all its objects.*/
    void Clear(void) {
        
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
            int poolSz = this ->m_poolSz;
            for (int f=0; f < poolSz; f++) {
                
                if (boolPtr[f]) {
                    T& poolRef = *poolPair ->first;
                    
                    std::cout << " [Leak detected]  < " << &poolPtr[f] << " >  pool #" << i << "  pool *";
                    std::cout << &poolRef << "\n";
                }
                
            }
            
    #endif
#endif
            
            // Deallocate the pool pair
            std::free(poolPtr);
            
        }
        
        m_pool.clear();
        
        return;
    }
    
    /** Returns the number of active objects.*/
    unsigned int Size(void) {
        return m_activeList.size();
    }
    
    
    /** Returns the number of used memory locations.*/
    unsigned int GetObjectCount(void) {
        
        int ObjectCount = 0;
        
        // Iterate the pool list
        int poolListSz = m_pool.size();
        for (int i=0; i < poolListSz; i++) {
            
            // Get the next pool and free list pair
            //T*    poolPtr = m_pool[i].first;
            std::vector<bool> boolPtr = m_pool[i].second;
            
            // Iterate the pool its self
            int poolSz = this ->m_poolSz;
            for (int f=0; f < poolSz; f++) {
                
                // Check if the object is active
                if (boolPtr[f]) ObjectCount++;
                
            }
            
        }
        
        return ObjectCount;
    }
    /** Returns the number of unused memory locations.*/
    unsigned int GetFreeCount(void) {
        
        unsigned int FreeCount = 0;
        
        // Iterate the pool list
        unsigned int poolListSz = m_pool.size();
        for (unsigned int i=0; i < poolListSz; i++) {
            
            // Get the next pool and free list pair
            //T*    poolPtr = m_pool[i].first;
            std::vector<bool> boolPtr = m_pool[i].second;
            
            // Iterate the pool its self
            unsigned int poolSz = this ->m_poolSz;
            for (unsigned int a=0; a < poolSz; a++) {
                
                // Check if the object is inactive
                if (!boolPtr[a]) FreeCount++;
                
            }
            
        }
        
        return FreeCount;
    }
    
    /** Debug output to console. Must #define ENABLE_CONSOLE_DEBUG__ */
    void Debug(void) {
        
#ifdef  ENABLE_CONSOLE_DEBUG__
        
    #ifdef  ENABLE_DEBUG_DETAILS__
        
        std::cout << "\n\n\n";
        
        std::cout << "  Memory offset    State\n\n";
        
        // Iterate the pool list
        unsigned int poolListSz = m_pool.size();
        for (unsigned int i=0; i < poolListSz; i++) {
            
            // Get the next pool and free list
            T*    poolPtr = m_pool[i].first;
            std::vector<bool> boolPtr = m_pool[i].second;
            
            // Iterate the pool objects
            unsigned int poolSz = this ->m_poolSz;
            for (unsigned int f=0; f < poolSz; f++) {
                
                std::cout << "  " << &poolPtr[f];
                
                if (boolPtr[f]) std::cout << "         " << "Reserved";
                
                std::cout << "\n";
                
            }
            
        }
    #endif
        
        std::cout << "\n\n === Memory statistical analysis === \n\n";
        
        std::string spcStr = "  ";
        
        std::cout << spcStr << this ->getObjectCount() << " - Used memory locations\n";
        std::cout << spcStr << this ->getFreeCount() << " - Unused memory locations\n";
        
        std::cout << "\n";
        
        std::cout << spcStr << spcStr << m_poolSz << " - Pool size\n";
        std::cout << spcStr << spcStr << m_poolCount << " - Total pools\n";
        
        std::cout << "\n";
        
        std::cout << spcStr << spcStr << m_poolSz * sizeof(T) <<      " - Size of a pool in bytes\n";
        std::cout << spcStr << spcStr << m_poolSz * sizeof(T) * m_poolCount << " - Total size of all pools in bytes\n";
        
        std::cout << "\n\n";
        
#endif
        
        return;
    }
    
    /** Forces a memory dump. Must #define ENABLE_CRASH_DUMP__. */
    void ForceCrashDump(const char* FileName) {
        
        std::fstream fDump;
        fDump.open(FileName, std::fstream::trunc | std::fstream::out);
        
        // Create the file if it does not exist
        if (!fDump.is_open()) fDump << "\n";
        
        fDump << "  Allocation crash report\n";
        
        // Iterate the pool list
        int poolListSz = m_pool.size();
        for (unsigned int i=0; i < poolListSz; i++) {
            
            // Get the next pair
            std::pair<T*, std::vector<bool>>* poolPair = &m_pool[i];
            T*    poolPtr = poolPair ->first;
            std::vector<bool> boolPtr = poolPair ->second;
            
            // Log the pool pointer
            T& poolRef = *poolPair ->first;
            fDump << "pool[" << i << "] " << poolPair ->first << "\n";
            
            // Iterate the pool
            int poolSz = this ->m_poolSz;
            for (unsigned int f=0; f < poolSz; f++) {
                
                if (boolPtr[f]) {
                    
                    // Log the object pointer
                    T* poolRef = &poolPtr[f];
                    fDump << " " << f << " " << poolRef << "\n";
                    
                    char* charPtr = (char*)poolPtr;
                    
                    for (unsigned int z=0; z < sizeof(T); z++) 
                        fDump << &charPtr[z];
                    
                }
                
            }
            
        }
        
        fDump.close();
        
        return;
    }
    
};

#endif

