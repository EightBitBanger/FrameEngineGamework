#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/Plugins/plugins.h>

bool isProfilerEnabled = false;

Actor* actorInSights = nullptr;


void Run() {
    
    // Update plug-in systems
    Weather.Update();
    Particle.Update();
    GameWorld.Update();
    
    if (Engine.cameraController == nullptr) 
        return;
    
    Camera* mainCamera = Engine.sceneMain->camera;
    if (mainCamera == nullptr) 
        return;
    
    glm::vec3 forward = mainCamera->forward;
    glm::vec3 from = mainCamera->transform.position;
    
    
    // Place static object
    if (Input.CheckMouseLeftPressed()) {
        Input.SetMouseLeftPressed(false);
        
    }
    
    // Remove static object
    if (Input.CheckMouseRightPressed()) {
        Input.SetMouseRightPressed(false);
        
    }
    
    
    // Spawn actors
    if (Input.CheckMouseMiddlePressed()) {
        //Input.SetMouseMiddlePressed(false);
        float randAmount = 8.0f;
        float xx = Random.Range(0.0f, randAmount) - Random.Range(0.0f, randAmount);
        float zz = Random.Range(0.0f, randAmount) - Random.Range(0.0f, randAmount);
        
        Hit hit;
        if (Physics.Raycast(from, forward, 100, hit, LayerMask::Ground)) {
            Actor* actor = GameWorld.SummonActor( glm::vec3(hit.point.x + xx, hit.point.y+5, hit.point.z + zz) );
            AI.genomes.presets.Human(actor);
            actor->physical.SetAge( Random.Range(actor->physical.GetAdultAge() / 2.0f, actor->physical.GetAdultAge() * 2.0f) );
            
            actor->memories.Add("action", "curiosity:1.0, speak:chirp:0.1");
            Sound* sound = Audio.CreateSound();
            
            actor->RebuildGeneticExpression();
            actor->isActive = true;
        }
        
    }
    
    
    
    //
    // Profiling
    //
    /*
    if (isProfilerEnabled) {
        glm::vec3 playerPos(0);
        if (Engine.sceneMain != nullptr) 
            if (Engine.sceneMain->camera != nullptr) 
                playerPos = Engine.sceneMain->camera->transform.position;
        
        Engine.console.WriteDialog(0, "player      " + Float.ToString( playerPos.x ) + ", " + Float.ToString( playerPos.y ) + ", " + Float.ToString( playerPos.z ));
        
        // Get chunk info
        glm::vec3 chunkPosition(0);
        Hit hit;
        if (Physics.Raycast(from, glm::vec3(0.0f, -1.0f, 0.0f), 1000, hit, LayerMask::Ground)) {
            GameObject* chunkObject = (GameObject*)hit.userData;
            chunkPosition = chunkObject->GetPosition();
            Engine.console.WriteDialog(1, "chunk       " + Float.ToString( chunkPosition.x ) + "_" + Float.ToString( chunkPosition.z ));
        }
        
        
        Engine.console.textDialog(, "AI          " + Float.ToString( Profiler.profileActorAI ) );
        Engine.console.WriteDialog(10, "AI          " + Float.ToString( Profiler.profileActorAI ) );
        Engine.console.WriteDialog(11, "Engine      " + Float.ToString( Profiler.profileGameEngineUpdate ) );
        Engine.console.WriteDialog(12, "Renderer    " + Float.ToString( Profiler.profileRenderSystem ) );
        
        Engine.console.WriteDialog(13, "Draw calls      " + Int.ToString(Renderer.GetNumberOfDrawCalls()) );
        
        Engine.console.WriteDialog(14, "GameObjects     " + Int.ToString(Engine.GetNumberOfGameObjects()) );
        Engine.console.WriteDialog(15, "Components      " + Int.ToString(Engine.GetNumberOfComponents()) );
        
        Engine.console.WriteDialog(16, "MeshRenderers   " + Int.ToString(Renderer.GetNumberOfMeshRenderers()) );
        Engine.console.WriteDialog(17, "Meshes          " + Int.ToString(Renderer.GetNumberOfMeshes()) );
        Engine.console.WriteDialog(18, "Materials       " + Int.ToString(Renderer.GetNumberOfMaterials()) );
        Engine.console.WriteDialog(19, "RigidBodies     " + Int.ToString(Physics.world->getNbRigidBodies()) );
        Engine.console.WriteDialog(20, "Actors          " + Int.ToString(AI.GetNumberOfActors()) );
        
    }
    */
    
    //
    // DEBUG - Show actor stats
    //
    
    /*
    if (actorInSights != nullptr && !isProfilerEnabled) {
        Engine.console.WriteDialog( 1, actorInSights->GetName() );
        Engine.console.WriteDialog( 2, Int.ToString( actorInSights->physical.GetAge() ) );
        
        Engine.console.WriteDialog( 17, "Distance to target   " + Int.ToString( actorInSights->navigation.GetDistanceToTarget() ) );
        Engine.console.WriteDialog( 18, "TargetPoint          " + Int.ToString( actorInSights->navigation.GetTargetPoint().x ) + ", " 
                                                                + Int.ToString( actorInSights->navigation.GetTargetPoint().y ) + ", "
                                                                + Int.ToString( actorInSights->navigation.GetTargetPoint().z ) );
        if (actorInSights->navigation.GetTargetActor() != nullptr) {
            Engine.console.WriteDialog( 19, "TargetActor          " + actorInSights->navigation.GetTargetActor()->GetName());
        } else {
            Engine.console.WriteDialog( 19, "TargetActor          nullptr");
        }
        
        Engine.console.WriteDialog( 19, "[ Vitality ]" );
        Engine.console.WriteDialog( 20, "Health   " + Int.ToString( actorInSights->biological.health ) );
        if (actorInSights->physical.GetSexualOrientation()) 
            Engine.console.WriteDialog( 21, "Male");
        else 
            Engine.console.WriteDialog( 21, "Female");
        
        Engine.console.WriteDialog( 22, "Mesh renderers   " + Int.ToString( actorInSights->genetics.GetNumberOfMeshRenderers() ) );
        
    }
    */
    

    //
    // Camera controller movement
    
    glm::vec3 force(0);
    
    if (Engine.cameraController == nullptr) 
        return;
    
    float forceAccelerate = 17.0f;
    
    if (mainCamera != nullptr) {
        
        // No movement when paused
        if (!Platform.isPaused) {
            
            // WASD Directional
            if (Input.CheckKeyCurrent(VK_W)) {force += mainCamera->forward;}
            if (Input.CheckKeyCurrent(VK_S)) {force -= mainCamera->forward;}
            if (Input.CheckKeyCurrent(VK_A)) {force -= mainCamera->right;}
            if (Input.CheckKeyCurrent(VK_D)) {force += mainCamera->right;}
            
            // Space/Shift Elevation
            if (Input.CheckKeyCurrent(VK_SPACE)) {force += mainCamera->up;}
            if (Input.CheckKeyCurrent(VK_SHIFT)) {force -= mainCamera->up;}
        }
        
        force *= forceAccelerate * forceAccelerate;
        
        Engine.cameraController->AddForce(force.x, force.y, force.z);
        
        rp3d::RigidBody* rigidBody = Engine.cameraController->GetComponent<rp3d::RigidBody>();
        rp3d::Vector3 forceVec3 = rigidBody->getLinearVelocity();
        float forceLen = glm::length(glm::vec3(forceVec3.x, forceVec3.y, forceVec3.z));
        
        // Field of view zoom effect
        const float fovMaxPullback = 3.0f;
        float fovPullback = (forceLen / 1.0f) * 0.3f;
        if (fovPullback > fovMaxPullback) 
            fovPullback = fovMaxPullback;
        Engine.sceneMain->camera->fov = 60 + fovPullback;
        
    }
    
    return;
}


void TickUpdate(void) {
    
    return;
}
