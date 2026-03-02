#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/Plugins/plugins.h>

bool isProfilerEnabled = false;

extern float playerHeight;
extern std::vector<GameObject*> vehicles;

glm::vec3 force(0);
float forceDblTime=0;

// Player controller physics

const float walkSpeed       = 2.0f;
const float runSpeed        = 4.0f;
const float maxRunSpeed     = 8.5f;

const float inputSmoothRate = 14.0f;
const float maxSpeedFactor  = 1.6f;

// Walk/run acceleration
const float walkAccel       = 40.0f;           // ramp into walk
const float runAccel        = 55.0f;           // ramp into run / sprint
const float decel           = 25.0f;           // when changing from faster to slower
const float stopDecel       = 55.0f;           // when releasing movement
const float maxBoostTaps    = 3.0f;            // taps to reach maxRunSpeed

// Slope angle
const float slopeLimitDegrees = 60.0f;         // max walkable angle
const float slideMinSpeed     = 2.0f;          // slide speed at slopeLimit
const float slideMaxSpeed     = 8.0f;          // slide speed at vertical cliff

// Height stepping
const float maxStepHeight     = 0.5f;          // max height we can step up
const float stepCheckDistance = 0.4f;          // how far ahead we probe
const float minStepHeight     = 0.01f;         // ignore tiny noise

const float groundSkin        = 0.05f;
const float maxUpCorrection   = 0.3f;          // max upward snap per frame

// Decay timing
const float decayInterval          = 0.016f;   // Interval between decay ticks (seconds)

// Decay per tick
const float decayWhileRunningTick  = 0.01f;
const float decayWhenNotSprintTick = 0.03f;

// Sprint reset
const float sprintGraceDuration    = 0.30f;    // time window after releasing shift
const float sprintResetDelay       = 0.75f;    // how long shift must be "off" before we fully reset boost



bool useGodMode = true;
const float godModeSpeedMul = 20.0f;




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

std::vector<GameObject*> projectiles;


void Run() {
    
    Weather.Update();
    //Particle.Update();
    //Vehicular.Update( 1.0f / static_cast<float>(PHYSICS_UPDATES_PER_SECOND) );
    
    if (Engine.cameraController == nullptr) 
        return;
    
    Camera* cameraPtr = Engine.cameraController->GetComponent<Camera>();
    if (cameraPtr == nullptr) 
        return;
    
    glm::vec3 forward = cameraPtr->forward;
    glm::vec3 from = cameraPtr->transform.position;
    
    
    
    float accel=0.0f;
    float decel=0.0f;
    float steer=0.0f;
    
    if (Input.CheckKeyCurrent(VK_A)) steer =  1.0f;
    if (Input.CheckKeyCurrent(VK_D)) steer = -1.0f;
    if (Input.CheckKeyCurrent(VK_W)) accel = 1.0f;
    if (Input.CheckKeyCurrent(VK_S)) decel = 1.0f;
    
    
    
    Vehicle* car = vehicles[0]->GetComponent<Vehicle>();
    if (car == nullptr) 
        return;
    
    car->ApplyEngineForce(accel);
    car->ApplyBreakForce(decel);
    car->ApplySteeringForce(steer);
    //car->SetHandBreakForce(0.0f);    
    
    
    if (Input.CheckMouseLeftPressed()) {
        MouseButtonLeft();
        Input.SetMouseLeftPressed(false);
        
        //glm::vec3 angle = Engine.sceneMain->camera->forward;
        //glm::vec3 position = angle * startingOffset;
        
        
        
        
        
        
        /*
        GameObject* gameObject = Engine.Create<GameObject>();
        gameObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
        gameObject->AddComponent( Engine.CreateComponent<RigidBody>() );
        
        MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
        meshRenderer->mesh = Engine.meshes.sphere;
        meshRenderer->material = Engine.Create<Material>();
        meshRenderer->material->shader = Engine.shaders.color;
        
        meshRenderer->material->EnableCulling();
        meshRenderer->SetBoundingBox({-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f});
        
        meshRenderer->material->diffuse = Colors.gray;
        meshRenderer->material->ambient = Colors.gray;
        
        Engine.sceneMain->AddMeshRendererToSceneRoot(meshRenderer);
        
        rp3d::RigidBody* rigidBody = gameObject->GetComponent<RigidBody>();
        rigidBody->setType(rp3d::BodyType::DYNAMIC);
        rigidBody->setIsActive(true);
        rigidBody->enableGravity(true);
        rigidBody->setIsAllowedToSleep(true);
        
        rp3d::SphereShape* sphereShape = Physics.CreateColliderSphere(0.4f);
        rp3d::Collider* collider = rigidBody->addCollider(sphereShape, rp3d::Transform::identity());
        
        rp3d::Material& material = collider->getMaterial();
        material.setBounciness(1.0f);
        material.setFrictionCoefficient(0.3f);
        
        rigidBody->updateMassPropertiesFromColliders();
        //rigidBody->setLinearDamping(0.002f);
        rigidBody->setMass(2.4f);
        
        Transform* transform = gameObject->GetComponent<Transform>();
        transform->Scale(0.01f, 0.01f, 0.01f);
        
        glm::vec3 angle = glm::normalize(Engine.sceneMain->camera->forward);
        
        float startingOffset = 1.2f;
        glm::vec3 startingHeight = {0.0f, 0.7f, 0.0f};
        gameObject->SetPosition( Engine.cameraController->GetPosition() + (angle * startingOffset) + startingHeight);
        //gameObject->AddForce();
        
        rp3d::Vector3 force(angle.x, angle.y, angle.z);
        rigidBody->applyLocalForceAtCenterOfMass(force * 8000.0f);
        
        projectiles.push_back(gameObject);
        */
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
    
    if (isProfilerEnabled) {
        glm::vec3 playerPos(0);
        if (Engine.sceneMain != nullptr) 
            if (Engine.sceneMain->camera != nullptr) 
                playerPos = Engine.sceneMain->camera->transform.position;
        
        Engine.console.WriteDialog(0, "player      " + Float.ToString( playerPos.x ) + ", " + Float.ToString( playerPos.y ) + ", " + Float.ToString( playerPos.z ));
        
        // Get chunk info
        /*
        glm::vec3 chunkPosition(0);
        Hit hit;
        if (Physics.Raycast(from, glm::vec3(0.0f, -1.0f, 0.0f), 1000, hit, LayerMask::Ground)) {
            GameObject* chunkObject = (GameObject*)hit.userData;
            chunkPosition = chunkObject->GetPosition();
            Engine.console.WriteDialog(1, "chunk       " + Float.ToString( chunkPosition.x ) + "_" + Float.ToString( chunkPosition.z ));
        }
        */
        
        Engine.console.WriteDialog(9,  "AI          " + Float.ToString( Profiler.profileActorAI ) );
        Engine.console.WriteDialog(10, "Physics     " + Float.ToString( Profiler.profilePhysicsSystem ) );
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
    // Player controller direct movement
    
    if (!useGodMode) {
        RigidBody* bodyPtr = Engine.cameraController->GetComponent<RigidBody>();
        if (bodyPtr != nullptr) {
            rp3d::Transform transform = bodyPtr->getTransform();
            rp3d::Vector3 bodyPos     = transform.getPosition();
            rp3d::Vector3 vel         = bodyPtr->getLinearVelocity();
            
            glm::vec3 origin(bodyPos.x, bodyPos.y, bodyPos.z);
            
            Hit   hit;
            const float centerToFeet = (0.1 + playerHeight * 0.5) + 0.5f;
            
            // Check grounded
            bool hasGround = Physics.Raycast(origin,
                                            glm::vec3(0.0f, -1.0f, 0.0f),
                                            1000.0f,
                                            hit,
                                            LayerMask::Ground);
            
            if (hasGround) {
                float groundY       = hit.point.y;
                float feetY         = bodyPos.y - centerToFeet;
                float allowedMinY   = groundY - groundSkin;
                
                // If our feet are below the tolerated ground level, push up
                if (feetY < allowedMinY) {
                    float correction = allowedMinY - feetY;
                    if (correction > maxUpCorrection) {
                        correction = maxUpCorrection;
                    }
                    
                    bodyPos.y += correction;
                    transform.setPosition(bodyPos);
                    bodyPtr->setTransform(transform);
                    
                    // Avoid slow re-sinking
                    if (vel.y < 0.0f) {
                        vel.y = 0.0f;
                        bodyPtr->setLinearVelocity(vel);
                    }
                }
                
                //
                // Step offset handling
                
                vel = bodyPtr->getLinearVelocity();
                glm::vec2 horizVel(vel.x, vel.z);
                float horizSpeed = glm::length(horizVel);
                if (horizSpeed > 0.01f) {
                    
                    glm::vec2 moveDir2 = horizVel / horizSpeed;
                    glm::vec3 stepDir(moveDir2.x, 0.0f, moveDir2.y);
                    
                    glm::vec3 probeStart = glm::vec3(bodyPos.x, groundY + maxStepHeight, bodyPos.z)
                                        + stepDir * stepCheckDistance;
                    
                    Hit forwardHit;
                    bool hasForwardGround = Physics.Raycast(probeStart,
                                                            glm::vec3(0.0f, -1.0f, 0.0f),
                                                            maxStepHeight * 2.0f,
                                                            forwardHit,
                                                            LayerMask::Ground);
                    
                    if (hasForwardGround) {
                        float targetGroundY = forwardHit.point.y;
                        float deltaY        = targetGroundY - groundY;
                        
                        // If the ground ahead is higher but not too high, step up
                        if (deltaY > minStepHeight && deltaY <= maxStepHeight) {
                            float newGroundY = targetGroundY;
                            float newCenterY = newGroundY + centerToFeet;
                            
                            bodyPos.y = newCenterY;
                            transform.setPosition(bodyPos);
                            bodyPtr->setTransform(transform);
                            
                            // Reset downward velocity on step up
                            vel = bodyPtr->getLinearVelocity();
                            if (vel.y < 0.0f) {
                                vel.y = 0.0f;
                                bodyPtr->setLinearVelocity(vel);
                            }
                        }
                    }
                }
            }
        }
    }
    
    //
    // Camera controller movement
    //
    
    if (Engine.cameraController == nullptr) 
        return;
    
    Camera* mainCamera = Engine.sceneMain->camera;
    if (mainCamera != nullptr) {
        
        float dt = 1.0f / PHYSICS_UPDATES_PER_SECOND;
        
        //
        // God mode free-fly noclip
        if (useGodMode) {
            // Directions based on camera orientation (full 3D, including pitch)
            glm::vec3 fwd  = glm::normalize(mainCamera->forward);
            glm::vec3 right = glm::normalize(mainCamera->right);
            glm::vec3 up    = glm::normalize(mainCamera->up);
            
            glm::vec3 moveDir(0.0f);
            
            if (!Platform.isPaused) {
                if (Input.CheckKeyCurrent(VK_W))       moveDir += fwd;
                if (Input.CheckKeyCurrent(VK_S))       moveDir -= fwd;
                if (Input.CheckKeyCurrent(VK_A))       moveDir += right;
                if (Input.CheckKeyCurrent(VK_D))       moveDir -= right;
                if (Input.CheckKeyCurrent(VK_SPACE))   moveDir += up;
                if (Input.CheckKeyCurrent(VK_CONTROL)) moveDir -= up;
            }
            
            bool hasInput = (glm::length(moveDir) > 0.0001f);
            if (hasInput) {
                moveDir = glm::normalize(moveDir);
            } else {
                moveDir = glm::vec3(0.0f);
            }
            
            // Simple speed: walkSpeed vs runSpeed, with Shift as sprint
            bool shiftPressed =
                Input.CheckKeyCurrent(VK_SHIFT)  ||
                Input.CheckKeyCurrent(VK_LSHIFT) ||
                Input.CheckKeyCurrent(VK_RSHIFT);
            
            float baseSpeed = shiftPressed ? runSpeed : walkSpeed;
            float flySpeed  = baseSpeed;
            
            // Optionally you can scale by maxRunSpeed to go stupid fast:
            // float flySpeed = baseSpeed + (maxRunSpeed - baseSpeed) * 0.75f;
            
            rp3d::RigidBody* bodyPtrGod = Engine.cameraController->GetComponent<RigidBody>();
            if (bodyPtrGod != nullptr) {
                rp3d::Transform t = bodyPtrGod->getTransform();
                rp3d::Vector3 p   = t.getPosition();
                
                glm::vec3 pos(p.x, p.y, p.z);
                pos += moveDir * flySpeed * dt * godModeSpeedMul;
                
                t.setPosition(rp3d::Vector3(pos.x, pos.y, pos.z));
                bodyPtrGod->setTransform(t);
                
                // Zero out velocity so physics doesn't interfere
                bodyPtrGod->setLinearVelocity(rp3d::Vector3(0.0f, 0.0f, 0.0f));
            }
            
            // Skip grounded movement / slope / sliding completely in god mode
            return;
        }
        
        // Tap-sprint / boost state
        static float decayTimer = 0.0f;
        static float sprintBoostLevel   = 0.0f;   // number of effective "taps"
        static bool  lastShiftPressed   = false;
        static float sprintGraceTimer   = 0.0f;   // keeps sprinting briefly after release
        static float shiftReleaseTimer  = 0.0f;   // how long shift has been fully released
        
        // Build flat forward/right so we only move in XZ
        glm::vec3 flatForward(mainCamera->forward.x, 0.0f, mainCamera->forward.z);
        glm::vec3 flatRight  (mainCamera->right.x,   0.0f, mainCamera->right.z);
        
        if (glm::length(flatForward) > 0.0001f) flatForward = glm::normalize(flatForward);
        if (glm::length(flatRight)   > 0.0001f) flatRight   = glm::normalize(flatRight);
        
        glm::vec3 inputDir(0.0f);
        
        if (!Platform.isPaused) {
            if (Input.CheckKeyCurrent(VK_W)) inputDir += flatForward;
            if (Input.CheckKeyCurrent(VK_S)) inputDir -= flatForward;
            if (Input.CheckKeyCurrent(VK_A)) inputDir += flatRight;
            if (Input.CheckKeyCurrent(VK_D)) inputDir -= flatRight;
            if (Input.CheckKeyCurrent(VK_SPACE))   inputDir += mainCamera->up;
            if (Input.CheckKeyCurrent(VK_CONTROL)) inputDir -= mainCamera->up;
        }
        
        bool hasMoveInput = (glm::length(inputDir) > 0.0001f);
        if (hasMoveInput) {
            inputDir = glm::normalize(inputDir);
        } else {
            inputDir = glm::vec3(0.0f);
        }
        
        // Smooth input direction over time (horizontal only)
        float inputLerp = glm::clamp(inputSmoothRate * dt, 0.0f, 1.0f);
        force += (inputDir - force) * inputLerp;
        force.y = 0.0f; // no vertical
        
        bool shiftPressed = Input.CheckKeyCurrent(VK_SHIFT) || Input.CheckKeyCurrent(VK_LSHIFT) || Input.CheckKeyCurrent(VK_RSHIFT);
        bool wantsRun = shiftPressed && hasMoveInput;
        
        // Detect run tap edges while moving
        if (wantsRun && !lastShiftPressed) {
            sprintBoostLevel += 1.0f;
            if (sprintBoostLevel > maxBoostTaps) {
                sprintBoostLevel = maxBoostTaps;
            }
        }
        
        // On shift release, start a grace period where we keep sprinting
        if (!shiftPressed && lastShiftPressed && hasMoveInput) {
            sprintGraceTimer = sprintGraceDuration;
        }
        lastShiftPressed = shiftPressed;
        
        // Update grace timer
        if (sprintGraceTimer > 0.0f) {
            sprintGraceTimer -= dt;
            if (sprintGraceTimer < 0.0f) sprintGraceTimer = 0.0f;
        }
        
        // Are we considered in "run mode" / "sprint mode" right now
        bool inSprintMode = (shiftPressed || sprintGraceTimer > 0.0f) && hasMoveInput;
        
        // Track how long shift has been fully "off"
        bool anySprintState = shiftPressed || (sprintGraceTimer > 0.0f);
        if (anySprintState) {
            shiftReleaseTimer = 0.0f;
        } else {
            shiftReleaseTimer += dt;
            // If shift has been off long enough, kill any remaining boost
            if (shiftReleaseTimer > sprintResetDelay) {
                sprintBoostLevel = 0.0f;
            }
        }
        
        // Always decay boost, but rate depends on mode
        if (sprintBoostLevel > 0.0f) {
            decayTimer += dt;
            float tickRate = inSprintMode ? decayWhileRunningTick : decayWhenNotSprintTick;
            
            // Apply decay only whenever the interval elapses
            while (decayTimer >= decayInterval) {
                sprintBoostLevel -= tickRate;
                decayTimer -= decayInterval;
            }
            
            if (sprintBoostLevel < 0.0f)
                sprintBoostLevel = 0.0f;
        }
        else {
            decayTimer = 0.0f; // reset timer when no boost left
        }
        
        // Compute boost factor [0,1]
        float boostFactor = sprintBoostLevel / maxBoostTaps;
        boostFactor = glm::clamp(boostFactor, 0.0f, 1.0f);
        
        // Are we in run mode for speed selection?
        bool inRunMode = hasMoveInput && (shiftPressed || sprintGraceTimer > 0.0f);
        
        // Choose base speed depending on mode
        float baseSpeed = inRunMode ? runSpeed : walkSpeed;
        
        // Apply boost on top of base speed
        float boostedSpeed = baseSpeed + (maxRunSpeed - baseSpeed) * boostFactor;
        
        // Never let boosted speed go below baseSpeed
        if (boostedSpeed < baseSpeed) 
            boostedSpeed = baseSpeed;
        
        float targetSpeed = 0.0f;
        if (hasMoveInput) {
            targetSpeed = boostedSpeed;
        } else {
            targetSpeed = 0.0f;
        }
        
        static bool speedInit = false;
        if (!speedInit) {
            forceDblTime = 0.0f;
            speedInit = true;
        }
        
        float currentSpeed = forceDblTime;
        
        if (hasMoveInput) {
            float accel = inRunMode ? runAccel : walkAccel;
            
            if (currentSpeed < targetSpeed) {
                currentSpeed += accel * dt;
                if (currentSpeed > targetSpeed) currentSpeed = targetSpeed;
            } else if (currentSpeed > targetSpeed) {
                // Decelerate down toward target
                currentSpeed -= decel * dt;
                if (currentSpeed < targetSpeed) currentSpeed = targetSpeed;
            }
        } else {
            // No input: glide to stop
            if (currentSpeed > 0.0f) {
                currentSpeed -= stopDecel * dt;
                if (currentSpeed < 0.0f) currentSpeed = 0.0f;
            }
        }
        
        forceDblTime = currentSpeed;
        
        // Desired horizontal velocity
        glm::vec3 desiredVelXZ(0.0f);
        glm::vec3 forceXZ(force.x, 0.0f, force.z);
        if (glm::length(forceXZ) > 0.0001f && currentSpeed > 0.0f) {
            desiredVelXZ = glm::normalize(forceXZ) * currentSpeed;
        }
        
        // Apply to rigid body – only modify X/Z, keep Y free for gravity/jumps
        rp3d::RigidBody* bodyPtr = Engine.cameraController->GetComponent<RigidBody>();
        if (bodyPtr != nullptr) {
            rp3d::Vector3 currentVel = bodyPtr->getLinearVelocity();
            rp3d::Vector3 newVel = currentVel;
            
            rp3d::Vector3 targetVelXZ(desiredVelXZ.x, 0.0f, desiredVelXZ.z);
            float velLerp = glm::clamp(inputSmoothRate * dt, 0.0f, 1.0f);
            
            newVel.x = currentVel.x + (targetVelXZ.x - currentVel.x) * velLerp;
            newVel.z = currentVel.z + (targetVelXZ.z - currentVel.z) * velLerp;
            
            // Clamp horizontal speed to a bit above current target
            glm::vec2 horiz(newVel.x, newVel.z);
            float horizLen = glm::length(horiz);
            float maxAllowedSpeed = (hasMoveInput ? targetSpeed : runSpeed) * maxSpeedFactor;
            if (horizLen > maxAllowedSpeed && horizLen > 0.0001f) {
                glm::vec2 clamped = (horiz / horizLen) * maxAllowedSpeed;
                newVel.x = clamped.x;
                newVel.z = clamped.y;
            }
            
            // Extra damping if totally no input
            if (!hasMoveInput) {
                float stopFactor = glm::clamp(8.0f * dt, 0.0f, 1.0f);
                newVel.x *= (1.0f - stopFactor);
                newVel.z *= (1.0f - stopFactor);
            }

            //
            // Slope sliding
            //
            // Cast a short ray down to get ground normal
            rp3d::Transform t = bodyPtr->getTransform();
            rp3d::Vector3 p   = t.getPosition();
            glm::vec3 rayOrigin(p.x, p.y, p.z);
            
            Hit slopeHit;
            bool onGround = Physics.Raycast(rayOrigin,
                                            glm::vec3(0.0f, -1.0f, 0.0f),
                                            2.0f, 
                                            slopeHit,
                                            LayerMask::Ground);
            
            if (onGround) {
                glm::vec3 groundNormal = slopeHit.normal;
                glm::vec3 up(0.0f, 1.0f, 0.0f);
                
                float cosSlope = glm::dot(groundNormal, up);
                if (cosSlope < 1.0f) {
                    cosSlope = glm::clamp(cosSlope, -1.0f, 1.0f);
                    float slopeAngle = glm::degrees(std::acos(cosSlope));
                    
                    if (slopeAngle > slopeLimitDegrees) {
                        // Only slide if we're not jumping upward strongly
                        if (newVel.y <= 0.5f) {
                            // Project gravity onto the slope plane to get slide direction
                            glm::vec3 gravityDir(0.0f, -1.0f, 0.0f);
                            float gDotN = glm::dot(gravityDir, groundNormal);
                            glm::vec3 slideDir = gravityDir - groundNormal * gDotN;
                            
                            float slideLen = glm::length(slideDir);
                            if (slideLen > 0.0001f) {
                                slideDir /= slideLen;
                                
                                // Scale slide speed based on steepness
                                float tSteep = (slopeAngle - slopeLimitDegrees) / (90.0f - slopeLimitDegrees);
                                if (tSteep < 0.0f) tSteep = 0.0f;
                                if (tSteep > 1.0f) tSteep = 1.0f;
                                
                                float slideSpeed = slideMinSpeed + (slideMaxSpeed - slideMinSpeed) * tSteep;
                                
                                // If player is pushing uphill reduce sliding a bit
                                if (hasMoveInput) {
                                    glm::vec2 inputDir2(inputDir.x, inputDir.z);
                                    if (glm::length(inputDir2) > 0.0001f) {
                                        inputDir2 = glm::normalize(inputDir2);
                                        glm::vec2 slideDir2(slideDir.x, slideDir.z);
                                        
                                        float opposeDot = glm::dot(inputDir2, -slideDir2);
                                        if (opposeDot > 0.2f) {
                                            // They're trying to walk uphill: weaken slide
                                            slideSpeed *= 0.5f;
                                        }
                                    }
                                }
                                
                                // Apply slide horizontally, keep whatever Y physics wants
                                glm::vec3 slideVel = slideDir * slideSpeed;
                                
                                newVel.x = slideVel.x;
                                newVel.z = slideVel.z;
                            }
                        }
                    }
                }
            }
            
            bodyPtr->setLinearVelocity(newVel);
        }
    }
    
    
    
    
}


void TickUpdate(void) {
    
}

