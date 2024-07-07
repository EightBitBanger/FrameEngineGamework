#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/Types/Types.h>
#include <GameEngineFramework/Profiler/profiler.h>

extern FloatType Float;
extern IntType Int;

extern ProfilerTimer Profiler;

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
    
    
    //
    // Profiler
    //
    
    if (mIsProfilerEnabled) {
        
        mProfilerText[0]->text  = "Renderer - " + Float.ToString( Profiler.profileRenderSystem );
        mProfilerText[0]->color = Colors.white;
        if (Profiler.profileRenderSystem > 10) mProfilerText[0]->color = Colors.yellow;
        if (Profiler.profileRenderSystem > 20) mProfilerText[0]->color = Colors.orange;
        if (Profiler.profileRenderSystem > 30) mProfilerText[0]->color = Colors.red;
        
        mProfilerText[1]->text = "Physics  - " + Float.ToString( Profiler.profilePhysicsSystem );
        mProfilerText[1]->color = Colors.white;
        if (Profiler.profilePhysicsSystem > 10) mProfilerText[1]->color = Colors.yellow;
        if (Profiler.profilePhysicsSystem > 20) mProfilerText[1]->color = Colors.orange;
        if (Profiler.profilePhysicsSystem > 30) mProfilerText[1]->color = Colors.red;
        
        
        mProfilerText[3]->text = "Engine   - " + Float.ToString( Profiler.profileGameEngineUpdate );
        
        mProfilerText[4]->text = "Draw calls - " + Float.ToString( Renderer.GetNumberOfDrawCalls() );
        
        mProfilerText[6]->text = "GameObject ------ " + Int.ToString( GetNumberOfGameObjects() );
        mProfilerText[7]->text = "Component ------- " + Int.ToString( GetNumberOfComponents() );
        mProfilerText[8]->text = "MeshRenderer ---- " + Int.ToString( Renderer.GetNumberOfMeshRenderers() );
        mProfilerText[9]->text = "Mesh ------------ " + Int.ToString( Renderer.GetNumberOfMeshes() );
        
        mProfilerText[11]->text = "x - " + Int.ToString( cameraController->GetComponent<Transform>()->position.x );
        mProfilerText[12]->text = "y - " + Int.ToString( cameraController->GetComponent<Transform>()->position.y );
        mProfilerText[13]->text = "z - " + Int.ToString( cameraController->GetComponent<Transform>()->position.z );
        
        mProfilerText[15]->text = "Garbage game objects --- " + Int.ToString( mGarbageGameObjects.size() );
        mProfilerText[16]->text = "Garbage rigid bodies --- " + Int.ToString( mGarbageRigidBodies.size() );
        mProfilerText[17]->text = "Clean rigid bodies ----- " + Int.ToString( mFreeRigidBodies.size() );
        
        mProfilerText[19]->text = "Camera Yaw ---- " + Float.ToString( sceneMain->camera->lookAngle.x );
        mProfilerText[20]->text = "Camera Pitch -- " + Float.ToString( sceneMain->camera->lookAngle.y );
        
    }
    
    return;
}





