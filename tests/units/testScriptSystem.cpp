#include <iostream>
#include <string>

#include "../framework.h"
#include <GameEngineFramework/Scripting/ScriptSystem.h>
extern ScriptSystem Scripting;


void TestFramework::TestScriptSystem(void) {
    if (hasTestFailed) return;
    
    std::cout << "Script system........... ";
    
    Script* scriptObject = Scripting.CreateScript();
    // Check create script
    if (scriptObject == nullptr) Throw(msgFailedObjectCreate, __FILE__, __LINE__);
    // Check destroy script
    if (!Scripting.DestroyScript(scriptObject)) Throw(msgFailedObjectDestroy, __FILE__, __LINE__);
    // Check no scripts left over after destruction
    if (Scripting.GetScriptCount() > 0) Throw(msgFailedAllocatorNotZero, __FILE__, __LINE__);
    
    return;
}

