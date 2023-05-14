#ifndef SCRIPTING_SYSTEM
#define SCRIPTING_SYSTEM

#include <mutex>

#include "../../vendor/my-std-lib/poolallocator.h"

#include "components/script.h"


class ScriptSystem {
    
public:
    
    /// Create a script object and return its pointer.
    Script* CreateScript(void);
    
    /// Destroy a script object.
    bool DestroyScript(Script* scriptPtr);
    
    /// Call update on all active script objects.
    void Update(void);
    
private:
    
    PoolAllocator<Script> mScript;
    
};

#endif
