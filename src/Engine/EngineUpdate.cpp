#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/Types/Types.h>
#include <GameEngineFramework/Profiler/profiler.h>

extern FloatType Float;
extern IntType Int;

extern ProfilerTimer Profiler;

int lastKey;

void EngineSystemManager::Update(void) {
    
    // Update command console
    if (console.input != nullptr) {
        if (console.input->isActive) {
            console.input->x = 0;
            console.input->y = Platform.windowArea.h - 65;
            console.input->w = Platform.windowArea.w;
            console.input->h = 20;
        }
    }
    
    for (unsigned int i=0; i < 32; i++) {
        Text* text = console.textElements[i];
        text->x = 5;
        text->y = Platform.windowArea.h - (90 + (16 * i));
    }
    
    // Update player/camera position in the simulation
    if (sceneMain != nullptr) {
        
        if (sceneMain->camera != nullptr) {
            
            Camera* activeCamera = sceneMain->camera;
            
            // Update player position in the AI simulation
            AI.SetPlayerWorldPosition( activeCamera->transform.position );
            
            // Update audio listener
            Audio.listenerPosition = activeCamera->transform.position;
            Audio.listenerDirection = activeCamera->forward;
        }
        
    }
    
    // Update component stream buffer
    UpdateComponentStream();
    
    // Run through the parent matrix transform chains
    UpdateTransformationChains();
    
    //
    // Update attached components
    //
    
    for (unsigned int i=0; i < mStreamSize; i++ ) {
        if (mStreamBuffer[i].rigidBody != nullptr)     UpdateRigidBody(i);
        if (mStreamBuffer[i].meshRenderer != nullptr)  UpdateMeshRenderer(i);
        if (mStreamBuffer[i].camera != nullptr)        UpdateCamera(i);
        if (mStreamBuffer[i].actor != nullptr)         UpdateActor(i);
        if (mStreamBuffer[i].light != nullptr)         UpdateLight(i);
        if (mStreamBuffer[i].sound != nullptr)         UpdateAudio(i);
        continue;
    }
    
    return;
}





