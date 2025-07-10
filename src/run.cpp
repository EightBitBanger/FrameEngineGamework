#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/plugins.h>

void ApplyGene(Actor* targetActor) {
    
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetName("Ox");
    
    targetActor->physical.SetAdultAge(800);
    targetActor->physical.SetSeniorAge(5000);
    
    targetActor->physical.SetSpeed(1.0);
    targetActor->physical.SetSpeedYouth(0.9);
    targetActor->physical.SetSpeedMultiplier(1.2f);
    
    targetActor->physical.SetYouthScale(0.4f);
    targetActor->physical.SetAdultScale(1.2f);
    
    targetActor->behavior.SetHeightPreferenceMax(20.0f);
    
    targetActor->biological.health = 170;
    
    if (Random.Range(0, 100) > 55) 
    {targetActor->physical.SetSexualOrientation(true);} else  // Male
    {targetActor->physical.SetSexualOrientation(false);}      // Female
    
    // Color variants
    Color headColor  = Colors.dkgray;
    Color bodyColor  = Colors.dkgray;
    Color limbColor  = Colors.dkgray;
    Color hornColor  = Colors.MakeGrayScale(0.037);
    Color udderColor = Colors.Lerp(Colors.MakeGrayScale(0.1), Colors.red, 0.2);
    
    headColor *= Colors.MakeRandomGrayScale();
    bodyColor *= Colors.MakeRandomGrayScale();
    limbColor *= Colors.MakeRandomGrayScale();
    udderColor *= Colors.MakeGrayScale(0.3);
    
    if (Random.Range(0, 100) > 50) headColor *= Colors.brown;
    if (Random.Range(0, 100) > 50) bodyColor *= Colors.brown;
    if (Random.Range(0, 100) > 50) limbColor *= Colors.brown;
    
    // Body
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 0.9, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.5, 0.5, 1.2);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    
    // Head
    Gene geneHead;
    geneHead.offset    = Codon(0, 1.1, 0.45);
    geneHead.position  = Codon(0, 0, 0.35);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(0.45, 0.42, 0.4);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    
    // Horns
    Gene geneHornLeft;
    geneHornLeft.attachmentIndex = 2;
    geneHornLeft.offset    = Codon(0, 0, 0);
    geneHornLeft.position  = Codon(0.25, 0.2, 0.22);
    geneHornLeft.rotation  = Codon(0, 0, -0.3);
    geneHornLeft.scale     = Codon(0.4, 0.1, 0.1);
    geneHornLeft.color.x   = hornColor.r;
    geneHornLeft.color.y   = hornColor.g;
    geneHornLeft.color.z   = hornColor.b;
    geneHornLeft.type = EXPRESSION_TYPE_MALE;
    geneHornLeft.expressionFactor = 1.0f;
    
    Gene geneHornLeftUpper;
    geneHornLeftUpper.attachmentIndex = 2;
    geneHornLeftUpper.offset    = Codon(0, 0, 0);
    geneHornLeftUpper.position  = Codon(0.5, 0.2, 0.22);
    geneHornLeftUpper.rotation  = Codon(0, 0, 0.75);
    geneHornLeftUpper.scale     = Codon(0.2, 0.1, 0.1);
    geneHornLeftUpper.color.x   = hornColor.r;
    geneHornLeftUpper.color.y   = hornColor.g;
    geneHornLeftUpper.color.z   = hornColor.b;
    geneHornLeftUpper.type = EXPRESSION_TYPE_MALE;
    geneHornLeftUpper.expressionFactor = 1.0f;
    
    Gene geneHornRight;
    geneHornRight.attachmentIndex = 2;
    geneHornRight.offset    = Codon(0, 0, 0);
    geneHornRight.position  = Codon(-0.25, 0.2, 0.22);
    geneHornRight.rotation  = Codon(0, 0, 0.3);
    geneHornRight.scale     = Codon(0.4, 0.1, 0.1);
    geneHornRight.color.x   = hornColor.r;
    geneHornRight.color.y   = hornColor.g;
    geneHornRight.color.z   = hornColor.b;
    geneHornRight.type = EXPRESSION_TYPE_MALE;
    geneHornRight.expressionFactor = 1.0f;
    
    Gene geneHornRightUpper;
    geneHornRightUpper.attachmentIndex = 2;
    geneHornRightUpper.offset    = Codon(0, 0, 0);
    geneHornRightUpper.position  = Codon(-0.5, 0.2, 0.22);
    geneHornRightUpper.rotation  = Codon(0, 0, -0.75);
    geneHornRightUpper.scale     = Codon(0.2, 0.1, 0.1);
    geneHornRightUpper.color.x   = hornColor.r;
    geneHornRightUpper.color.y   = hornColor.g;
    geneHornRightUpper.color.z   = hornColor.b;
    geneHornRightUpper.type = EXPRESSION_TYPE_MALE;
    geneHornRightUpper.expressionFactor = 1.0f;
    
    // Udders
    Gene geneUdders;
    geneUdders.offset    = Codon(0, 0, 0);
    geneUdders.position  = Codon(0, 0.68, -0.3);
    geneUdders.rotation  = Codon(0, 0, 0);
    geneUdders.scale     = Codon(0.2, 0.3, 0.24);
    geneUdders.color.x   = udderColor.r;
    geneUdders.color.y   = udderColor.g;
    geneUdders.color.z   = udderColor.b;
    geneUdders.type = EXPRESSION_TYPE_FEMALE;
    geneUdders.expressionFactor = 1.0f;
    
    // Front Left Leg
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = Codon(0.22, 0.75, 0.4);
    geneLimbFrontLeft.position  = Codon(0, -0.4, 0);
    geneLimbFrontLeft.rotation  = Codon(0, 0, 0);
    geneLimbFrontLeft.scale     = Codon(0.2, 0.7, 0.2);
    geneLimbFrontLeft.color.x   = limbColor.r;
    geneLimbFrontLeft.color.y   = limbColor.g;
    geneLimbFrontLeft.color.z   = limbColor.b;
    geneLimbFrontLeft.animationType = ActorState::Animation::Limb;
    geneLimbFrontLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbFrontLeft.animationRange   = 10;
    
    // Front Right Leg
    Gene geneLimbFrontRight;
    geneLimbFrontRight.offset    = Codon(-0.22, 0.75, 0.4);
    geneLimbFrontRight.position  = Codon(0, -0.4, 0);
    geneLimbFrontRight.rotation  = Codon(0, 0, 0);
    geneLimbFrontRight.scale     = Codon(0.2, 0.7, 0.2);
    geneLimbFrontRight.color.x   = limbColor.r;
    geneLimbFrontRight.color.y   = limbColor.g;
    geneLimbFrontRight.color.z   = limbColor.b;
    geneLimbFrontRight.animationType = ActorState::Animation::Limb;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = Codon(1, 0, 0);
    geneLimbFrontRight.animationRange     = 10;
    
    // Rear Left Leg
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = Codon(0.22, 0.75, -0.45);
    geneLimbRearLeft.position  = Codon(0, -0.4, 0);
    geneLimbRearLeft.rotation  = Codon(0, 0, 0);
    geneLimbRearLeft.scale     = Codon(0.2, 0.7, 0.2);
    geneLimbRearLeft.color.x   = limbColor.r;
    geneLimbRearLeft.color.y   = limbColor.g;
    geneLimbRearLeft.color.z   = limbColor.b;
    geneLimbRearLeft.animationType = ActorState::Animation::Limb;
    geneLimbRearLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbRearLeft.animationRange   = 10;
    
    // Rear Right Leg
    Gene geneLimbRearRight;
    geneLimbRearRight.offset    = Codon(-0.22, 0.75, -0.45);
    geneLimbRearRight.position  = Codon(0, -0.4, 0);
    geneLimbRearRight.rotation  = Codon(0, 0, 0);
    geneLimbRearRight.scale     = Codon(0.2, 0.7, 0.2);
    geneLimbRearRight.color.x   = limbColor.r;
    geneLimbRearRight.color.y   = limbColor.g;
    geneLimbRearRight.color.z   = limbColor.b;
    geneLimbRearRight.animationType = ActorState::Animation::Limb;
    geneLimbRearRight.doInverseAnimation = true;
    geneLimbRearRight.animationAxis      = Codon(1, 0, 0);
    geneLimbRearRight.animationRange     = 10;
    
    // Tail
    Gene geneTail;
    geneTail.offset    = Codon(0, 0.85, -0.60);
    geneTail.position  = Codon(0, 0, 0);
    geneTail.rotation  = Codon(0, 0, 0);
    geneTail.scale     = Codon(0.1, 0.52, 0.1);
    geneTail.color.x   = bodyColor.r;
    geneTail.color.y   = bodyColor.g;
    geneTail.color.z   = bodyColor.b;
    
    // Apply genes to the actor
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneHornLeft);
    targetActor->genetics.AddGene(geneHornLeftUpper);
    targetActor->genetics.AddGene(geneHornRight);
    targetActor->genetics.AddGene(geneHornRightUpper);
    targetActor->genetics.AddGene(geneUdders);
    targetActor->genetics.AddGene(geneLimbFrontLeft);
    targetActor->genetics.AddGene(geneLimbFrontRight);
    targetActor->genetics.AddGene(geneLimbRearLeft);
    targetActor->genetics.AddGene(geneLimbRearRight);
    targetActor->genetics.AddGene(geneTail);
    
    
}


Actor* actorInSights = nullptr;






bool isProfilerEnabled = false;

glm::vec3 force(0);
float forceDblTime=0;

bool isDebugReportActive = false;
bool isFullScreen = false;

std::string targetGene = "";

Actor* actorSelected = nullptr;

bool isNetInit = false;
bool isTrigger = false;

AudioSample* sample;
Sound* sound;

void Run() {
    
    // Update plug-in systems
    Weather.Update();
    Particle.Update();
    GameWorld.Update();
    
    sample = Audio.CreateAudioSample();
    Audio.Presets.RenderWhiteNoise(sample, 0.8f);
    
    if (!isTrigger) {
        isTrigger = true;
        sample = Audio.CreateAudioSample();
        Audio.Presets.RenderWhiteNoise(sample, 0.1f);
    }
    
    
    // Cast a ray from the player
    
    Hit hit;
    
    Camera* cameraPtr = Engine.cameraController->GetComponent<Camera>();
    
    glm::vec3 from    = cameraPtr->transform.position;
    glm::vec3 forward = cameraPtr->forward;
    
    // Prevent player controller from going under ground
    if (Physics.Raycast(from, glm::vec3(0.0f, -1.0f, 0.0f), 1000, hit, LayerMask::Ground)) {
        
        RigidBody* bodyPtr = Engine.cameraController->GetComponent<RigidBody>();
        
        rp3d::Transform transform = bodyPtr->getTransform();
        rp3d::Vector3 position = transform.getPosition();
        
        if (position.y < hit.point.y) {
            position.y = hit.point.y;
            transform.setPosition( position );
            bodyPtr->setTransform( transform );
        }
        
    }
    
    
    
    //
    // DEBUG - Show actor stats
    
    if (actorInSights != nullptr) {
        Engine.console.WriteDialog( 1, actorInSights->GetName() );
        Engine.console.WriteDialog( 2, Int.ToString( actorInSights->physical.GetAge() ) );
        
        Engine.console.WriteDialog( 4, "Active   " + Int.ToString( actorInSights->isActive ) );
        Engine.console.WriteDialog( 5, "Garbage  " + Int.ToString( actorInSights->isGarbage ) );
        
        Engine.console.WriteDialog( 7, "[ States ]" );
        
        switch (actorInSights->state.current) {
            case ActorState::State::None:    Engine.console.WriteDialog(8, "State  None"); break;
            case ActorState::State::Attack:  Engine.console.WriteDialog(8, "State  Attack"); break;
            case ActorState::State::Flee:    Engine.console.WriteDialog(8, "State  Flee"); break;
            case ActorState::State::Defend:  Engine.console.WriteDialog(8, "State  Defend"); break;
            case ActorState::State::Focus:   Engine.console.WriteDialog(8, "State  Focus"); break;
            case ActorState::State::Look:    Engine.console.WriteDialog(8, "State  Look"); break;
        }
        
        switch (actorInSights->state.mode) {
            case ActorState::Mode::Idle:        Engine.console.WriteDialog(9, "Mode   Idle"); break;
            case ActorState::Mode::Sleeping:    Engine.console.WriteDialog(9, "Mode   Sleeping"); break;
            case ActorState::Mode::MoveRandom:  Engine.console.WriteDialog(9, "Mode   MoveRandom"); break;
            case ActorState::Mode::MoveTo:      Engine.console.WriteDialog(9, "Mode   MoveTo"); break;
            case ActorState::Mode::WalkTo:      Engine.console.WriteDialog(9, "Mode   WalkTo"); break;
            case ActorState::Mode::RunTo:       Engine.console.WriteDialog(9, "Mode   RunTo"); break;
            
        }
        
        Engine.console.WriteDialog( 11, "[ Counters ]" );
        
        Engine.console.WriteDialog( 12, "Attack    " + Int.ToString( actorInSights->counters.GetCoolDownAttack() ) );
        Engine.console.WriteDialog( 13, "Movement  " + Int.ToString( actorInSights->counters.GetCoolDownMovement() ) );
        Engine.console.WriteDialog( 14, "Observe   " + Int.ToString( actorInSights->counters.GetCoolDownObservation() ) );
        Engine.console.WriteDialog( 15, "Breeding  " + Int.ToString( actorInSights->counters.GetCoolDownBreeding() ) );
        
        Engine.console.WriteDialog( 17, "Distance to target   " + Int.ToString( actorInSights->navigation.GetDistanceToTarget() ) );
        
        Engine.console.WriteDialog( 19, "[ Vitality ]" );
        Engine.console.WriteDialog( 20, "Health   " + Int.ToString( actorInSights->biological.health ) );
        
        Engine.console.WriteDialog( 22, "Target list    " + Int.ToString( actorInSights->behavior.GetNumberOfTargets() ) );
        
        
    } else {
        
        for (unsigned int i=0; i < 20; i++) 
            Engine.console.WriteDialog(i, "");
    }
    
    
    
    // Pick an actor
    
    if (Input.CheckMouseLeftPressed()) {
        
        glm::vec3 fromHigh = from;
        if (Physics.Raycast(from, forward, 1000, hit, LayerMask::Actor)) {
            GameObject* hitObject = (GameObject*)hit.gameObject;
            
            if (hitObject != nullptr) {
                Actor* hitActor = hitObject->GetComponent<Actor>();
                actorInSights = hitActor;
            }
        }
        
    }
    
    
    
    
    
    if (Input.CheckMouseMiddlePressed()) {
        if (Physics.Raycast(from, forward, 100, hit, LayerMask::Ground)) {
            float xx = Random.Range(0, 10) - Random.Range(0, 10);
            float zz = Random.Range(0, 10) - Random.Range(0, 10);
            
            Actor* actor = AI.CreateActor();
            
            actor->navigation.SetPosition(glm::vec3(hit.point.x + xx, hit.point.y+5, hit.point.z + zz));
            actor->navigation.SetTargetPoint(glm::vec3(hit.point.x + xx, hit.point.y+5, hit.point.z + zz));
            
            //AI.genomes.presets.Human(actor);
            ApplyGene(actor);
            
            actor->physical.SetAge( Random.Range(0.0f, actor->physical.GetAdultAge()) );
            actor->RebuildGeneticExpression();
            
            actor->isActive = true;
            
            actorInSights = actor;
        }
        
        /*
        // Spawn a tree example
        if (Physics.Raycast(from, forward, 1000, hit, LayerMask::Ground)) {
            
            GameObject* gameObject = (GameObject*)hit.gameObject;
            
            std::vector<std::string> name_pos = String.Explode(gameObject->name, '_');
            float posX = String.ToInt( name_pos[0] );
            float posZ = String.ToInt( name_pos[1] );
            
            Chunk* chunk = GameWorld.FindChunk(posX, posZ);
            
            if (chunk->gameObject != nullptr) {
                
                float chunkPosX = posX - hit.point.x;
                float chunkPosY = hit.point.y;
                float chunkPosZ = posZ - hit.point.z;
                
                GameObject* hitObject = chunk->staticObject;
                
                MeshRenderer* chunkRenderer = hitObject->GetComponent<MeshRenderer>();
                
                StaticObject staticObj;
                staticObj.x = chunkPosX;
                staticObj.y = chunkPosY;
                staticObj.z = chunkPosZ;
                
                GameWorld.AddDecorTree(*chunk, staticObj, chunkRenderer->mesh, -chunkPosX, chunkPosY, -chunkPosZ, Decoration::TreeOak);
                chunkRenderer->mesh->Load();
                
            }
            
        }
        */
        
    }
    
    
    
    //
    // Profiling
    //
    
    
    if (Input.CheckKeyPressed(VK_F4)) {
        isProfilerEnabled = !isProfilerEnabled;
        
        if (!isProfilerEnabled) 
            for (unsigned int i=0; i < PROFILER_NUMBER_OF_ELEMENTS; i++) 
                Engine.console.WriteDialog(i, "");
    }
    
    if (isProfilerEnabled) {
        
        Engine.console.WriteDialog( 0, "[Profiler]" );
        Engine.console.WriteDialog( 1, "AI          " + Float.ToString( Profiler.profileActorAI ) );
        Engine.console.WriteDialog( 2, "Engine      " + Float.ToString( Profiler.profileGameEngineUpdate ) );
        Engine.console.WriteDialog( 3, "Renderer    " + Float.ToString( Profiler.profileRenderSystem ) );
        
        
        Engine.console.WriteDialog( 5, "Draw calls      " + Int.ToString(Renderer.GetNumberOfDrawCalls()) );
        
        Engine.console.WriteDialog( 7, "GameObjects     " + Int.ToString(Engine.GetNumberOfGameObjects()) );
        Engine.console.WriteDialog( 8, "Components      " + Int.ToString(Engine.GetNumberOfComponents()) );
        
        Engine.console.WriteDialog( 9, "MeshRenderers   " + Int.ToString(Renderer.GetNumberOfMeshRenderers()) );
        Engine.console.WriteDialog(10, "Meshes          " + Int.ToString(Renderer.GetNumberOfMeshes()) );
        Engine.console.WriteDialog(11, "Materials       " + Int.ToString(Renderer.GetNumberOfMaterials()) );
        Engine.console.WriteDialog(12, "RigidBodies     " + Int.ToString(Physics.world->getNbRigidBodies()) );
        Engine.console.WriteDialog(13, "Actors          " + Int.ToString(AI.GetNumberOfActors()) );
        
    }
    
    
    
    // Full screen switching
    if (Input.CheckKeyPressed(VK_F11)) {
        isFullScreen = !isFullScreen;
        if (isFullScreen) {
            Platform.WindowEnableFullscreen();
        } else {
            Platform.WindowDisableFullscreen();
        }
    }
    
    
    
    
    
    //
    // Camera controller movement
    //
    
    if (Engine.cameraController == nullptr) 
        return;
    
    float forceAccelerate = 0.0043f;
    float forceDecelerate = 0.015f;
    
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
        
        Engine.cameraController->AddForce(forceTotal.x, forceTotal.y, forceTotal.z);
        
        // Field of view zoom effect
        float fovPullback = glm::length(forceTotal) * 40.0f;
        if (fovPullback > 4.0f) 
            fovPullback = 4.0f;
        Engine.sceneMain->camera->fov = 60 + fovPullback;
        
    }
    
    
    //
    // Escape key pausing
    if (Input.CheckKeyPressed(VK_ESCAPE)) {
        Platform.Pause();
        if (Platform.isPaused) {
            MainMenuEnable();
        } else {
            MainMenuDisable();
        }
    }
    
    
    return;
}


void TickUpdate(void) {
    
    return;
}

