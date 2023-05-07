#include "ScriptSystem.h"


void ScriptSystem::Update(void) {
    
    for (int i=0; i < mScript.Size(); i++ ) {
        Script* scriptRef = mScript[i];
        
        if (!scriptRef->isActive) 
            continue;
        
        if (!scriptRef->hasBeenInitiated) {
            scriptRef->hasBeenInitiated = true;
            scriptRef->OnCreate(scriptRef->gameObject);
        }
        
        scriptRef->OnUpdate(scriptRef->gameObject);
    }
    
    return;
}

Script* ScriptSystem::CreateScript(void) {
    Script* scriptPtr = mScript.Create();
    return scriptPtr;
}

bool ScriptSystem::DestroyScript(Script* scriptPtr) {
    bool ret = mScript.Destroy(scriptPtr);
    return ret;
}

