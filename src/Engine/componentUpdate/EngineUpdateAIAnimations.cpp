#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateActorAnimation(unsigned int index) {
    
    for (unsigned int a = 0; a < mStreamBuffer[index].actor->mGeneticRenderers.size(); a++) {
        
        MeshRenderer* geneRenderer = mStreamBuffer[index].actor->mGeneticRenderers[a];
        
        geneRenderer->transform.position = mStreamBuffer[index].actor->mPosition;
        
        glm::mat4 matrix = glm::translate(glm::mat4(1), geneRenderer->transform.position);
        
        ApplyScaleByAge(matrix, index, a);
        ApplyRotation(matrix, index, a);
        ApplyOffsetFromCenter(matrix, index, a);
        UpdateAnimation(matrix, index, a);
        
        geneRenderer->transform.matrix = glm::scale(matrix, geneRenderer->transform.scale);
    }
    
    return;
}

void EngineSystemManager::ApplyScaleByAge(glm::mat4& matrix, unsigned int index, unsigned int a) {
    
    float ageScalerValue = std::min(((float)mStreamBuffer[index].actor->mAge) * 0.001f, 1.0f);
    
    float ageScale = Math.Lerp(mStreamBuffer[index].actor->mYouthScale, 
                               mStreamBuffer[index].actor->mAdultScale, 
                               ageScalerValue);
    
    matrix = glm::scale(matrix, glm::vec3(ageScale));
    
    return;
}

void EngineSystemManager::ApplyRotation(glm::mat4& matrix, unsigned int index, unsigned int a) {
    
    float orientationCenterMass = glm::length(mStreamBuffer[index].actor->mRotation);
    
    if (orientationCenterMass > 0) {
        matrix = glm::rotate(matrix, glm::radians(orientationCenterMass), 
                             glm::normalize(mStreamBuffer[index].actor->mRotation));
    }
    
    return;
}

void EngineSystemManager::ApplyOffsetFromCenter(glm::mat4& matrix, unsigned int index, unsigned int a) {
    matrix = glm::translate(matrix, glm::vec3(mStreamBuffer[index].actor->mGenes[a].offset.x,
                                              mStreamBuffer[index].actor->mGenes[a].offset.y,
                                              mStreamBuffer[index].actor->mGenes[a].offset.z));
}

void EngineSystemManager::UpdateAnimation(glm::mat4& matrix, unsigned int index, unsigned int a) {
    
    // Check skip animation for this renderer
    if (!mStreamBuffer[index].actor->mGenes[a].doAnimationCycle || !mStreamBuffer[index].actor->mIsWalking) {
        
        matrix = glm::translate(matrix, glm::vec3(mStreamBuffer[index].actor->mGenes[a].position.x,
                                                mStreamBuffer[index].actor->mGenes[a].position.y,
                                                mStreamBuffer[index].actor->mGenes[a].position.z));
        
        return;
    }
    
    // Rotate then translate for proper limb animations
    
    ApplyAnimationRotation(matrix, index, a);
    
    matrix = glm::translate(matrix, glm::vec3(mStreamBuffer[index].actor->mGenes[a].position.x,
                                              mStreamBuffer[index].actor->mGenes[a].position.y,
                                              mStreamBuffer[index].actor->mGenes[a].position.z));
    
    glm::vec4 animationFactor = glm::normalize(glm::vec4(mStreamBuffer[index].actor->mGenes[a].animationAxis.x, 
                                                         mStreamBuffer[index].actor->mGenes[a].animationAxis.y, 
                                                         mStreamBuffer[index].actor->mGenes[a].animationAxis.z, 0));
    
    float animationMaxSwingRange = mStreamBuffer[index].actor->mGenes[a].animationRange;
    
    if (mStreamBuffer[index].actor->mAnimationStates[a].w < 0) {
        
        HandleAnimationSwingForward(index, a, animationFactor, animationMaxSwingRange);
        
    } else {
        
        HandleAnimationSwingBackward(index, a, animationFactor, animationMaxSwingRange);
    }
    
    return;
}

void EngineSystemManager::HandleAnimationSwingForward(unsigned int index, unsigned int a, glm::vec4& animationFactor, float animationMaxSwingRange) {
    if (mStreamBuffer[index].actor->mGenes[a].doInverseAnimation) {
        mStreamBuffer[index].actor->mAnimationStates[a] += animationFactor;
        if (glm::any(glm::greaterThan(mStreamBuffer[index].actor->mAnimationStates[a], glm::vec4(animationMaxSwingRange)))) {
            mStreamBuffer[index].actor->mAnimationStates[a].w = mStreamBuffer[index].actor->mGenes[a].doInverseAnimation ? 1 : -1;
        }
    } else {
        mStreamBuffer[index].actor->mAnimationStates[a] -= animationFactor;
        if (glm::any(glm::lessThan(mStreamBuffer[index].actor->mAnimationStates[a], glm::vec4(-animationMaxSwingRange)))) {
            mStreamBuffer[index].actor->mAnimationStates[a].w = mStreamBuffer[index].actor->mGenes[a].doInverseAnimation ? -1 : 1;
        }
    }
}

void EngineSystemManager::HandleAnimationSwingBackward(unsigned int index, unsigned int a, glm::vec4& animationFactor, float animationMaxSwingRange) {
    if (!mStreamBuffer[index].actor->mGenes[a].doInverseAnimation) {
        mStreamBuffer[index].actor->mAnimationStates[a] += animationFactor;
        if (glm::any(glm::greaterThan(mStreamBuffer[index].actor->mAnimationStates[a], glm::vec4(animationMaxSwingRange)))) {
            mStreamBuffer[index].actor->mAnimationStates[a].w = mStreamBuffer[index].actor->mGenes[a].doInverseAnimation ? 1 : -1;
        }
    } else {
        mStreamBuffer[index].actor->mAnimationStates[a] -= animationFactor;
        if (glm::any(glm::lessThan(mStreamBuffer[index].actor->mAnimationStates[a], glm::vec4(-animationMaxSwingRange)))) {
            mStreamBuffer[index].actor->mAnimationStates[a].w = mStreamBuffer[index].actor->mGenes[a].doInverseAnimation ? -1 : 1;
        }
    }
}

void EngineSystemManager::ApplyAnimationRotation(glm::mat4& matrix, unsigned int index, unsigned int a) {
    EnsureNonZeroAnimationState(index, a);
    glm::vec3 baseRotation = glm::vec3(mStreamBuffer[index].actor->mGenes[a].rotation.x, 
                                       mStreamBuffer[index].actor->mGenes[a].rotation.y, 
                                       mStreamBuffer[index].actor->mGenes[a].rotation.z) + 0.0001f;
    float rotationLength = glm::length(baseRotation);
    matrix = glm::rotate(matrix, rotationLength, glm::normalize(baseRotation));

    float animationLength = glm::length(mStreamBuffer[index].actor->mAnimationStates[a]);
    matrix = glm::rotate(matrix, glm::radians(animationLength), 
                         glm::normalize(glm::vec3(mStreamBuffer[index].actor->mAnimationStates[a])));
}

void EngineSystemManager::EnsureNonZeroAnimationState(unsigned int index, unsigned int a) {
    if (mStreamBuffer[index].actor->mAnimationStates[a].x == 0) 
        mStreamBuffer[index].actor->mAnimationStates[a].x += 0.0001f;
    if (mStreamBuffer[index].actor->mAnimationStates[a].y == 0) 
        mStreamBuffer[index].actor->mAnimationStates[a].y += 0.0001f;
    if (mStreamBuffer[index].actor->mAnimationStates[a].z == 0) 
        mStreamBuffer[index].actor->mAnimationStates[a].z += 0.0001f;
}














/*



#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateActorAnimation(unsigned int index) {
    
    for (unsigned int a=0; a < mStreamBuffer[index].actor->mGeneticRenderers.size(); a++) {
        
        MeshRenderer* geneRenderer = mStreamBuffer[index].actor->mGeneticRenderers[a];
        
        geneRenderer->transform.position = mStreamBuffer[index].actor->mPosition;
        
        // Initiate the transform
        glm::mat4 matrix = glm::translate(glm::mat4(1), geneRenderer->transform.position);
        
        // Rotate around center mass
        float orientationCenterMass = glm::length( mStreamBuffer[index].actor->mRotation );
        
        
        // Scale by age
        float ageScalerValue = ((float)mStreamBuffer[index].actor->mAge) * 0.001f;
        
        if (ageScalerValue > 1.0f) 
            ageScalerValue = 1.0f;
        
        float ageScale = Math.Lerp(mStreamBuffer[index].actor->mYouthScale, 
                                   mStreamBuffer[index].actor->mAdultScale, 
                                   ageScalerValue);
        
        matrix = glm::scale( matrix, glm::vec3( ageScale ));
        
        // Clamp gene scale
        geneRenderer->transform.scale = glm::clamp(geneRenderer->transform.scale, 0.0f, 8.0f);
        
        
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
                
                // Swing forward
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
                
                // Swing backward
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
        
        // Cannot be zero when rotating
        if (mStreamBuffer[index].actor->mAnimationStates[a].x == 0) 
            mStreamBuffer[index].actor->mAnimationStates[a].x += 0.0001f;
        if (mStreamBuffer[index].actor->mAnimationStates[a].y == 0) 
            mStreamBuffer[index].actor->mAnimationStates[a].y += 0.0001f;
        if (mStreamBuffer[index].actor->mAnimationStates[a].z == 0) 
            mStreamBuffer[index].actor->mAnimationStates[a].z += 0.0001f;
        
        // Calculate base rotation
        glm::vec3 baseRotation = glm::vec3(mStreamBuffer[index].actor->mGenes[a].rotation.x + 0.0001f, 
                                           mStreamBuffer[index].actor->mGenes[a].rotation.y + 0.0001f, 
                                           mStreamBuffer[index].actor->mGenes[a].rotation.z + 0.0001f);
        
        float rotationLength = glm::length( baseRotation );
        
        matrix = glm::rotate(matrix, 
                            rotationLength, 
                            glm::normalize( baseRotation ));
        
        // Calculate animation rotation
        float animationLength = glm::length( mStreamBuffer[index].actor->mAnimationStates[a] );
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






*/
