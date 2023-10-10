//
// Custom allocator pool class
//
// Author: SniperChicken

#ifndef _POOL_ALLOCATOR_SUPPORT__
#define _POOL_ALLOCATOR_SUPPORT__

#include <cstdlib>
#include <vector>


//
// Configuration

//#define  ENABLE_CONSOLE_DEBUG__
//#define  ENABLE_DEBUG_DETAILS__
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
    
    /** Initial number of pools.*/
    int poolSize;
    
    /** Number of objects per pool.*/
    int poolCount;
    
};

template<typename T> class PoolAllocator {
    
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
        
        m_poolSz           = 24;
        m_poolCount        = 1;
        
        this ->allocate();
        
    }
    PoolAllocator(CustomAllocator customAllocator) {
        
        m_poolSz           = customAllocator.poolSize;
        m_poolCount        = 0;
        
        if (customAllocator.poolCount < 1) {customAllocator.poolCount=1;}
        
        for (int i=0; i < customAllocator.poolCount; i++) this ->allocate();
        
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
            int poolSz = this ->m_poolSz;
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
                    
                    std::cout << "  Constructed :: " << &poolPtr[f] << "   Pool * " << &poolPtr[0] << std::endl;
                    
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
            
            // Check if the pool allocated
            if (&poolPtr == nullptr) return nullptr;
            
#ifdef ENABLE_CONSOLE_DEBUG__
    #ifdef ENABLE_DEBUG_ON_CONSTRUCT__
            
            std::cout << "  Constructed :: " << &poolPtr << "   Pool * " << &poolPtr << std::endl;
            
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
                        
                        std::cout << "  Destructed :: " << &poolPtr[f] << "   Pool * " << &poolRef << std::endl;
                        
    #endif
#endif
                        
                        // Explicitly call the destructor
                        destruct(poolPtr[f]);
                        
                        // Mark the object as inactive
                        boolPtr[f] = false;
                        
                        // Object is active, add to list
                        int ListSize = m_activeList.size();
                        for (int i=0; i < ListSize; i++) {
                            
                            if (m_activeList[i] == objectPtr) {m_activeList.erase( m_activeList.begin() + i ); break;}
                            
                        }
                        
                        return true;
                    } else return false;
                    
                }
                
            }
            
        }
        
        return false;
    }
    
    /** Returns the number of active objects.*/
    int Size(void) {return m_activeList.size();}
    
    
    /** Returns the number of used memory locations.*/
    int GetObjectCount(void) {
        
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
    int GetFreeCount(void) {
        
        int FreeCount = 0;
        
        // Iterate the pool list
        int poolListSz = m_pool.size();
        for (int i=0; i < poolListSz; i++) {
            
            // Get the next pool and free list pair
            //T*    poolPtr = m_pool[i].first;
            std::vector<bool> boolPtr = m_pool[i].second;
            
            // Iterate the pool its self
            int poolSz = this ->m_poolSz;
            for (int i=0; i < poolSz; i++) {
                
                // Check if the object is inactive
                if (!boolPtr[i]) FreeCount++;
                
            }
            
        }
        
        return FreeCount;
    }
    
    /** Debug output to console. Must #define ENABLE_CONSOLE_DEBUG__ */
    void Debug(void) {
        
#ifdef  ENABLE_CONSOLE_DEBUG__
        
    #ifdef  ENABLE_DEBUG_DETAILS__
        
        std::cout << std::endl << std::endl << std::endl;
        
        std::cout << "  Memory offset    State" << std::endl << std::endl;
        
        // Iterate the pool list
        int poolListSz = m_pool.size();
        for (int i=0; i < poolListSz; i++) {
            
            // Get the next pool and free list
            T*    poolPtr = m_pool[i].first;
            std::vector<bool> boolPtr = m_pool[i].second;
            
            // Iterate the pool objects
            int poolSz = this ->m_poolSz;
            for (int f=0; f < poolSz; f++) {
                
                std::cout << "  " << &poolPtr[f];
                
                if (boolPtr[f]) std::cout << "         " << "Reserved";
                
                std::cout << std::endl;
                
            }
            
        }
    #endif
        
        std::cout << std::endl << std::endl << " === Memory statistical analysis === " << std::endl << std::endl;
        
        std::string spcStr = "  ";
        
        std::cout << spcStr << this ->getObjectCount() << " - Used memory locations" << std::endl;
        std::cout << spcStr << this ->getFreeCount() << " - Unused memory locations" << std::endl;
        
        std::cout << std::endl;
        
        std::cout << spcStr << spcStr << m_poolSz << " - Pool size" << std::endl;
        std::cout << spcStr << spcStr << m_poolCount << " - Total pools" << std::endl;
        
        std::cout << std::endl;
        
        std::cout << spcStr << spcStr << m_poolSz * sizeof(T) <<      " - Size of a pool in bytes" << std::endl;
        std::cout << spcStr << spcStr << m_poolSz * sizeof(T) * m_poolCount << " - Total size of all pools in bytes" << std::endl;
        
        std::cout << std::endl<< std::endl;
        
#endif
        
    }
    
    /** Forces a memory dump. Must #define ENABLE_CRASH_DUMP__. */
    void ForceCrashDump(const char* FileName) {
        
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
            int poolSz = this ->m_poolSz;
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
    
};

#endif

