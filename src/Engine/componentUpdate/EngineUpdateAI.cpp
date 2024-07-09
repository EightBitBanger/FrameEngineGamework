//
// Update actor components

#include <GameEngineFramework/Engine/Engine.h>

ENGINE_API extern EngineComponents  Components;
ENGINE_API extern ColorPreset       Colors;
ENGINE_API extern NumberGeneration  Random;
ENGINE_API extern StringType        String;
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

ENGINE_API extern PlatformLayer     Platform;


void EngineSystemManager::UpdateActor(unsigned int index) {
    
    //
    // Optimize out actor meshes at distance
    
    // Remove from scene
    if (mStreamBuffer[index].actor->mDistance > AI.GetActorDetailDistance() - 10.0f) {
        
        if (mStreamBuffer[index].actor->mIsActorActiveInScene) {
            
            mStreamBuffer[index].actor->mIsActorActiveInScene = false;
            
            for (unsigned int a=0; a < mStreamBuffer[index].actor->mGeneticRenderers.size(); a++) {
                
                if (a == 0) 
                    continue;
                
                MeshRenderer* meshRenderer = mStreamBuffer[index].actor->mGeneticRenderers[a];
                
                sceneMain->RemoveMeshRendererFromSceneRoot( meshRenderer, RENDER_QUEUE_DEFAULT );
                
            }
        }
        
    }
    
    
    // Add to scene
    if (mStreamBuffer[index].actor->mDistance < AI.GetActorDetailDistance() + 10.0f) {
        
        if (!mStreamBuffer[index].actor->mIsActorActiveInScene) {
            
            mStreamBuffer[index].actor->mIsActorActiveInScene = true;
            
            for (unsigned int a=0; a < mStreamBuffer[index].actor->mGeneticRenderers.size(); a++) {
                
                if (a == 0) 
                    continue;
                
                MeshRenderer* meshRenderer = mStreamBuffer[index].actor->mGeneticRenderers[a];
                
                sceneMain->AddMeshRendererToSceneRoot( meshRenderer, RENDER_QUEUE_DEFAULT );
                
            }
            
        }
        
    }
    
    // Check walking state
    if (mStreamBuffer[index].actor->mIsWalking) {
        
        // Apply forward velocity
        glm::vec3 forward;
        
        forward.x = cos( glm::radians( -(mStreamBuffer[index].actor->mRotation.y - 90.0f) ) );
        // TODO: Actors should flying???
        //forward.y = tan( glm::radians( -(mStreamBuffer[index].actor->mRotation.x - 90) ) );
        forward.z = sin( glm::radians( -(mStreamBuffer[index].actor->mRotation.y - 90) ) );
        
        float actorSpeed = mStreamBuffer[index].actor->mSpeed;
        
        if (mStreamBuffer[index].actor->mAge < mStreamBuffer[index].actor->mAdultAge) 
            actorSpeed *= 0.8f;
        
        glm::vec3 actorVelocity = forward * (actorSpeed * 0.1f) * 0.1f;
        
        mStreamBuffer[index].actor->mVelocity.x = actorVelocity.x;
        mStreamBuffer[index].actor->mVelocity.z = actorVelocity.z;
        
        
        // Check running speed multiplier
        if (mStreamBuffer[index].actor->mIsRunning) {
            mStreamBuffer[index].actor->mVelocity.x *= mStreamBuffer[index].actor->mSpeedMul;
            mStreamBuffer[index].actor->mVelocity.z *= mStreamBuffer[index].actor->mSpeedMul;
        }
        
        // Get distance to target
        float targetDistance = glm::distance( mStreamBuffer[index].actor->mTargetPoint, 
                                        mStreamBuffer[index].actor->mPosition );
        
        // Check arrived at target point
        if (targetDistance < 1.5) 
            mStreamBuffer[index].actor->mIsWalking = false;
        
    } else {
        
        // Stop moving but keep falling
        mStreamBuffer[index].actor->mVelocity *= glm::vec3(0, 1, 0);
        
    }
    
    
    UpdateActorTargetRotation(index);
    
    UpdateActorGenetics(index);
    
    UpdateActorAnimation(index);
    
    
    //
    // Update actor physics
    
    if (mStreamBuffer[index].rigidBody == nullptr) 
        return;
    
    
    glm::vec3 actorPosition = mStreamBuffer[index].transform->position;
    glm::vec3 actorRotation = mStreamBuffer[index].actor->mRotation;
    glm::vec3 actorVelocity = mStreamBuffer[index].actor->mVelocity;
    
    // Ray cast here
    glm::vec3 from      = actorPosition;
    glm::vec3 direction = glm::vec3(0, -1, 0);
    from.y = 900;
    
    Hit hit;
    
    float distance = 1000;
    
    // Move the actor out of the way since we cant cast a ray from inside the collider...
    rp3d::Transform transform = mStreamBuffer[index].rigidBody->getTransform();
    rp3d::Vector3 currentPosition = transform.getPosition();
    currentPosition.y += 1000;
    transform.setPosition(currentPosition);
    mStreamBuffer[index].rigidBody->setTransform(transform);
    
    // Check not on ground
    if (!Physics.Raycast(from, direction, distance, hit)) {
        
        // Apply some falling action
        actorVelocity.y -= 0.01;
        
        // Terminal velocity
        if (actorVelocity.y < -1) 
            actorVelocity.y = -1;
        
    } else {
        
        // Standing on ground
        actorPosition.y   = hit.point.y;
        currentPosition.y = hit.point.y + 1000;
        
        actorVelocity.y = 0;
        
    }
    
    // Move the actor back into position since we are finished casting rays...
    currentPosition.y -= 1000;
    transform.setPosition(currentPosition);
    mStreamBuffer[index].rigidBody->setTransform(transform);
    
    // Apply force velocity
    mStreamBuffer[index].rigidBody->applyLocalForceAtCenterOfMass( rp3d::Vector3(actorVelocity.x, 
                                                                                 actorVelocity.y, 
                                                                                 actorVelocity.z) );
    
    // Sync actor position
    mStreamBuffer[index].actor->mPosition = actorPosition;
    mStreamBuffer[index].actor->mRotation = actorRotation;
    mStreamBuffer[index].actor->mVelocity = actorVelocity;
    
    return;
}






void EngineSystemManager::UpdateActorTargetRotation(unsigned int index) {
    
    // Face toward target point
    glm::vec3 position = mStreamBuffer[index].actor->mPosition;
    
    float xx = position.x - mStreamBuffer[index].actor->mTargetPoint.x;
    float zz = position.z - mStreamBuffer[index].actor->mTargetPoint.z;
    
    mStreamBuffer[index].actor->mRotateTo.y = glm::degrees( glm::atan(xx, zz) ) + 180;
    
    // Check to invert facing direction
    if (!mStreamBuffer[index].actor->mIsFacing) {
        
        mStreamBuffer[index].actor->mRotateTo.y += 180;
        
        if (mStreamBuffer[index].actor->mRotateTo.y > 360) 
            mStreamBuffer[index].actor->mRotateTo.y -= 360;
    }
    
    //
    // Check actor target direction
    
    // Wrap euler rotations
    if (mStreamBuffer[index].actor->mRotation.y < 90) 
        if (mStreamBuffer[index].actor->mRotateTo.y > 270) 
            mStreamBuffer[index].actor->mRotation.y += 360;
    
    if (mStreamBuffer[index].actor->mRotation.y > 270) 
        if (mStreamBuffer[index].actor->mRotateTo.y < 90) 
            mStreamBuffer[index].actor->mRotation.y -= 360;
    
    // Rotate actor toward the focal point
    if (mStreamBuffer[index].actor->mRotation != mStreamBuffer[index].actor->mRotateTo) {
        
        glm::vec3 fadeValue( mStreamBuffer[index].actor->mRotation );
        glm::vec3 fadeTo   ( mStreamBuffer[index].actor->mRotateTo );
        
        fadeValue.x = Math.Lerp(fadeValue.x, fadeTo.x, mStreamBuffer[index].actor->mSnapSpeed);
        fadeValue.y = Math.Lerp(fadeValue.y, fadeTo.y, mStreamBuffer[index].actor->mSnapSpeed);
        fadeValue.z = Math.Lerp(fadeValue.z, fadeTo.z, mStreamBuffer[index].actor->mSnapSpeed);
        
        mStreamBuffer[index].actor->mRotation = fadeValue;
    }
    
    return;
}




void EngineSystemManager::UpdateActorGenetics(unsigned int index) {
    
    if (!mStreamBuffer[index].actor->mDoUpdateGenetics) 
        return;
    
    int numberOfGenes = mStreamBuffer[index].actor->GetNumberOfGenes();
    
    //
    // Destroy and clear old genetic renderers
    
    mStreamBuffer[index].actor->mIsActorActiveInScene = false;
    
    for (unsigned int a=0; a < mStreamBuffer[index].actor->mGeneticRenderers.size(); a++) {
        
        MeshRenderer* geneRenderer = mStreamBuffer[index].actor->mGeneticRenderers[a];
        
        // Destroy old material
        if (geneRenderer->material != nullptr) 
            Renderer.DestroyMaterial(geneRenderer->material);
        
        sceneMain->RemoveMeshRendererFromSceneRoot( geneRenderer, RENDER_QUEUE_DEFAULT );
        
        Renderer.DestroyMeshRenderer( geneRenderer );
        
        continue;
    }
    
    mStreamBuffer[index].actor->mGeneticRenderers.clear();
    mStreamBuffer[index].actor->mAnimationStates.clear();
    
    
    //
    // Create and express genetic elements
    //
    
    for (int a=0; a < numberOfGenes; a++) {
        
        if (!mStreamBuffer[index].actor->mGenes[a].doExpress) 
            continue;
        
        // Generate a new mesh renderer for the gene
        
        Material* newMaterial = Renderer.CreateMaterial();
        newMaterial->shader = shaders.color;
        newMaterial->diffuse.r = mStreamBuffer[index].actor->mGenes[a].color.x;
        newMaterial->diffuse.g = mStreamBuffer[index].actor->mGenes[a].color.y;
        newMaterial->diffuse.b = mStreamBuffer[index].actor->mGenes[a].color.z;
        
        float ambient = 0.01f;
        newMaterial->ambient = Colors.Make(ambient, ambient, ambient);
        
        MeshRenderer* newRenderer = Renderer.CreateMeshRenderer();
        newRenderer->isActive = false;
        
        newRenderer->mesh     = meshes.cube;
        newRenderer->material = newMaterial;
        
        newRenderer->EnableFrustumCulling();
        
        // Position
        newRenderer->transform.position = mStreamBuffer[index].actor->mPosition;
        
        glm::vec3 offset(0);
        glm::vec4 orientation(0);
        
        if (mStreamBuffer[index].actor->mGenes[a].attachmentIndex > 0) {
            
            unsigned int attachmentIndex = mStreamBuffer[index].actor->mGenes[a].attachmentIndex - 1;
            
            // Position offset
            mStreamBuffer[index].actor->mGenes[a].offset.x = mStreamBuffer[index].actor->mGenes[attachmentIndex].offset.x;
            mStreamBuffer[index].actor->mGenes[a].offset.y = mStreamBuffer[index].actor->mGenes[attachmentIndex].offset.y;
            mStreamBuffer[index].actor->mGenes[a].offset.z = mStreamBuffer[index].actor->mGenes[attachmentIndex].offset.z;
            
            // Color
            newMaterial->diffuse.r = mStreamBuffer[index].actor->mGenes[attachmentIndex].color.x;
            newMaterial->diffuse.g = mStreamBuffer[index].actor->mGenes[attachmentIndex].color.y;
            newMaterial->diffuse.b = mStreamBuffer[index].actor->mGenes[attachmentIndex].color.z;
            
            // Animation state
            mStreamBuffer[index].actor->mGenes[a].animationAxis = mStreamBuffer[index].actor->mGenes[attachmentIndex].animationAxis;
            mStreamBuffer[index].actor->mGenes[a].animationRange = mStreamBuffer[index].actor->mGenes[attachmentIndex].animationRange;
            mStreamBuffer[index].actor->mGenes[a].doAnimationCycle = mStreamBuffer[index].actor->mGenes[attachmentIndex].doAnimationCycle;
            mStreamBuffer[index].actor->mGenes[a].doInverseAnimation = mStreamBuffer[index].actor->mGenes[attachmentIndex].doInverseAnimation;
            
        }
        
        // Scale
        newRenderer->transform.scale.x = mStreamBuffer[index].actor->mGenes[a].scale.x;
        newRenderer->transform.scale.y = mStreamBuffer[index].actor->mGenes[a].scale.y;
        newRenderer->transform.scale.z = mStreamBuffer[index].actor->mGenes[a].scale.z;
        
        // Rotation
        newRenderer->transform.RotateAxis( mStreamBuffer[index].actor->mGenes[a].rotation.x, glm::vec3(1, 0, 0) );
        newRenderer->transform.RotateAxis( mStreamBuffer[index].actor->mGenes[a].rotation.y, glm::vec3(0, 1, 0) );
        newRenderer->transform.RotateAxis( mStreamBuffer[index].actor->mGenes[a].rotation.z, glm::vec3(0, 0, 1) );
        
        // Default animation orientation
        Transform transform;
        
        orientation = glm::vec4( transform.rotation.w, 
                                 transform.rotation.x, 
                                 transform.rotation.y, 
                                 transform.rotation.z );
        
        mStreamBuffer[index].actor->mGeneticRenderers.push_back( newRenderer );
        mStreamBuffer[index].actor->mAnimationStates .push_back( orientation );
        
        if (a == 0) 
            sceneMain->AddMeshRendererToSceneRoot( newRenderer, RENDER_QUEUE_DEFAULT );
        
        continue;
    }
    
    mStreamBuffer[index].actor->mDoUpdateGenetics = false;
    
    return;
}








void EngineSystemManager::UpdateActorAnimation(unsigned int index) {
    
    //
    // Update animations
    //
    
    for (unsigned int a=0; a < mStreamBuffer[index].actor->mGeneticRenderers.size(); a++) {
        
        MeshRenderer* geneRenderer = mStreamBuffer[index].actor->mGeneticRenderers[a];
        
        geneRenderer->transform.position = mStreamBuffer[index].actor->mPosition;
        
        // Initiate the transform
        glm::mat4 matrix = glm::translate(glm::mat4(1), geneRenderer->transform.position);
        
        // Rotate around center mass
        float orientationCenterMass = glm::length( mStreamBuffer[index].actor->mRotation );
        
        
        // Scale by age
        float ageScale = (mStreamBuffer[index].actor->mAge * 0.0001f);
        
        if (ageScale < 1) {
            
            matrix = glm::scale( matrix, Math.Lerp(glm::vec3(mStreamBuffer[index].actor->mYouthScale), 
                                                   glm::vec3(mStreamBuffer[index].actor->mAdultScale), 
                                                   ageScale) );
            
        } else {
            
            matrix = glm::scale( matrix, glm::vec3(mStreamBuffer[index].actor->mAdultScale) );
            
        }
        
        // Rotate around center
        if (orientationCenterMass > 0) {
            
            matrix = glm::rotate(matrix, 
                                glm::radians( orientationCenterMass ), 
                                glm::normalize( mStreamBuffer[index].actor->mRotation ));
            
        }
        
        
        // Offset from center
        matrix = glm::translate( matrix, glm::vec3(mStreamBuffer[index].actor->mGenes[a].offset.x,
                                                mStreamBuffer[index].actor->mGenes[a].offset.y,
                                                mStreamBuffer[index].actor->mGenes[a].offset.z));
        
        //
        // Update animation
        //
        
        if ((!mStreamBuffer[index].actor->mGenes[a].doAnimationCycle) | (!mStreamBuffer[index].actor->mIsWalking)) {
            
            matrix = glm::translate( matrix, glm::vec3(mStreamBuffer[index].actor->mGenes[a].position.x,
                                                       mStreamBuffer[index].actor->mGenes[a].position.y,
                                                       mStreamBuffer[index].actor->mGenes[a].position.z));
            
            geneRenderer->transform.matrix = glm::scale(matrix, geneRenderer->transform.scale);
            
            continue;
        }
        
        if (!mStreamBuffer[index].actor->mIsActorActiveInScene) 
            continue;
        
        // Rotate current animation state
        glm::vec4 animationFactor(mStreamBuffer[index].actor->mGenes[a].animationAxis.x, 
                                  mStreamBuffer[index].actor->mGenes[a].animationAxis.y, 
                                  mStreamBuffer[index].actor->mGenes[a].animationAxis.z, 
                                  0);
        
        animationFactor = glm::normalize(animationFactor);
        
        // Step the animation swing direction
        float animationMaxSwingRange = mStreamBuffer[index].actor->mGenes[a].animationRange;
        
        if (mStreamBuffer[index].actor->mAnimationStates[a].w < 0) {
            
            // Check inverted animation cycle
            if (mStreamBuffer[index].actor->mGenes[a].doInverseAnimation) {
                
                mStreamBuffer[index].actor->mAnimationStates[a] += animationFactor;
                
                if ((mStreamBuffer[index].actor->mAnimationStates[a].x > animationMaxSwingRange) | 
                    (mStreamBuffer[index].actor->mAnimationStates[a].y > animationMaxSwingRange) | 
                    (mStreamBuffer[index].actor->mAnimationStates[a].z > animationMaxSwingRange)) {
                    
                    if (mStreamBuffer[index].actor->mGenes[a].doInverseAnimation) {
                        
                        mStreamBuffer[index].actor->mAnimationStates[a].w = 1;
                        
                    } else {
                        
                        mStreamBuffer[index].actor->mAnimationStates[a].w = -1;
                    }
                }
                
            } else {
                
                mStreamBuffer[index].actor->mAnimationStates[a] -= animationFactor;
                
                if ((mStreamBuffer[index].actor->mAnimationStates[a].x < -animationMaxSwingRange) | 
                    (mStreamBuffer[index].actor->mAnimationStates[a].y < -animationMaxSwingRange) | 
                    (mStreamBuffer[index].actor->mAnimationStates[a].z < -animationMaxSwingRange)) {
                    
                    if (mStreamBuffer[index].actor->mGenes[a].doInverseAnimation) {
                        
                        mStreamBuffer[index].actor->mAnimationStates[a].w = -1;
                        
                    } else {
                        
                        mStreamBuffer[index].actor->mAnimationStates[a].w = 1;
                    }
                }
                
            }
            
        } else {
            
            // Check to invert the animation
            if (!mStreamBuffer[index].actor->mGenes[a].doInverseAnimation) {
                
                // Animation cycle
                
                mStreamBuffer[index].actor->mAnimationStates[a] += animationFactor;
                
                if ((mStreamBuffer[index].actor->mAnimationStates[a].x > animationMaxSwingRange) | 
                    (mStreamBuffer[index].actor->mAnimationStates[a].y > animationMaxSwingRange) | 
                    (mStreamBuffer[index].actor->mAnimationStates[a].z > animationMaxSwingRange)) {
                    
                    if (mStreamBuffer[index].actor->mGenes[a].doInverseAnimation) {
                        
                        mStreamBuffer[index].actor->mAnimationStates[a].w = 1;
                        
                    } else {
                        
                        mStreamBuffer[index].actor->mAnimationStates[a].w = -1;
                        
                    }
                }
                
            } else {
                
                // Inverse animation cycle
                
                mStreamBuffer[index].actor->mAnimationStates[a] -= animationFactor;
                
                if ((mStreamBuffer[index].actor->mAnimationStates[a].x < -animationMaxSwingRange) | 
                    (mStreamBuffer[index].actor->mAnimationStates[a].y < -animationMaxSwingRange) | 
                    (mStreamBuffer[index].actor->mAnimationStates[a].z < -animationMaxSwingRange)) {
                    
                    if (mStreamBuffer[index].actor->mGenes[a].doInverseAnimation) {
                        
                        mStreamBuffer[index].actor->mAnimationStates[a].w = -1;
                        
                    } else {
                        
                        mStreamBuffer[index].actor->mAnimationStates[a].w = 1;
                    }
                }
                
            }
            
        }
        
        
        // Apply animation rotation state
        float animationLength = glm::length( mStreamBuffer[index].actor->mAnimationStates[a] );
        
        // Cannot be zero when rotating
        if (mStreamBuffer[index].actor->mAnimationStates[a].x == 0) 
            mStreamBuffer[index].actor->mAnimationStates[a].x += 0.0001f;
        if (mStreamBuffer[index].actor->mAnimationStates[a].y == 0) 
            mStreamBuffer[index].actor->mAnimationStates[a].y += 0.0001f;
        if (mStreamBuffer[index].actor->mAnimationStates[a].z == 0) 
            mStreamBuffer[index].actor->mAnimationStates[a].z += 0.0001f;
        
        matrix = glm::rotate(matrix, 
                            glm::radians( animationLength ), 
                            glm::normalize( glm::vec3(mStreamBuffer[index].actor->mAnimationStates[a].x, 
                                                    mStreamBuffer[index].actor->mAnimationStates[a].y, 
                                                    mStreamBuffer[index].actor->mAnimationStates[a].z) ));
        
        // Final position after animation rotation
        matrix = glm::translate( matrix, glm::vec3(mStreamBuffer[index].actor->mGenes[a].position.x,
                                                mStreamBuffer[index].actor->mGenes[a].position.y,
                                                mStreamBuffer[index].actor->mGenes[a].position.z));
        
        geneRenderer->transform.matrix = glm::scale(matrix, geneRenderer->transform.scale);
        
        continue;
    }
    
    
    
    return;
}

