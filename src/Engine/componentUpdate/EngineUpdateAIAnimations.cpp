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
        float ageScale = (mStreamBuffer[index].actor->GetAge() * 0.0001f);
        
        if (ageScale < 1.0f) {
            
            matrix = glm::scale( matrix, Math.Lerp(glm::vec3(mStreamBuffer[index].actor->mYouthScale), 
                                                   glm::vec3(mStreamBuffer[index].actor->mAdultScale), 
                                                   ageScale) );
            
        } else {
            
            matrix = glm::scale( matrix, glm::vec3(mStreamBuffer[index].actor->mAdultScale) );
            
            ageScale = 1.0f;
            
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

