#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/plugins.h>

void ApplyGene(Actor* targetActor) {
    
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetName("Human");
    
    targetActor->physical.SetAdultAge( 1000 );
    targetActor->physical.SetSeniorAge( 40000 );
    
    targetActor->physical.SetSpeed(1.0f);
    targetActor->physical.SetSpeedYouth(1.4f);
    targetActor->physical.SetSpeedMultiplier(1.24f);
    
    targetActor->physical.SetYouthScale(0.3f);
    targetActor->physical.SetAdultScale(1.0f);
    
    targetActor->behavior.SetHeightPreferenceMax(20.0f);
    
    targetActor->biological.health = 200;
    
    if (Random.Range(0, 100) > 55) 
    {targetActor->physical.SetSexualOrientation(true);} else  // Male
    {targetActor->physical.SetSexualOrientation(false);}      // Female
    
    // Color variants
    Color headColor = Colors.Lerp(Colors.white, Colors.yellow, 0.3);
    Color limbColor = Colors.Lerp(Colors.white, Colors.yellow, 0.3);
    Color bodyColor = Colors.Lerp(Colors.white, Colors.yellow, 0.3);
    
    bodyColor = Colors.Lerp(bodyColor, Colors.red, 0.5);
    bodyColor = Colors.Lerp(bodyColor, Colors.brown, 0.9);
    bodyColor = Colors.Lerp(bodyColor, Colors.black, 0.5);
    bodyColor *= Colors.MakeRandomGrayScale() * 0.8f;
    
    // Limb colors
    
    // Yellow/reddish tint
    int race = Random.Range(0, 100);
    
    if (race >= 0 && race < 5) {
        headColor *= Colors.Lerp(headColor, Colors.orange, 0.3);
        limbColor *= Colors.Lerp(limbColor, Colors.orange, 0.3);
        headColor *= Colors.Lerp(headColor, Colors.red, 0.1);
        limbColor *= Colors.Lerp(limbColor, Colors.red, 0.1);
        race = -1;
    }
    
    // Brown tint
    if (race >= 5 && race < 10) {
        headColor = Colors.Lerp(headColor, Colors.brown, 0.4);
        limbColor = Colors.Lerp(limbColor, Colors.brown, 0.4);
        headColor = Colors.Lerp(headColor, Colors.red, 0.087);
        limbColor = Colors.Lerp(limbColor, Colors.red, 0.087);
        headColor = Colors.Lerp(headColor, Colors.black, 0.87);
        limbColor = Colors.Lerp(limbColor, Colors.black, 0.87);
        race = -1;
    }
    
    // Dark tone
    if (race >= 10 && race < 20) {
        headColor = Colors.Lerp(headColor, Colors.black, 0.998);
        limbColor = Colors.Lerp(limbColor, Colors.black, 0.998);
        race = -1;
    }
    
    float headSize   = (Random.Range(0, 99) * 0.0001f);
    float breastSize = (Random.Range(0, 99) * 0.00054f);
    
    // Body
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 0.9, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.37, 0.5, 0.24);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    geneBody.animationType = ActorState::Animation::Body;
    
    // Head
    Gene geneHead;
    geneHead.offset    = Codon(0, 0, 0);
    geneHead.position  = Codon(0, 1.36, 0);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(headSize + 0.3, 0.3, headSize + 0.3);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    geneHead.animationType = ActorState::Animation::Head;
    
    // Neck
    Gene geneNeck;
    geneNeck.offset    = Codon(0, 0, 0);
    geneNeck.position  = Codon(0, 1.2, 0);
    geneNeck.rotation  = Codon(0, 0, 0);
    geneNeck.scale     = Codon(0.18, 0.1, 0.18);
    geneNeck.color.x   = bodyColor.r;
    geneNeck.color.y   = bodyColor.g;
    geneNeck.color.z   = bodyColor.b;
    geneNeck.colorIndex = 1;
    geneNeck.animationType = ActorState::Animation::Head;
    
    // Left breast
    Gene geneBreastLeft;
    geneBreastLeft.offset    = Codon(0.112, 1.03, breastSize + 0.06f);
    geneBreastLeft.position  = Codon(0, 0, 0);
    geneBreastLeft.rotation  = Codon(0, 0, 0);
    geneBreastLeft.scale     = Codon(0.14, 0.14, 0.14);
    geneBreastLeft.color.x   = bodyColor.r;
    geneBreastLeft.color.y   = bodyColor.g;
    geneBreastLeft.color.z   = bodyColor.b;
    geneBreastLeft.type = EXPRESSION_TYPE_FEMALE;
    geneBreastLeft.colorIndex       = 1;
    geneBreastLeft.expressionFactor = 1.5;
    geneBreastLeft.expressionMax    = 1.4;
    geneBreastLeft.expressionAge    = 900;
    
    // Right breast
    Gene geneBreastRight;
    geneBreastRight.offset    = Codon(-0.112, 1.03, breastSize + 0.06f);
    geneBreastRight.position  = Codon(0, 0, 0);
    geneBreastRight.rotation  = Codon(0, 0, 0);
    geneBreastRight.scale     = Codon(0.14, 0.14, 0.14);
    geneBreastRight.color.x   = bodyColor.r;
    geneBreastRight.color.y   = bodyColor.g;
    geneBreastRight.color.z   = bodyColor.b;
    geneBreastRight.type = EXPRESSION_TYPE_FEMALE;
    geneBreastRight.scaleIndex       = 4;
    geneBreastRight.colorIndex       = 1;
    geneBreastRight.expressionFactor = 1.5;
    geneBreastRight.expressionMax    = 1.4;
    geneBreastRight.expressionAge    = 900;
    
    // Left shoulder
    Gene geneShoulderLeft;
    geneShoulderLeft.offset    = Codon(0, 1.09, 0);
    geneShoulderLeft.position  = Codon(0.24, -0.0425, 0);
    geneShoulderLeft.rotation  = Codon(0, 0, 0);
    geneShoulderLeft.scale     = Codon(0.12, 0.24, 0.12);
    geneShoulderLeft.color.x   = limbColor.r;
    geneShoulderLeft.color.y   = limbColor.g;
    geneShoulderLeft.color.z   = limbColor.b;
    geneShoulderLeft.animationAxis    = Codon(1, 0, 0);
    geneShoulderLeft.animationRange   = 13;
    geneShoulderLeft.colorIndex       = 2;
    geneShoulderLeft.type = EXPRESSION_TYPE_MALE;
    geneShoulderLeft.expressionFactor = 1.5;
    geneShoulderLeft.expressionMax    = 1.1;
    geneShoulderLeft.expressionAge    = 900;
    geneShoulderLeft.animationType = ActorState::Animation::Limb;
    
    // Right shoulder
    Gene geneShoulderRight;
    geneShoulderRight.offset    = Codon(0, 1.09, 0);
    geneShoulderRight.position  = Codon(-0.24, -0.0425, 0);
    geneShoulderRight.rotation  = Codon(0, 0, 0);
    geneShoulderRight.scale     = Codon(0.12, 0.24, 0.12);
    geneShoulderRight.color.x   = limbColor.r;
    geneShoulderRight.color.y   = limbColor.g;
    geneShoulderRight.color.z   = limbColor.b;
    geneShoulderRight.doInverseAnimation = true;
    geneShoulderRight.animationAxis      = Codon(1, 0, 0);
    geneShoulderRight.animationRange     = 13;
    geneShoulderRight.colorIndex         = 2;
    geneShoulderRight.type = EXPRESSION_TYPE_MALE;
    geneShoulderRight.expressionFactor = 1.5;
    geneShoulderRight.expressionMax    = 1.1;
    geneShoulderRight.expressionAge    = 900;
    geneShoulderRight.animationType = ActorState::Animation::Limb;
    
    // Left arm
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = Codon(0.24, 1.09, 0);
    geneLimbFrontLeft.position  = Codon(0, -0.24, 0);
    geneLimbFrontLeft.rotation  = Codon(0, 0, 0);
    geneLimbFrontLeft.scale     = Codon(0.1, 0.55, 0.1);
    geneLimbFrontLeft.color.x   = limbColor.r;
    geneLimbFrontLeft.color.y   = limbColor.g;
    geneLimbFrontLeft.color.z   = limbColor.b;
    geneLimbFrontLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbFrontLeft.animationRange   = 13;
    geneLimbFrontLeft.colorIndex       = 6;
    geneLimbFrontLeft.animationType = ActorState::Animation::Limb;
    
    // Right arm
    Gene geneLimbFrontRight;
    geneLimbFrontRight.offset    = Codon(-0.24, 1.09, 0);
    geneLimbFrontRight.position  = Codon(0, -0.24, 0);
    geneLimbFrontRight.rotation  = Codon(0, 0, 0);
    geneLimbFrontRight.scale     = Codon(0.1, 0.55, 0.1);
    geneLimbFrontRight.color.x   = limbColor.r;
    geneLimbFrontRight.color.y   = limbColor.g;
    geneLimbFrontRight.color.z   = limbColor.b;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = Codon(1, 0, 0);
    geneLimbFrontRight.animationRange     = 13;
    geneLimbFrontRight.colorIndex         = 7;
    geneLimbFrontRight.animationType = ActorState::Animation::Limb;
    
    // Left Leg
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = Codon(0.12, 0.6, 0);
    geneLimbRearLeft.position  = Codon(0, -0.24, 0);
    geneLimbRearLeft.rotation  = Codon(0, 0, 0);
    geneLimbRearLeft.scale     = Codon(0.18, 0.61, 0.18);
    geneLimbRearLeft.color.x   = limbColor.r;
    geneLimbRearLeft.color.y   = limbColor.g;
    geneLimbRearLeft.color.z   = limbColor.b;
    geneLimbRearLeft.doInverseAnimation = true;
    geneLimbRearLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbRearLeft.animationRange   = 13;
    geneLimbRearLeft.animationType = ActorState::Animation::Limb;
    
    // Right Leg
    Gene geneLimbRearRight;
    geneLimbRearRight.offset    = Codon(-0.12, 0.6, 0);
    geneLimbRearRight.position  = Codon(0, -0.24 , 0);
    geneLimbRearRight.rotation  = Codon(0, 0, 0);
    geneLimbRearRight.scale     = Codon(0.18, 0.61, 0.18);
    geneLimbRearRight.color.x   = limbColor.r;
    geneLimbRearRight.color.y   = limbColor.g;
    geneLimbRearRight.color.z   = limbColor.b;
    geneLimbRearRight.animationAxis      = Codon(1, 0, 0);
    geneLimbRearRight.animationRange     = 13;
    geneLimbRearRight.animationType = ActorState::Animation::Limb;
    
    // Apply genes to the actor
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneNeck);
    targetActor->genetics.AddGene(geneBreastLeft);
    targetActor->genetics.AddGene(geneBreastRight);
    targetActor->genetics.AddGene(geneShoulderLeft);
    targetActor->genetics.AddGene(geneShoulderRight);
    targetActor->genetics.AddGene(geneLimbFrontLeft);
    targetActor->genetics.AddGene(geneLimbFrontRight);
    targetActor->genetics.AddGene(geneLimbRearLeft);
    targetActor->genetics.AddGene(geneLimbRearRight);
    
    
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

