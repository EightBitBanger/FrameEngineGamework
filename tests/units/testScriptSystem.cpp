#include <iostream>
#include <string>

#include "../unitTest.h"
#include "../../source/Scripting/ScriptSystem.h"
extern ScriptSystem Scripting;


void ApplicationTest::TestScriptSystem(void) {
    if (hasTestFailed) return;
    
    std::cout << "Script system........... ";
    
    Script* scriptObject = Scripting.CreateScript();
    // Check create script
    if (scriptObject == nullptr) mLogString += msgFailedObjectCreation;
    // Check destroy script
    if (!Scripting.DestroyScript(scriptObject)) mLogString += msgFailedObjectDestruction;
    // Check no scripts left over after destruction
    if (Scripting.GetScriptCount() > 0) mLogString += msgFailedObjectAllocator;
    
    return;
}

