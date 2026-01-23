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
    
    std::lock_guard<std::mutex> lock(mux);
    std::lock_guard<std::mutex> renderLock(Renderer.mux);
    
    // Update component stream buffer
    UpdateComponentStream();
    
    // Update components by type
    unsigned int numberOfComponents = mComponentUpdaters.size();
    for (unsigned int c=0; c < numberOfComponents; c++) {
        
        switch (mComponentUpdateType[c]) {
        default:
            break;
        case ComponentUpdateType::PerFrame: mComponentUpdaters[c](0); break;
        case ComponentUpdateType::PerObject: for (unsigned int o=0; o < GetStreamSize(); o++) mComponentUpdaters[c](o); break;
        }
    }
}
