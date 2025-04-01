#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateActorAnimation(unsigned int index) {
    
    for (unsigned int a = 0; a < mStreamBuffer[index].actor->genetics.mGeneticRenderers.size(); a++) {
        
        MeshRenderer* geneRenderer = mStreamBuffer[index].actor->genetics.mGeneticRenderers[a];
        
        geneRenderer->transform.position = mStreamBuffer[index].actor->navigation.mPosition;
        
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
    
    float ageScalerValue = glm::clamp((float)mStreamBuffer[index].actor->physical.mAge * 0.001f, 0.0f, 1.0f);
    
    float ageScale = Math.Lerp(mStreamBuffer[index].actor->physical.mYouthScale, 
                               mStreamBuffer[index].actor->physical.mAdultScale, 
                               ageScalerValue);
    
    matrix = glm::scale(matrix, glm::vec3(ageScale));
    
    return;
}

void EngineSystemManager::ApplyRotation(glm::mat4& matrix, unsigned int index, unsigned int a) {
    
    float orientationCenterMass = glm::length(mStreamBuffer[index].actor->navigation.mRotation);
    
    if (orientationCenterMass != 0) {
        matrix = glm::rotate(matrix, glm::radians(orientationCenterMass), 
                             glm::normalize(mStreamBuffer[index].actor->navigation.mRotation));
    }
    
    return;
}

void EngineSystemManager::ApplyOffsetFromCenter(glm::mat4& matrix, unsigned int index, unsigned int a) {
    matrix = glm::translate(matrix, glm::vec3(mStreamBuffer[index].actor->genetics.mGenes[a].offset.x,
                                              mStreamBuffer[index].actor->genetics.mGenes[a].offset.y,
                                              mStreamBuffer[index].actor->genetics.mGenes[a].offset.z));
}

void EngineSystemManager::UpdateAnimation(glm::mat4& matrix, unsigned int index, unsigned int a) {
    
    // Set base rotation
    glm::vec3 baseRotation = glm::vec3(mStreamBuffer[index].actor->genetics.mGenes[a].rotation.x, 
                                    mStreamBuffer[index].actor->genetics.mGenes[a].rotation.y, 
                                    mStreamBuffer[index].actor->genetics.mGenes[a].rotation.z);
    
    float rotationLength = glm::length(baseRotation);
    
    // Check skip animation for this renderer
    if (!mStreamBuffer[index].actor->genetics.mGenes[a].doAnimationCycle || !mStreamBuffer[index].actor->state.mIsWalking) {
        
        matrix = glm::translate(matrix, glm::vec3(mStreamBuffer[index].actor->genetics.mGenes[a].position.x,
                                                  mStreamBuffer[index].actor->genetics.mGenes[a].position.y,
                                                  mStreamBuffer[index].actor->genetics.mGenes[a].position.z));
        
        if (rotationLength != 0.0f) 
            matrix = glm::rotate(matrix, rotationLength, glm::normalize(baseRotation));
        
        return;
    }
    
    // Rotate then translate for proper limb animations
    
    if (rotationLength != 0.0f) {
        
        matrix = glm::translate(matrix, glm::vec3(mStreamBuffer[index].actor->genetics.mGenes[a].position.x,
                                              mStreamBuffer[index].actor->genetics.mGenes[a].position.y,
                                              mStreamBuffer[index].actor->genetics.mGenes[a].position.z));
        
        matrix = glm::rotate(matrix, rotationLength, glm::normalize(baseRotation));
        
        return;
    } else {
        
        ApplyAnimationRotation(matrix, index, a);
        
        matrix = glm::translate(matrix, glm::vec3(mStreamBuffer[index].actor->genetics.mGenes[a].position.x,
                                                mStreamBuffer[index].actor->genetics.mGenes[a].position.y,
                                                mStreamBuffer[index].actor->genetics.mGenes[a].position.z));
        
    }
    
    glm::vec4 animationFactor = glm::normalize(glm::vec4(mStreamBuffer[index].actor->genetics.mGenes[a].animationAxis.x, 
                                                         mStreamBuffer[index].actor->genetics.mGenes[a].animationAxis.y, 
                                                         mStreamBuffer[index].actor->genetics.mGenes[a].animationAxis.z, 0));
    
    float animationMaxSwingRange = mStreamBuffer[index].actor->genetics.mGenes[a].animationRange;
    
    if (mStreamBuffer[index].actor->state.mAnimation[a].w < 0) {
        
        HandleAnimationSwingForward(index, a, animationFactor, animationMaxSwingRange);
        
    } else {
        
        HandleAnimationSwingBackward(index, a, animationFactor, animationMaxSwingRange);
    }
    
    return;
}

void EngineSystemManager::HandleAnimationSwingForward(unsigned int index, unsigned int a, glm::vec4& animationFactor, float animationMaxSwingRange) {
    if (mStreamBuffer[index].actor->genetics.mGenes[a].doInverseAnimation) {
        mStreamBuffer[index].actor->state.mAnimation[a] += animationFactor;
        if (glm::any(glm::greaterThan(mStreamBuffer[index].actor->state.mAnimation[a], glm::vec4(animationMaxSwingRange)))) {
            mStreamBuffer[index].actor->state.mAnimation[a].w = mStreamBuffer[index].actor->genetics.mGenes[a].doInverseAnimation ? 1 : -1;
        }
    } else {
        mStreamBuffer[index].actor->state.mAnimation[a] -= animationFactor;
        if (glm::any(glm::lessThan(mStreamBuffer[index].actor->state.mAnimation[a], glm::vec4(-animationMaxSwingRange)))) {
            mStreamBuffer[index].actor->state.mAnimation[a].w = mStreamBuffer[index].actor->genetics.mGenes[a].doInverseAnimation ? -1 : 1;
        }
    }
}

void EngineSystemManager::HandleAnimationSwingBackward(unsigned int index, unsigned int a, glm::vec4& animationFactor, float animationMaxSwingRange) {
    if (!mStreamBuffer[index].actor->genetics.mGenes[a].doInverseAnimation) {
        mStreamBuffer[index].actor->state.mAnimation[a] += animationFactor;
        if (glm::any(glm::greaterThan(mStreamBuffer[index].actor->state.mAnimation[a], glm::vec4(animationMaxSwingRange)))) {
            mStreamBuffer[index].actor->state.mAnimation[a].w = mStreamBuffer[index].actor->genetics.mGenes[a].doInverseAnimation ? 1 : -1;
        }
    } else {
        mStreamBuffer[index].actor->state.mAnimation[a] -= animationFactor;
        if (glm::any(glm::lessThan(mStreamBuffer[index].actor->state.mAnimation[a], glm::vec4(-animationMaxSwingRange)))) {
            mStreamBuffer[index].actor->state.mAnimation[a].w = mStreamBuffer[index].actor->genetics.mGenes[a].doInverseAnimation ? -1 : 1;
        }
    }
}

void EngineSystemManager::ApplyAnimationRotation(glm::mat4& matrix, unsigned int index, unsigned int a) {
    EnsureNonZeroAnimationState(index, a);
    glm::vec3 baseRotation = glm::vec3(mStreamBuffer[index].actor->genetics.mGenes[a].rotation.x, 
                                       mStreamBuffer[index].actor->genetics.mGenes[a].rotation.y, 
                                       mStreamBuffer[index].actor->genetics.mGenes[a].rotation.z) + 0.0001f;
    float rotationLength = glm::length(baseRotation);
    matrix = glm::rotate(matrix, rotationLength, glm::normalize(baseRotation));

    float animationLength = glm::length(mStreamBuffer[index].actor->state.mAnimation[a]);
    matrix = glm::rotate(matrix, glm::radians(animationLength), 
                         glm::normalize(glm::vec3(mStreamBuffer[index].actor->state.mAnimation[a])));
}

void EngineSystemManager::EnsureNonZeroAnimationState(unsigned int index, unsigned int a) {
    if (mStreamBuffer[index].actor->state.mAnimation[a].x == 0) 
        mStreamBuffer[index].actor->state.mAnimation[a].x += 0.0001f;
    if (mStreamBuffer[index].actor->state.mAnimation[a].y == 0) 
        mStreamBuffer[index].actor->state.mAnimation[a].y += 0.0001f;
    if (mStreamBuffer[index].actor->state.mAnimation[a].z == 0) 
        mStreamBuffer[index].actor->state.mAnimation[a].z += 0.0001f;
}


