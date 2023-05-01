#include "ScriptSystem.h"


void ScriptSystem::Update(void) {
    
    for (int i=0; i < script.Size(); i++ ) {
        Script* scriptRef = script[i];
        
        if (!scriptRef->isActive) 
            continue;
        
        if (!scriptRef->hasBeenInitiated) {
            scriptRef->hasBeenInitiated = true;
            scriptRef->OnCreate();
        }
        
        scriptRef->OnUpdate();
    }
    
    return;
}


Script* ScriptSystem::CreateScript(void) {
    Script* scriptPtr = script.Create();
    return scriptPtr;
}

bool ScriptSystem::DestroyScript(Script* scriptPtr) {
    return script.Destroy(scriptPtr);
}

