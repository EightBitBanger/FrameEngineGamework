#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/Plugins/plugins.h>

bool isProfilerEnabled = false;

glm::vec3 force(0);
float forceDblTime=0;

Actor* actorInSights = nullptr;

const float hitMaxDistance        = 4.5f;
const float hitThreshold          = 0.97f;
const float clickRepeatTimeout    = 250.0f;

float clickCooldownLeft  = 0.0;
float clickCooldownRight = 0.0;
bool isLeftClickHold     = false;
bool isRightClickHold    = false;

bool CheckCooldown(float nowMs, float& nextAllowedMs, float cooldownMs) {
    if (nowMs < nextAllowedMs) return false;
    nextAllowedMs = nowMs + cooldownMs;
    return true;
}

void MouseButtonLeft() {
    
}

void MouseButtonRight() {
    
}

MeshRenderer* selectedMeshRenderer = nullptr;


void Run() {
    
    // Update plug-in systems
    Weather.Update();
    Particle.Update();
    
    if (Engine.cameraController == nullptr) 
        return;
    
    Camera* cameraPtr = Engine.cameraController->GetComponent<Camera>();
    if (cameraPtr == nullptr) 
        return;
    
    glm::vec3 forward = cameraPtr->forward;
    glm::vec3 from = cameraPtr->transform.position;
    
    
    if (isLeftClickHold) {
        MouseButtonLeft();
        
        for (unsigned int i=0; i < 24; i++) {
            
            GameObject* gameObject = Engine.Create<GameObject>();
            gameObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
            
            MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
            meshRenderer->mesh = Engine.meshes.cube;
            meshRenderer->material = Engine.Create<Material>();
            meshRenderer->material->shader = Engine.shaders.color;
            
            meshRenderer->material->DisableCulling();
            meshRenderer->material->EnableShadowVolumePass();
            
            glm::vec3 randomColor = {Random.Range(0.0f, 1.0f), 0, Random.Range(0.0f, 1.0f)};
            
            meshRenderer->material->diffuse = randomColor * 0.4f;
            meshRenderer->material->ambient = Colors.gray;
            
            meshRenderer->material->EnableShadowVolumePass();
            meshRenderer->material->SetShadowVolumeColor((Colors.yellow + (Colors.red * Colors.gray * 0.018)) * 0.01f);
            meshRenderer->material->SetShadowVolumeIntensityLow(0.001f);
            meshRenderer->material->SetShadowVolumeIntensityHigh(0.03f);
            meshRenderer->material->SetShadowVolumeColorIntensity(0.05f);
            meshRenderer->material->SetShadowVolumeLength(8.0f);
            
            meshRenderer->material->SetShadowVolumeAngleOfView(4.0f);
            
            glm::vec3 randomOffset = {Random.Range(-100, 100), 
                                      Random.Range(-100, 100) + 50, 
                                      Random.Range(-100, 100)};
            
            Transform* transform = gameObject->GetComponent<Transform>();
            transform->Translate( Engine.cameraController->GetPosition() + randomOffset );
            transform->Scale(2.0f, 2.0f, 2.0f);
            transform->UpdateMatrix();
            
            Engine.sceneMain->AddMeshRendererToSceneRoot(meshRenderer);
        }
        
    }
    
    /*
    glm::vec3 position = glm::vec3(hit.point.x, hit.point.y+1, hit.point.z);
    
    // Fire emitter
    Emitter* fireEmitter = Particle.CreateEmitter();
    fireEmitter->type = EmitterType::Point;
    fireEmitter->position  = position;
    fireEmitter->direction = glm::vec3(0.0f, 0.0f, 0.0f);
    fireEmitter->scale     = glm::vec3(0.1f, 0.04f, 0.1f);
    fireEmitter->scaleTo   = glm::vec3(1.0008f, 1.0008f, 1.0008f);
    
    fireEmitter->velocity = glm::vec3(0.0f, 0.01f, 0.0f);
    fireEmitter->velocityBias = 0.008f;
    
    fireEmitter->width = 0.1f;
    fireEmitter->height = 0.1f;
    
    fireEmitter->angle = 0.18f;
    fireEmitter->spread = 0.4f;
    
    fireEmitter->colorBegin = Colors.red;
    fireEmitter->colorEnd = Colors.yellow;
    
    fireEmitter->maxParticles = 20;
    fireEmitter->spawnRate = 20;
    
    fireEmitter->heightMinimum = GameWorld.world.waterLevel;
    
    Material* fireEmitterMaterial = fireEmitter->GetMaterial();
    
    //fireEmitterMaterial->EnableBlending();
    //fireEmitterMaterial->shader = Engine.shaders.water;
    
    
    // Smoke emitter test
    
    Emitter* smokeEmitter = Particle.CreateEmitter();
    smokeEmitter->type = EmitterType::Point;
    smokeEmitter->position = position;
    smokeEmitter->direction = glm::vec3(0.0f, 0.0f, 0.0f);
    smokeEmitter->scale     = glm::vec3(0.08f, 0.08f, 0.08f);
    smokeEmitter->scaleTo   = glm::vec3(1.005f, 1.0005f, 1.005f);
    
    float randomX = Random.Range(-1.0f, 1.0f) * 0.0024f;
    float randomZ = Random.Range(-1.0f, 1.0f) * 0.0024f;
    
    smokeEmitter->velocity = glm::vec3(randomX, 0.024f, randomZ);
    smokeEmitter->velocityBias = 0.02f;
    
    smokeEmitter->width = 4;
    smokeEmitter->height = 8;
    
    smokeEmitter->angle = 0.3f;
    smokeEmitter->spread = 0.5f;
    
    smokeEmitter->colorBegin = Colors.dkgray;
    smokeEmitter->colorEnd = Colors.gray;
    
    smokeEmitter->maxParticles = 40;
    smokeEmitter->spawnRate = 16;
    
    smokeEmitter->heightMinimum = GameWorld.world.waterLevel;
    
    Material* smokeEmitterMaterial = smokeEmitter->GetMaterial();
    
    //smokeEmitterMaterial->EnableBlending();
    //smokeEmitterMaterial->shader = Engine.shaders.water;
    */
    
    
    
    
    
    
    //
    // Profiling
    //
    
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
        Engine.console.WriteDialog(21, "Colliders       " + Int.ToString(Engine.mBoxCollider.size()) );
        
    }
    
    
    //
    // Cast a ray from the player
    
    Hit hit;
    float heightStanding = 0.87f;
    
    // Prevent player controller from going under ground
    if (Physics.Raycast(from, glm::vec3(0.0f, -1.0f, 0.0f), 1000, hit, LayerMask::Ground)) {
        
        RigidBody* bodyPtr = Engine.cameraController->GetComponent<RigidBody>();
        
        rp3d::Transform transform = bodyPtr->getTransform();
        rp3d::Vector3 position = transform.getPosition();
        GameObject* chunkObject = (GameObject*)hit.userData;
        
        glm::vec3 chunkPosition = chunkObject->GetPosition();
        
        if (position.y < hit.point.y + heightStanding) {
            position.y = hit.point.y + heightStanding;
            transform.setPosition( position );
            bodyPtr->setTransform( transform );
        }
        
    }
    
    
    //
    // Camera controller movement
    
    if (Engine.cameraController == nullptr) 
        return;
    
    float forceAccelerate = 0.00145f;
    float forceDecelerate = 0.0117f;
    
    Camera* mainCamera = Engine.sceneMain->camera;
    
    if (mainCamera != nullptr) {
        
        // No movement when paused
        if (!Platform.isPaused) {
            
            // WASD Directional
            if (Input.CheckKeyCurrent(VK_W)) {force += mainCamera->forward;}
            if (Input.CheckKeyCurrent(VK_S)) {force -= mainCamera->forward;}
            if (Input.CheckKeyCurrent(VK_A)) {force += mainCamera->right;}
            if (Input.CheckKeyCurrent(VK_D)) {force -= mainCamera->right;}
            
            // Space/Shift Elevation
            if (Input.CheckKeyCurrent(VK_SPACE)) {force += mainCamera->up;}
            if (Input.CheckKeyCurrent(VK_SHIFT)) {force -= mainCamera->up;}
        }
        
        // Apply gravity
        //force -= mainCamera->up;
        
        // Double speed
        if (Input.CheckKeyCurrent(VK_CONTROL)) 
            forceDblTime += 0.24f;
        
        if (forceDblTime > 1.0f) {forceDblTime -= (forceDecelerate * 8.0f);} else {forceDblTime = 1.0f;}
        
        if (forceDblTime > 3.5f) 
            forceDblTime = 3.5f;
        
        // Accelerate
        glm::vec3 forceTotal = force * forceAccelerate * forceDblTime;
        
        // Decelerate
        if ( glm::length(force) >  0.0001f) force -= (force * forceDecelerate);
        if (-glm::length(force) < -0.0001f) force -= (force * forceDecelerate);
        float forceLength = glm::length(forceTotal);
        
        // Prevent drift
        if (forceLength < 0.0001f) 
            return;
        
        Engine.cameraController->AddForce(forceTotal.x, forceTotal.y, forceTotal.z);
        
        // Field of view zoom effect
        float fovPullback = forceLength * 40.0f;
        if (fovPullback > 4.0f) 
            fovPullback = 4.0f;
        Engine.sceneMain->camera->fov = 60 + fovPullback;
        
    }
    
}


void TickUpdate(void) {
    
}

