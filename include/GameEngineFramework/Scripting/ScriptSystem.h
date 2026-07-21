#ifndef SCRIPTING_SYSTEM
#define SCRIPTING_SYSTEM

#include <GameEngineFramework/MemoryAllocation/PoolAllocator.h>

#include <GameEngineFramework/Scripting/components/script.h>

#include <mutex>


class ENGINE_API ScriptSystem {
    
public:
    
    /// Create a script object and return its pointer.
    Script* CreateScript(void);
    
    /// Destroy a script object.
    bool DestroyScript(Script* scriptPtr);
    
    /// Call update on all active script objects.
    void Update(void);
    
    /// Return the number of scripts that are currently allocated.
    unsigned int GetScriptCount(void);
    
private:
    
    PoolAllocator<Script> mScript;
    
};

#endif
