#include <GameEngineFramework/Engine/Engine.h>

ENGINE_API extern EngineComponents  Components;
ENGINE_API extern ColorPreset       Colors;
ENGINE_API extern NumberGeneration  Random;
ENGINE_API extern Logger            Log;
ENGINE_API extern Timer             PhysicsTime;
ENGINE_API extern Timer             Time;

ENGINE_API extern Serialization     Serializer;
ENGINE_API extern ResourceManager   Resources;
ENGINE_API extern ScriptSystem      Scripting;
ENGINE_API extern RenderSystem      Renderer;
ENGINE_API extern PhysicsSystem     Physics;
ENGINE_API extern AudioSystem       Audio;
ENGINE_API extern InputSystem       Input;
ENGINE_API extern MathCore          Math;
ENGINE_API extern ActorSystem       AI;

ENGINE_API extern ApplicationLayer      Application;
ENGINE_API extern EngineSystemManager   Engine;



void EngineSystemManager::Update(void) {
    
    // Update player position in the AI simulation
    if (sceneMain != nullptr) {
        
        Camera* activeCamera = sceneMain->camera;
        
        if (activeCamera != nullptr) 
            AI.SetPlayerWorldPosition( activeCamera->transform.position );
        
    }
    
    // Check to update the data stream
    if (doUpdateDataStream) {
        
        doUpdateDataStream = true;
        
        streamSize = mGameObjects.Size();
        
        for (unsigned int i=0; i < streamSize; i++ ) {
            
            streamBuffer[i].gameObject    = mGameObjects[i];
            
            streamBuffer[i].light         = mGameObjects[i]->mLightCache;
            streamBuffer[i].actor         = mGameObjects[i]->mActorCache;
            streamBuffer[i].camera        = mGameObjects[i]->mCameraCache;
            streamBuffer[i].rigidBody     = mGameObjects[i]->mRigidBodyCache;
            streamBuffer[i].meshRenderer  = mGameObjects[i]->mMeshRendererCache;
            
            streamBuffer[i].text          = mGameObjects[i]->mTextCache;
            streamBuffer[i].panel         = mGameObjects[i]->mPanelCache;
            
            continue;
        }
        
    }
    
    
    
    //
    // Run the game object list
    //
    
    for (unsigned int i=0; i < streamSize; i++ ) {
        
        if (!streamBuffer[i].gameObject->isActive) 
            continue;
        
        // Current transform
        Transform currentTransform;
        currentTransform.position    = streamBuffer[i].gameObject->mTransformCache->position;
        currentTransform.orientation = streamBuffer[i].gameObject->mTransformCache->orientation;
        currentTransform.scale       = streamBuffer[i].gameObject->mTransformCache->scale;
        
        // Calculate parent transforms
        GameObject* parent = streamBuffer[i].gameObject->parent;
        
        // Roll over the parent matrix transform chain
        while (parent != nullptr) {
            
            //currentTransform.position    += parent->transform.position;
            //currentTransform.scale       *= parent->transform.scale;
            //currentTransform.orientation *= parent->transform.orientation;
            
            parent = parent->parent;
        }
        
        glm::mat4 translation = glm::translate(glm::mat4(1), currentTransform.position);
        glm::mat4 rotation    = glm::toMat4(currentTransform.orientation);
        glm::mat4 scale       = glm::scale(glm::mat4(1), currentTransform.scale);
        
        currentTransform.matrix = translation * rotation * scale;
        
        
        
        //
        // Rigid body
        //
        if (streamBuffer[i].rigidBody != nullptr) {
            
            // Use the rigid body as the source transform
            rp3d::Transform bodyTransform = streamBuffer[i].rigidBody->getTransform();
            rp3d::Vector3 bodyPosition = bodyTransform.getPosition();
            rp3d::Quaternion quaterion = bodyTransform.getOrientation();
            
            currentTransform.position.x = bodyPosition.x;
            currentTransform.position.y = bodyPosition.y;
            currentTransform.position.z = bodyPosition.z;
            
            currentTransform.orientation.w = quaterion.w;
            currentTransform.orientation.x = quaterion.x;
            currentTransform.orientation.y = quaterion.y;
            currentTransform.orientation.z = quaterion.z;
            
            // Source matrix
            bodyTransform.getOpenGLMatrix(&currentTransform.matrix[0][0]);
            
            // Update the game object transform
            streamBuffer[i].gameObject->mTransformCache->position    = currentTransform.position;
            streamBuffer[i].gameObject->mTransformCache->orientation = currentTransform.orientation;
            
            currentTransform.matrix = glm::scale(currentTransform.matrix, streamBuffer[i].gameObject->mTransformCache->scale);
            
        }
        
        
        
        //
        // Mesh renderer
        //
        if (streamBuffer[i].meshRenderer != nullptr) {
            
            streamBuffer[i].meshRenderer->transform.matrix = currentTransform.matrix;
            
            if (streamBuffer[i].rigidBody == nullptr) {
                streamBuffer[i].meshRenderer->transform.position     = currentTransform.position;
                streamBuffer[i].meshRenderer->transform.orientation  = currentTransform.orientation;
                streamBuffer[i].meshRenderer->transform.scale        = currentTransform.scale;
            }
            
        }
        
        
        
        
        
        //
        // Camera
        //
        
        if (streamBuffer[i].camera != nullptr) {
            
            // Update mouse looking
            if (streamBuffer[i].camera->useMouseLook) {
                
                float MouseDiffX = (Input.mouseX - Renderer.displayCenter.x) * streamBuffer[i].camera->MouseSensitivityYaw;
                float MouseDiffY = (Input.mouseY - Renderer.displayCenter.y) * streamBuffer[i].camera->MouseSensitivityPitch;
                
                Input.SetMousePosition(Renderer.displayCenter.x, Renderer.displayCenter.y);
                
                streamBuffer[i].camera->lookAngle.x += MouseDiffX * 0.01;
                streamBuffer[i].camera->lookAngle.y -= MouseDiffY * 0.01;
                
                // Yaw limit
                if (streamBuffer[i].camera->lookAngle.x >= 0.109655) {streamBuffer[i].camera->lookAngle.x -= 0.109655;}
                if (streamBuffer[i].camera->lookAngle.x <= 0.109655) {streamBuffer[i].camera->lookAngle.x += 0.109655;}
                
                // Pitch limit
                if (streamBuffer[i].camera->lookAngle.y >  0.0274f) streamBuffer[i].camera->lookAngle.y =  0.0274f;
                if (streamBuffer[i].camera->lookAngle.y < -0.0274f) streamBuffer[i].camera->lookAngle.y = -0.0274f;
                
            }
            
            // Restore looking angle
            streamBuffer[i].camera->transform.orientation.x = streamBuffer[i].camera->lookAngle.x;
            streamBuffer[i].camera->transform.orientation.y = streamBuffer[i].camera->lookAngle.y;
            
            // Check camera panel
            if (streamBuffer[i].panel != nullptr) {
                
                // Align the camera with a panel canvas
                
            } else {
                
                streamBuffer[i].camera->transform.position = currentTransform.position;
                
            }
            
        }
        
        
        
        //
        // Lights
        //
        if (streamBuffer[i].light != nullptr) {
            
            streamBuffer[i].light->position    = currentTransform.position;
            
            streamBuffer[i].light->direction   = currentTransform.EulerAngles();
            
        }
        
        
        
        //
        // Actor
        //
        if (streamBuffer[i].actor != nullptr) {
            
            // Update actor
            
            if (streamBuffer[i].actor->mIsActive) {
                
                
                
                // TESTING random movement
                
                if (Random.Range(0, 100) > 98) 
                    streamBuffer[i].actor->mIsWalking = false;
                
                if (Random.Range(0, 100) > 98) 
                    streamBuffer[i].actor->mIsWalking = true;
                
                if (Random.Range(0, 100) > 97) 
                    streamBuffer[i].actor->mRotateTo.y = Random.Range(0, 360);
                
                
                
                
                
                
                
                
                // Check walking state
                if (streamBuffer[i].actor->mIsWalking) {
                    
                    // Apply forward velocity
                    glm::vec3 forward;
                    
                    forward.x = cos( glm::radians( -(streamBuffer[i].actor->mRotation.y - 90) ) );
                    //forward.y = tan( glm::radians( -(streamBuffer[i].actor->mRotation.x - 90) ) );
                    forward.z = sin( glm::radians( -(streamBuffer[i].actor->mRotation.y - 90) ) );
                    
                    streamBuffer[i].actor->mVelocity = forward * (streamBuffer[i].actor->mSpeed * 0.01f);
                    
                    // Check running speed multiplier
                    if (streamBuffer[i].actor->mIsRunning) 
                        streamBuffer[i].actor->mVelocity *= streamBuffer[i].actor->mSpeedMul;
                    
                } else {
                    
                    // Stop moving
                    streamBuffer[i].actor->mVelocity = glm::vec3(0);
                    
                }
                
                //
                // Check actor target direction range
                
                // Max rotation angles
                if (streamBuffer[i].actor->mRotateTo.x >= 360) {
                    streamBuffer[i].actor->mRotateTo.x -= 360;
                    streamBuffer[i].actor->mRotation.x -= 360;
                }
                if (streamBuffer[i].actor->mRotateTo.y >= 360) {
                    streamBuffer[i].actor->mRotateTo.y -= 360;
                    streamBuffer[i].actor->mRotation.y -= 360;
                }
                if (streamBuffer[i].actor->mRotateTo.z >= 360) {
                    streamBuffer[i].actor->mRotateTo.z -= 360;
                    streamBuffer[i].actor->mRotation.z -= 360;
                }
                
                // Min rotation angles
                if (streamBuffer[i].actor->mRotateTo.x < 0) {
                    streamBuffer[i].actor->mRotateTo.x += 360;
                    streamBuffer[i].actor->mRotation.x += 360;
                }
                if (streamBuffer[i].actor->mRotateTo.y < 0) {
                    streamBuffer[i].actor->mRotateTo.y += 360;
                    streamBuffer[i].actor->mRotation.y += 360;
                }
                if (streamBuffer[i].actor->mRotateTo.z < 0) {
                    streamBuffer[i].actor->mRotateTo.z += 360;
                    streamBuffer[i].actor->mRotation.z += 360;
                }
                
                // Rotate actor toward the focal point
                if (streamBuffer[i].actor->mRotation != streamBuffer[i].actor->mRotateTo) {
                    
                    glm::vec3 fadeValue( streamBuffer[i].actor->mRotation );
                    glm::vec3 fadeTo   ( streamBuffer[i].actor->mRotateTo );
                    
                    fadeValue.x = Math.Lerp(fadeValue.x, fadeTo.x, streamBuffer[i].actor->mSnapSpeed);
                    fadeValue.y = Math.Lerp(fadeValue.y, fadeTo.y, streamBuffer[i].actor->mSnapSpeed);
                    fadeValue.z = Math.Lerp(fadeValue.z, fadeTo.z, streamBuffer[i].actor->mSnapSpeed);
                    
                    streamBuffer[i].actor->mRotation = fadeValue;
                }
                
                
                //
                // Update genetic expression
                //
                
                if (streamBuffer[i].actor->mDoUpdateGenetics) {
                    
                    int numberOfGenes = streamBuffer[i].actor->GetNumberOfGenes();
                    
                    //
                    // Destroy and clear old genetic renderers
                    
                    for (int a=0; a < streamBuffer[i].actor->mGeneticRenderers.size(); a++) {
                        
                        MeshRenderer* geneRenderer = streamBuffer[i].actor->mGeneticRenderers[a];
                        
                        if (geneRenderer->material != nullptr) 
                            Renderer.DestroyMaterial(geneRenderer->material);
                        
                        sceneMain->RemoveMeshRendererFromSceneRoot( geneRenderer );
                        
                        Renderer.DestroyMeshRenderer( geneRenderer );
                        
                        continue;
                    }
                    streamBuffer[i].actor->mGeneticRenderers.clear();
                    streamBuffer[i].actor->mAnimationStates.clear();
                    
                    
                    //
                    // Create and express genetic elements
                    //
                    
                    for (int a=0; a < numberOfGenes; a++) {
                        
                        if (!streamBuffer[i].actor->mGenes[a].doExpress) 
                            continue;
                        
                        Material* newMaterial = Renderer.CreateMaterial();
                        newMaterial->shader = shaders.color;
                        newMaterial->diffuse.r = streamBuffer[i].actor->mGenes[a].color.x;
                        newMaterial->diffuse.g = streamBuffer[i].actor->mGenes[a].color.y;
                        newMaterial->diffuse.b = streamBuffer[i].actor->mGenes[a].color.z;
                        
                        MeshRenderer* newRenderer = Renderer.CreateMeshRenderer();
                        newRenderer->mesh = meshes.cube;
                        newRenderer->material = newMaterial;
                        
                        // Position offset
                        glm::vec3 offset( streamBuffer[i].actor->mGenes[a].offset.x, 
                                          streamBuffer[i].actor->mGenes[a].offset.y, 
                                          streamBuffer[i].actor->mGenes[a].offset.z );
                        
                        // Rotation
                        newRenderer->transform.RotateAxis( streamBuffer[i].actor->mGenes[a].rotation.x, glm::vec3(1, 0, 0) );
                        newRenderer->transform.RotateAxis( streamBuffer[i].actor->mGenes[a].rotation.y, glm::vec3(0, 1, 0) );
                        newRenderer->transform.RotateAxis( streamBuffer[i].actor->mGenes[a].rotation.z, glm::vec3(0, 0, 1) );
                        
                        // Scale
                        newRenderer->transform.scale.x = streamBuffer[i].actor->mGenes[a].scale.x;
                        newRenderer->transform.scale.y = streamBuffer[i].actor->mGenes[a].scale.y;
                        newRenderer->transform.scale.z = streamBuffer[i].actor->mGenes[a].scale.z;
                        
                        streamBuffer[i].actor->mGeneticRenderers.push_back( newRenderer );
                        streamBuffer[i].actor->mAnimationStates .push_back( glm::vec4(1) );
                        
                        sceneMain->AddMeshRendererToSceneRoot( newRenderer );
                        
                        continue;
                    }
                    
                    streamBuffer[i].actor->mDoUpdateGenetics = false;
                } else {
                    
                    //
                    // Update genetic renderers
                    //
                    
                    for (int a=0; a < streamBuffer[i].actor->mGeneticRenderers.size(); a++) {
                        
                        MeshRenderer* geneRenderer = streamBuffer[i].actor->mGeneticRenderers[a];
                        geneRenderer->transform.position  = streamBuffer[i].actor->mPosition;
                        
                        // Initiate the transform
                        glm::mat4 matrix = glm::translate(glm::mat4(1), geneRenderer->transform.position);
                        
                        // Rotate around center mass
                        float orientationCenterMass = glm::length( streamBuffer[i].actor->mRotation );
                        if (orientationCenterMass > 0) {
                            matrix = glm::rotate(matrix, 
                                                glm::radians( orientationCenterMass ), 
                                                glm::normalize( streamBuffer[i].actor->mRotation ));
                        }
                        
                        // Offset from center
                        matrix = glm::translate( matrix, glm::vec3(streamBuffer[i].actor->mGenes[a].offset.x,
                                                                   streamBuffer[i].actor->mGenes[a].offset.y,
                                                                   streamBuffer[i].actor->mGenes[a].offset.z));
                        
                        // Check should use animation
                        if ((!streamBuffer[i].actor->mGenes[a].doAnimationCycle) | (!streamBuffer[i].actor->mIsWalking)) {
                            
                            matrix = glm::translate( matrix, glm::vec3(streamBuffer[i].actor->mGenes[a].position.x,
                                                                   streamBuffer[i].actor->mGenes[a].position.y,
                                                                   streamBuffer[i].actor->mGenes[a].position.z));
                            
                            geneRenderer->transform.matrix = matrix * glm::scale(glm::mat4(1), geneRenderer->transform.scale);
                            
                            continue;
                        }
                        
                        // Rotate current animation state
                        glm::vec3 animationFactor(streamBuffer[i].actor->mGenes[a].animationAxis.x, 
                                                  streamBuffer[i].actor->mGenes[a].animationAxis.y, 
                                                  streamBuffer[i].actor->mGenes[a].animationAxis.z);
                        
                        // Step the animation swing direction
                        float animationMaxSwingRange = streamBuffer[i].actor->mGenes[a].animationRange;
                        
                        if (streamBuffer[i].actor->mAnimationStates[a].w < 0) {
                            
                            // Check inverted animation cycle
                            if (streamBuffer[i].actor->mGenes[a].doInverseAnimation) {
                                
                                streamBuffer[i].actor->mAnimationStates[a].x += animationFactor.x;
                                streamBuffer[i].actor->mAnimationStates[a].y += animationFactor.y;
                                streamBuffer[i].actor->mAnimationStates[a].z += animationFactor.z;
                                
                                if ((streamBuffer[i].actor->mAnimationStates[a].x > animationMaxSwingRange) | 
                                    (streamBuffer[i].actor->mAnimationStates[a].y > animationMaxSwingRange) | 
                                    (streamBuffer[i].actor->mAnimationStates[a].z > animationMaxSwingRange)) {
                                    
                                    if (streamBuffer[i].actor->mGenes[a].doInverseAnimation) {
                                        streamBuffer[i].actor->mAnimationStates[a].w = 1;
                                    } else {
                                        streamBuffer[i].actor->mAnimationStates[a].w = -1;
                                    }
                                }
                                
                            } else {
                                
                                streamBuffer[i].actor->mAnimationStates[a].x -= animationFactor.x;
                                streamBuffer[i].actor->mAnimationStates[a].y -= animationFactor.y;
                                streamBuffer[i].actor->mAnimationStates[a].z -= animationFactor.z;
                                
                                if ((streamBuffer[i].actor->mAnimationStates[a].x < -animationMaxSwingRange) | 
                                    (streamBuffer[i].actor->mAnimationStates[a].y < -animationMaxSwingRange) | 
                                    (streamBuffer[i].actor->mAnimationStates[a].z < -animationMaxSwingRange)) {
                                    
                                    if (streamBuffer[i].actor->mGenes[a].doInverseAnimation) {
                                        streamBuffer[i].actor->mAnimationStates[a].w = -1;
                                    } else {
                                        streamBuffer[i].actor->mAnimationStates[a].w = 1;
                                    }
                                }
                                
                            }
                            
                        } else {
                            
                            // Check inverted animation cycle
                            if (streamBuffer[i].actor->mGenes[a].doInverseAnimation) {
                                
                                streamBuffer[i].actor->mAnimationStates[a].x -= animationFactor.x;
                                streamBuffer[i].actor->mAnimationStates[a].y -= animationFactor.y;
                                streamBuffer[i].actor->mAnimationStates[a].z -= animationFactor.z;
                                
                                if ((streamBuffer[i].actor->mAnimationStates[a].x < -animationMaxSwingRange) | 
                                    (streamBuffer[i].actor->mAnimationStates[a].y < -animationMaxSwingRange) | 
                                    (streamBuffer[i].actor->mAnimationStates[a].z < -animationMaxSwingRange)) {
                                    
                                    if (streamBuffer[i].actor->mGenes[a].doInverseAnimation) {
                                        streamBuffer[i].actor->mAnimationStates[a].w = -1;
                                    } else {
                                        streamBuffer[i].actor->mAnimationStates[a].w = 1;
                                    }
                                }
                                
                            } else {
                                
                                streamBuffer[i].actor->mAnimationStates[a].x += animationFactor.x;
                                streamBuffer[i].actor->mAnimationStates[a].y += animationFactor.y;
                                streamBuffer[i].actor->mAnimationStates[a].z += animationFactor.z;
                                
                                if ((streamBuffer[i].actor->mAnimationStates[a].x > animationMaxSwingRange) | 
                                    (streamBuffer[i].actor->mAnimationStates[a].y > animationMaxSwingRange) | 
                                    (streamBuffer[i].actor->mAnimationStates[a].z > animationMaxSwingRange)) {
                                    
                                    if (streamBuffer[i].actor->mGenes[a].doInverseAnimation) {
                                        streamBuffer[i].actor->mAnimationStates[a].w = 1;
                                    } else {
                                        streamBuffer[i].actor->mAnimationStates[a].w = -1;
                                    }
                                }
                                
                            }
                            
                        }
                        
                        
                        // Apply animation rotation state
                        matrix = glm::rotate(matrix, 
                                             glm::radians( glm::length( streamBuffer[i].actor->mAnimationStates[a] ) ), 
                                             glm::normalize( glm::vec3(streamBuffer[i].actor->mAnimationStates[a].x, 
                                                                       streamBuffer[i].actor->mAnimationStates[a].y, 
                                                                       streamBuffer[i].actor->mAnimationStates[a].z) ));
                        
                        // Final position after animation rotation
                        matrix = glm::translate( matrix, glm::vec3(streamBuffer[i].actor->mGenes[a].position.x,
                                                                   streamBuffer[i].actor->mGenes[a].position.y,
                                                                   streamBuffer[i].actor->mGenes[a].position.z));
                        
                        geneRenderer->transform.matrix = matrix * glm::scale(glm::mat4(1), geneRenderer->transform.scale);
                        
                        continue;
                    }
                    
                }
                
                
                // Update actor physics
                
                if (streamBuffer[i].rigidBody != nullptr) {
                    
                    glm::vec3 actorVelocity = streamBuffer[i].actor->mVelocity;
                    
                    // Sync actor position
                    streamBuffer[i].actor->mPosition = currentTransform.position;
                    
                    // Apply force velocity
                    streamBuffer[i].rigidBody->applyLocalForceAtCenterOfMass( rp3d::Vector3(actorVelocity.x, actorVelocity.y, actorVelocity.z) );
                    
                }
                
            }
            
        }
        
        
        
        //
        // Panel canvas alignment
        //
        
        if (streamBuffer[i].panel != nullptr) {
            
            if (streamBuffer[i].meshRenderer != nullptr) {
                
                //
                // Anchor RIGHT
                //
                
                if (streamBuffer[i].panel->canvas.anchorRight) {
                    streamBuffer[i].gameObject->mTransformCache->position.z = Renderer.viewport.w + 
                                                                              streamBuffer[i].panel->width * 
                                                                              streamBuffer[i].panel->canvas.y;
                    
                } else {
                    
                    //
                    // Anchor LEFT by default
                    //
                    
                    streamBuffer[i].gameObject->mTransformCache->position.z  = (streamBuffer[i].panel->canvas.y * streamBuffer[i].panel->width);
                    streamBuffer[i].gameObject->mTransformCache->position.z += streamBuffer[i].panel->width;
                    
                    //
                    // Anchor CENTER horizontally
                    //
                    
                    if (streamBuffer[i].panel->canvas.anchorCenterHorz) {
                        
                        streamBuffer[i].gameObject->mTransformCache->position.z = (Renderer.viewport.w / 2) + (streamBuffer[i].panel->canvas.y * streamBuffer[i].panel->width);
                        
                    }
                    
                }
                
                //
                // Anchor TOP
                //
                
                if (streamBuffer[i].panel->canvas.anchorTop) {
                    int topAnchorTotal = Renderer.displaySize.y - Renderer.viewport.h;
                    
                    topAnchorTotal += (streamBuffer[i].panel->height * streamBuffer[i].panel->height) / 2;
                    topAnchorTotal += streamBuffer[i].panel->height * streamBuffer[i].panel->canvas.x;
                    
                    streamBuffer[i].gameObject->mTransformCache->position.y = topAnchorTotal;
                } else {
                    
                    //
                    // Anchor BOTTOM by default
                    //
                    
                    streamBuffer[i].gameObject->mTransformCache->position.y  = Renderer.displaySize.y - streamBuffer[i].panel->height;
                    streamBuffer[i].gameObject->mTransformCache->position.y -= streamBuffer[i].panel->height * -(streamBuffer[i].panel->canvas.x);
                    
                    //
                    // Anchor CENTER vertically
                    //
                    
                    if (streamBuffer[i].panel->canvas.anchorCenterVert) {
                        int topAnchorTotal = Renderer.displaySize.y - Renderer.viewport.h / 2;
                        
                        topAnchorTotal += (streamBuffer[i].panel->height * streamBuffer[i].panel->height) / 2;
                        topAnchorTotal += (streamBuffer[i].panel->height * streamBuffer[i].panel->canvas.x) - (streamBuffer[i].panel->height * 2);
                        
                        streamBuffer[i].gameObject->mTransformCache->position.y = topAnchorTotal;
                    }
                    
                }
                
            }
            
        }
        
        
        
        //
        // Text canvas alignment
        //
        
        if (streamBuffer[i].text != nullptr) {
            
            if (streamBuffer[i].meshRenderer != nullptr) {
                
                //
                // Anchor RIGHT
                //
                
                if (streamBuffer[i].text->canvas.anchorRight) {
                    streamBuffer[i].gameObject->mTransformCache->position.z = Renderer.viewport.w + 
                                                                              streamBuffer[i].text->size * 
                                                                              streamBuffer[i].text->canvas.x;
                    
                    // Keep text on screen when anchored right
                    streamBuffer[i].gameObject->mTransformCache->position.z -= streamBuffer[i].text->text.size() * // length of string
                                                                               streamBuffer[i].text->size;         // Size of font text
                    
                } else {
                    
                    //
                    // Anchor LEFT by default
                    //
                    
                    streamBuffer[i].gameObject->mTransformCache->position.z  = (streamBuffer[i].text->canvas.x * streamBuffer[i].text->size);
                    streamBuffer[i].gameObject->mTransformCache->position.z += streamBuffer[i].text->size;
                    
                    //
                    // Anchor CENTER horizontally
                    //
                    
                    if (streamBuffer[i].text->canvas.anchorCenterHorz) {
                        
                        streamBuffer[i].gameObject->mTransformCache->position.z = (Renderer.viewport.w / 2) + (streamBuffer[i].text->canvas.x * streamBuffer[i].text->size);
                        
                    }
                    
                }
                
                //
                // Anchor TOP
                //
                
                if (streamBuffer[i].text->canvas.anchorTop) {
                    int topAnchorTotal = Renderer.displaySize.y - Renderer.viewport.h;
                    
                    topAnchorTotal += (streamBuffer[i].text->size * streamBuffer[i].text->size) / 2;
                    topAnchorTotal += streamBuffer[i].text->size * streamBuffer[i].text->canvas.y;
                    
                    streamBuffer[i].gameObject->mTransformCache->position.y = topAnchorTotal;
                } else {
                    
                    //
                    // Anchor BOTTOM by default
                    //
                    
                    streamBuffer[i].gameObject->mTransformCache->position.y  = Renderer.displaySize.y - streamBuffer[i].text->size;
                    streamBuffer[i].gameObject->mTransformCache->position.y -= streamBuffer[i].text->size * -(streamBuffer[i].text->canvas.y);
                    
                    //
                    // Anchor CENTER vertically
                    //
                    
                    if (streamBuffer[i].text->canvas.anchorCenterVert) {
                        int topAnchorTotal = Renderer.displaySize.y - Renderer.viewport.h / 2;
                        
                        topAnchorTotal += (streamBuffer[i].text->size * streamBuffer[i].text->size) / 2;
                        topAnchorTotal += (streamBuffer[i].text->size * streamBuffer[i].text->canvas.y) - (streamBuffer[i].text->size * 2);
                        
                        streamBuffer[i].gameObject->mTransformCache->position.y = topAnchorTotal;
                    }
                    
                }
                
                // Flip height and width
                float textGlyphWidth  = streamBuffer[i].text->glyphHeight;
                float textGlyphHeight = streamBuffer[i].text->glyphWidth;
                
                streamBuffer[i].meshRenderer->mesh->ClearSubMeshes();
                Engine.AddMeshText(streamBuffer[i].gameObject, 0, 0, textGlyphWidth, textGlyphHeight, streamBuffer[i].text->text, streamBuffer[i].text->color);
            }
        }
        
        continue;
    }
    
    return;
}
