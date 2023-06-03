#include <iostream>
#include <string>

#include "../framework.h"
#include "../../source/Scripting/ScriptSystem.h"
extern ScriptSystem Scripting;


void TestFramework::TestScriptSystem(void) {
    if (hasTestFailed) return;
    
    std::cout << "Script system........... ";
    
    Script* scriptObject = Scripting.CreateScript();
    // Check create script
    if (scriptObject == nullptr) mLogString += msgFailedObjectCreate;
    // Check destroy script
    if (!Scripting.DestroyScript(scriptObject)) mLogString += msgFailedObjectDestroy;
    // Check no scripts left over after destruction
    if (Scripting.GetScriptCount() > 0) mLogString += msgFailedAllocatorNotZero;
    
    return;
}

