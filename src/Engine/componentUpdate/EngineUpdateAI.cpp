//
// Update actor components

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


void EngineSystemManager::UpdateActor(unsigned int index) {
    
    // Check walking state
    if (streamBuffer[index].actor->mIsWalking) {
        
        // Apply forward velocity
        glm::vec3 forward;
        
        forward.x = cos( glm::radians( -(streamBuffer[index].actor->mRotation.y - 90) ) );
        //forward.y = tan( glm::radians( -(streamBuffer[index].actor->mRotation.x - 90) ) );
        forward.z = sin( glm::radians( -(streamBuffer[index].actor->mRotation.y - 90) ) );
        
        streamBuffer[index].actor->mVelocity = forward * (streamBuffer[index].actor->mSpeed * 0.1f) * 0.1f;
        
        // Check running speed multiplier
        if (streamBuffer[index].actor->mIsRunning) 
            streamBuffer[index].actor->mVelocity *= streamBuffer[index].actor->mSpeedMul;
        
    } else {
        
        // Stop moving but keep falling
        streamBuffer[index].actor->mVelocity *= glm::vec3(0, 1, 0);
        
    }
    
    // Face toward target point
    glm::vec3 position = streamBuffer[index].actor->mPosition;
    
    float xx = position.x - streamBuffer[index].actor->mTargetPoint.x;
    float zz = position.z - streamBuffer[index].actor->mTargetPoint.z;
    
    streamBuffer[index].actor->mRotateTo.y = glm::degrees( glm::atan(xx, zz) ) + 180;
    
    // Check invert facing direction
    if (!streamBuffer[index].actor->mIsFacing) {
        streamBuffer[index].actor->mRotateTo.y += 180;
        if (streamBuffer[index].actor->mRotateTo.y > 360) 
            streamBuffer[index].actor->mRotateTo.y -= 360;
    }
    
    // Check arrived at target point
    float targetDistance = glm::distance( streamBuffer[index].actor->mTargetPoint, 
                                    streamBuffer[index].actor->mPosition );
    
    if (targetDistance < 1.5) 
        streamBuffer[index].actor->mIsWalking = false;
    
    //
    // Check actor target direction range
    
    // Wrap euler rotations
    if (streamBuffer[index].actor->mRotation.y < 90) 
        if (streamBuffer[index].actor->mRotateTo.y > 270) 
            streamBuffer[index].actor->mRotation.y += 360;
    
    if (streamBuffer[index].actor->mRotation.y > 270) 
        if (streamBuffer[index].actor->mRotateTo.y < 90) 
            streamBuffer[index].actor->mRotation.y -= 360;
    
    // Rotate actor toward the focal point
    if (streamBuffer[index].actor->mRotation != streamBuffer[index].actor->mRotateTo) {
        
        glm::vec3 fadeValue( streamBuffer[index].actor->mRotation );
        glm::vec3 fadeTo   ( streamBuffer[index].actor->mRotateTo );
        
        fadeValue.x = Math.Lerp(fadeValue.x, fadeTo.x, streamBuffer[index].actor->mSnapSpeed);
        fadeValue.y = Math.Lerp(fadeValue.y, fadeTo.y, streamBuffer[index].actor->mSnapSpeed);
        fadeValue.z = Math.Lerp(fadeValue.z, fadeTo.z, streamBuffer[index].actor->mSnapSpeed);
        
        streamBuffer[index].actor->mRotation = fadeValue;
    }
    
    
    //
    // Update genetic expression
    //
    
    if (streamBuffer[index].actor->mDoUpdateGenetics) {
        
        int numberOfGenes = streamBuffer[index].actor->GetNumberOfGenes();
        
        //
        // Destroy and clear old genetic renderers
        
        for (unsigned int a=0; a < streamBuffer[index].actor->mGeneticRenderers.size(); a++) {
            
            MeshRenderer* geneRenderer = streamBuffer[index].actor->mGeneticRenderers[a];
            
            if (geneRenderer->material != nullptr) 
                Renderer.DestroyMaterial(geneRenderer->material);
            
            sceneMain->RemoveMeshRendererFromSceneRoot( geneRenderer );
            
            Renderer.DestroyMeshRenderer( geneRenderer );
            
            continue;
        }
        
        streamBuffer[index].actor->mGeneticRenderers.clear();
        streamBuffer[index].actor->mAnimationStates.clear();
        
        
        //
        // Create and express genetic elements
        //
        
        for (int a=0; a < numberOfGenes; a++) {
            
            if (!streamBuffer[index].actor->mGenes[a].doExpress) 
                continue;
            
            Material* newMaterial = Renderer.CreateMaterial();
            newMaterial->shader = shaders.color;
            newMaterial->diffuse.r = streamBuffer[index].actor->mGenes[a].color.x;
            newMaterial->diffuse.g = streamBuffer[index].actor->mGenes[a].color.y;
            newMaterial->diffuse.b = streamBuffer[index].actor->mGenes[a].color.z;
            
            MeshRenderer* newRenderer = Renderer.CreateMeshRenderer();
            newRenderer->mesh = meshes.cube;
            newRenderer->material = newMaterial;
            
            // Position offset
            glm::vec3 offset( streamBuffer[index].actor->mGenes[a].offset.x, 
                              streamBuffer[index].actor->mGenes[a].offset.y, 
                              streamBuffer[index].actor->mGenes[a].offset.z );
            
            // Rotation
            newRenderer->transform.RotateAxis( streamBuffer[index].actor->mGenes[a].rotation.x, glm::vec3(1, 0, 0) );
            newRenderer->transform.RotateAxis( streamBuffer[index].actor->mGenes[a].rotation.y, glm::vec3(0, 1, 0) );
            newRenderer->transform.RotateAxis( streamBuffer[index].actor->mGenes[a].rotation.z, glm::vec3(0, 0, 1) );
            
            // Scale
            newRenderer->transform.scale.x = streamBuffer[index].actor->mGenes[a].scale.x;
            newRenderer->transform.scale.y = streamBuffer[index].actor->mGenes[a].scale.y;
            newRenderer->transform.scale.z = streamBuffer[index].actor->mGenes[a].scale.z;
            
            Transform transform;
            
            
            glm::vec4 orientation(transform.orientation.w, 
                                  transform.orientation.x, 
                                  transform.orientation.y, 
                                  transform.orientation.z);
            
            streamBuffer[index].actor->mGeneticRenderers.push_back( newRenderer );
            streamBuffer[index].actor->mAnimationStates .push_back( orientation );
            
            sceneMain->AddMeshRendererToSceneRoot( newRenderer );
            
            continue;
        }
        
        streamBuffer[index].actor->mDoUpdateGenetics = false;
        
    } else {
        
        //
        // Update genetic renderers
        //
        
        for (unsigned int a=0; a < streamBuffer[index].actor->mGeneticRenderers.size(); a++) {
            
            MeshRenderer* geneRenderer = streamBuffer[index].actor->mGeneticRenderers[a];
            geneRenderer->transform.position  = streamBuffer[index].actor->mPosition;
            
            // Initiate the transform
            glm::mat4 matrix = glm::translate(glm::mat4(1), geneRenderer->transform.position);
            
            // Rotate around center mass
            float orientationCenterMass = glm::length( streamBuffer[index].actor->mRotation );
            if (orientationCenterMass > 0) {
                matrix = glm::rotate(matrix, 
                                    glm::radians( orientationCenterMass ), 
                                    glm::normalize( streamBuffer[index].actor->mRotation ));
            }
            
            // Offset from center
            matrix = glm::translate( matrix, glm::vec3(streamBuffer[index].actor->mGenes[a].offset.x,
                                                       streamBuffer[index].actor->mGenes[a].offset.y,
                                                       streamBuffer[index].actor->mGenes[a].offset.z));
            
            //
            // Update animation
            //
            
            if ((!streamBuffer[index].actor->mGenes[a].doAnimationCycle) | (!streamBuffer[index].actor->mIsWalking)) {
                
                matrix = glm::translate( matrix, glm::vec3(streamBuffer[index].actor->mGenes[a].position.x,
                                                       streamBuffer[index].actor->mGenes[a].position.y,
                                                       streamBuffer[index].actor->mGenes[a].position.z));
                
                geneRenderer->transform.matrix = matrix * glm::scale(glm::mat4(1), geneRenderer->transform.scale);
                
                continue;
            }
            
            // Rotate current animation state
            glm::vec3 animationFactor(streamBuffer[index].actor->mGenes[a].animationAxis.x, 
                                      streamBuffer[index].actor->mGenes[a].animationAxis.y, 
                                      streamBuffer[index].actor->mGenes[a].animationAxis.z);
            
            // Step the animation swing direction
            float animationMaxSwingRange = streamBuffer[index].actor->mGenes[a].animationRange;
            
            if (streamBuffer[index].actor->mAnimationStates[a].w < 0) {
                
                // Check inverted animation cycle
                if (streamBuffer[index].actor->mGenes[a].doInverseAnimation) {
                    
                    streamBuffer[index].actor->mAnimationStates[a].x += animationFactor.x;
                    streamBuffer[index].actor->mAnimationStates[a].y += animationFactor.y;
                    streamBuffer[index].actor->mAnimationStates[a].z += animationFactor.z;
                    
                    if ((streamBuffer[index].actor->mAnimationStates[a].x > animationMaxSwingRange) | 
                        (streamBuffer[index].actor->mAnimationStates[a].y > animationMaxSwingRange) | 
                        (streamBuffer[index].actor->mAnimationStates[a].z > animationMaxSwingRange)) {
                        
                        if (streamBuffer[index].actor->mGenes[a].doInverseAnimation) {
                            streamBuffer[index].actor->mAnimationStates[a].w = 1;
                        } else {
                            streamBuffer[index].actor->mAnimationStates[a].w = -1;
                        }
                    }
                    
                } else {
                    
                    streamBuffer[index].actor->mAnimationStates[a].x -= animationFactor.x;
                    streamBuffer[index].actor->mAnimationStates[a].y -= animationFactor.y;
                    streamBuffer[index].actor->mAnimationStates[a].z -= animationFactor.z;
                    
                    if ((streamBuffer[index].actor->mAnimationStates[a].x < -animationMaxSwingRange) | 
                        (streamBuffer[index].actor->mAnimationStates[a].y < -animationMaxSwingRange) | 
                        (streamBuffer[index].actor->mAnimationStates[a].z < -animationMaxSwingRange)) {
                        
                        if (streamBuffer[index].actor->mGenes[a].doInverseAnimation) {
                            streamBuffer[index].actor->mAnimationStates[a].w = -1;
                        } else {
                            streamBuffer[index].actor->mAnimationStates[a].w = 1;
                        }
                    }
                    
                }
                
            } else {
                
                // Check inverted animation cycle
                if (streamBuffer[index].actor->mGenes[a].doInverseAnimation) {
                    
                    streamBuffer[index].actor->mAnimationStates[a].x -= animationFactor.x;
                    streamBuffer[index].actor->mAnimationStates[a].y -= animationFactor.y;
                    streamBuffer[index].actor->mAnimationStates[a].z -= animationFactor.z;
                    
                    if ((streamBuffer[index].actor->mAnimationStates[a].x < -animationMaxSwingRange) | 
                        (streamBuffer[index].actor->mAnimationStates[a].y < -animationMaxSwingRange) | 
                        (streamBuffer[index].actor->mAnimationStates[a].z < -animationMaxSwingRange)) {
                        
                        if (streamBuffer[index].actor->mGenes[a].doInverseAnimation) {
                            streamBuffer[index].actor->mAnimationStates[a].w = -1;
                        } else {
                            streamBuffer[index].actor->mAnimationStates[a].w = 1;
                        }
                    }
                    
                } else {
                    
                    streamBuffer[index].actor->mAnimationStates[a].x += animationFactor.x;
                    streamBuffer[index].actor->mAnimationStates[a].y += animationFactor.y;
                    streamBuffer[index].actor->mAnimationStates[a].z += animationFactor.z;
                    
                    if ((streamBuffer[index].actor->mAnimationStates[a].x > animationMaxSwingRange) | 
                        (streamBuffer[index].actor->mAnimationStates[a].y > animationMaxSwingRange) | 
                        (streamBuffer[index].actor->mAnimationStates[a].z > animationMaxSwingRange)) {
                        
                        if (streamBuffer[index].actor->mGenes[a].doInverseAnimation) {
                            streamBuffer[index].actor->mAnimationStates[a].w = 1;
                        } else {
                            streamBuffer[index].actor->mAnimationStates[a].w = -1;
                        }
                    }
                    
                }
                
            }
            
            
            // Apply animation rotation state
            matrix = glm::rotate(matrix, 
                                 glm::radians( glm::length( streamBuffer[index].actor->mAnimationStates[a] ) ), 
                                 glm::normalize( glm::vec3(streamBuffer[index].actor->mAnimationStates[a].x, 
                                                           streamBuffer[index].actor->mAnimationStates[a].y, 
                                                           streamBuffer[index].actor->mAnimationStates[a].z) ));
            
            // Final position after animation rotation
            matrix = glm::translate( matrix, glm::vec3(streamBuffer[index].actor->mGenes[a].position.x,
                                                       streamBuffer[index].actor->mGenes[a].position.y,
                                                       streamBuffer[index].actor->mGenes[a].position.z));
            
            geneRenderer->transform.matrix = matrix * glm::scale(glm::mat4(1), geneRenderer->transform.scale);
            
            continue;
        }
        
    }
    
    
    //
    // Update actor physics
    
    if (streamBuffer[index].rigidBody == nullptr) 
        return;
    
    // Raycast here
    glm::vec3 from      = streamBuffer[index].actor->mPosition;
    glm::vec3 direction = glm::vec3(0, -1, 0);
    from.y += 0.5;
    
    Hit hit;
    
    float distance = 2;
    
    // Move the actor out of the way since we cant cast a ray from inside the collider...
    rp3d::Transform transform = streamBuffer[index].rigidBody->getTransform();
    rp3d::Vector3 currentPosition = transform.getPosition();
    currentPosition.y += 100;
    transform.setPosition(currentPosition);
    streamBuffer[index].rigidBody->setTransform(transform);
    
    // Check not on ground
    if (!Physics.Raycast(from, direction, distance, hit)) {
        
        // Apply "gravity"
        streamBuffer[index].actor->mVelocity.y -= 0.01;
        
        // Terminal velocity
        if (streamBuffer[index].actor->mVelocity.y < -1) 
            streamBuffer[index].actor->mVelocity.y = -1;
        
    } else {
        
        // Standing on ground
        streamBuffer[index].actor->mPosition.y = hit.point.y;
        currentPosition.y = hit.point.y + 100;
        
        streamBuffer[index].actor->mVelocity.y = 0;
        
    }
    
    // Move the actor back into position since we are finished casting rays...
    currentPosition.y -= 100;
    transform.setPosition(currentPosition);
    streamBuffer[index].rigidBody->setTransform(transform);
    
    // Apply force velocity
    glm::vec3 actorVelocity = streamBuffer[index].actor->mVelocity;
    streamBuffer[index].rigidBody->applyLocalForceAtCenterOfMass( rp3d::Vector3(actorVelocity.x, actorVelocity.y, actorVelocity.z) );
    
    // Sync actor position
    streamBuffer[index].actor->mPosition = streamBuffer[index].gameObject->mTransformCache->position;
    
    return;
}

