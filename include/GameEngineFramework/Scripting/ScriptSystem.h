#ifndef SCRIPTING_SYSTEM
#define SCRIPTING_SYSTEM

#include <mutex>

#include "../../../vendor/CodeBaseLibrary/poolallocator.h"

#include <GameEngineFramework/Scripting/components/script.h>


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
