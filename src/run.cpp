#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/Plugins/plugins.h>

bool isProfilerEnabled = false;

GameObject* boundsObject = nullptr;
Actor* actorTarget = nullptr;



void HitDetection(void) {
    float distance = 7.0f;
    
    Camera* mainCamera = Engine.sceneMain->camera;
    if (mainCamera == nullptr) 
        return;
    glm::vec3 forward = mainCamera->forward;
    glm::vec3 from = mainCamera->transform.position;
    
    // Actors
    
    /*
    Actor* actorHit = AI.Raycast(from, forward, distance);
    if (actorHit != nullptr) {
        Engine.console.ClearDialog();
        Engine.console.WriteDialog( 1, actorHit->GetName() );
        Engine.console.WriteDialog( 2, "Age " + Int.ToString( actorHit->physical.GetAge() ) );
        if (actorHit->physical.GetSexualOrientation()) 
            Engine.console.WriteDialog( 6, "Male");
        else 
            Engine.console.WriteDialog( 6, "Female");
        
        Engine.console.textDialog[4]->color = Colors.green * Colors.yellow * 0.9f;
        Engine.console.WriteDialog( 4, "[ Vitality ]" );
        Engine.console.WriteDialog( 5, "Health   " + Int.ToString( actorHit->biological.health ) );
        
        Engine.console.WriteDialog( 7, "Mesh renderers   " + Int.ToString( actorHit->genetics.GetNumberOfMeshRenderers() ) );
        return;
    } else {
        Engine.console.ClearDialog();
    }
    */
    
    // Static objects
    
    DecorationHitInfo hit = GameWorld.QueryDecor(from, forward, distance, 0.8f);
    if (hit.didHit) {
        Engine.console.textDialog[0]->color = Colors.green * Colors.yellow * 0.9f;
        Engine.console.WriteDialog(0, "[object]");
        Engine.console.WriteDialog(1, hit.type);
        
        if (boundsObject == nullptr) {
            boundsObject = Engine.Create<GameObject>();
            
            boundsObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
            MeshRenderer* boundsRenderer = boundsObject->GetComponent<MeshRenderer>();
            
            boundsRenderer->mesh = Engine.Create<Mesh>();
            boundsRenderer->mesh->SetPrimitive(MESH_LINE_STRIP);
            boundsRenderer->mesh->isShared = false;
            
            boundsRenderer->material = Engine.Create<Material>();
            boundsRenderer->material->shader = Resources.shaders.color;
            boundsRenderer->material->diffuse = Colors.white;
            boundsRenderer->material->ambient = Colors.white;
            boundsRenderer->material->isShared = false;
            
            boundsRenderer->isActive = true;
            Engine.sceneMain->AddMeshRendererToSceneRoot(boundsRenderer);
        }
        
        Transform* transform = boundsObject->GetComponent<Transform>();
        
        transform->position = hit.worldPosition;
        transform->rotation = -hit.rotation;
        
        transform->scale = hit.scale;
        
        MeshRenderer* boundsRenderer = boundsObject->GetComponent<MeshRenderer>();
        
        MeshTag* meshTag = Resources.FindMeshTag(hit.mesh);
        boundsRenderer->mesh->ClearSubMeshes();
        if (meshTag != nullptr) {
            for (unsigned int i=0; i < meshTag->subMeshes.size(); i++) {
                SubMesh& subMesh = meshTag->subMeshes[i];
                
                boundsRenderer->mesh->AddSubMesh(0, 0, 0, subMesh, false);
            }
        }
        boundsRenderer->mesh->Load();
        
    } else {
        Engine.console.ClearDialog();
        if (boundsObject != nullptr) {
            MeshRenderer* boundsRenderer = boundsObject->GetComponent<MeshRenderer>();
            
            Engine.sceneMain->RemoveMeshRendererFromSceneRoot(boundsRenderer, RENDER_QUEUE_GEOMETRY);
            Engine.Destroy(boundsObject);
            boundsObject = nullptr;
        }
    }
    
}



void Run() {
    
    Camera* mainCamera = Engine.sceneMain->camera;
    if (mainCamera == nullptr) 
        return;
    glm::vec3 forward = mainCamera->forward;
    glm::vec3 from = mainCamera->transform.position;
    
    // Update plug-in systems
    Weather.Update();
    Particle.Update();
    GameWorld.Update();
    
    if (Engine.cameraController == nullptr) 
        return;
    
    if (Input.CheckMouseLeftPressed()) {
        Input.SetMouseLeftPressed(false);
        
    }
    
    if (Input.CheckMouseRightPressed()) {
        Input.SetMouseRightPressed(false);
        Actor* actorHit = AI.Raycast(from, forward, 100);
        if (actorHit != nullptr) {
            
            actorHit->biological.health -= 30.0f;
            
        }
        
    }
    
    
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
    
    
    
    
    HitDetection();
    
    
    
    
    
    
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
    
    const float forceAccelerate = 24.0f;
    
    glm::vec3 force(0);
    
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
        float fovPullback = (forceLen / 1.0f) * 0.4f;
        if (fovPullback > fovMaxPullback) 
            fovPullback = fovMaxPullback;
        Engine.sceneMain->camera->fov = 60 + fovPullback;
        
    }
    
}


void TickUpdate(void) {
    
}
