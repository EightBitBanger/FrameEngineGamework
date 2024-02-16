#include <GameEngineFramework/Engine/EngineSystems.h>


void EngineSystemManager::Update(void) {
    
    // Update player/camera position in the AI simulation
    if (sceneMain != nullptr) {
        
        Camera* activeCamera = sceneMain->camera;
        
        if (activeCamera != nullptr) 
            AI.SetPlayerWorldPosition( activeCamera->transform.position );
        
    }
    
    // Process garbage objects
    ProcessDeferredDeletion();
    
    // Update component stream buffer
    UpdateComponentStream();
    
    // Run through the parent matrix transform chains and apply the matrices therein
    UpdateTransformationChains();
    
    // Process UI elements
    UpdateUI();
    
    // Process console input
    UpdateConsole();
    
    
    //
    // Update attached components
    //
    
    for (unsigned int i=0; i < mStreamSize; i++ ) {
        
        if (mStreamBuffer[i].rigidBody != nullptr)       UpdateRigidBody(i);
        if (mStreamBuffer[i].meshRenderer != nullptr)    UpdateMeshRenderer(i);
        if (mStreamBuffer[i].camera != nullptr)          UpdateCamera(i);
        if (mStreamBuffer[i].actor != nullptr)           UpdateActor(i);
        if (mStreamBuffer[i].light != nullptr)           UpdateLight(i);
        if (mStreamBuffer[i].text != nullptr)            UpdateTextUI(i);
        
        if ((mStreamBuffer[i].panel != nullptr) & 
            (mStreamBuffer[i].meshRenderer != nullptr))  UpdatePanelUI(i);
        
        continue;
    }
    
    return;
}





