#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>


void ActorSystem::UpdateAnimationState(Actor* actor) {
    
    for (unsigned int a = 0; a < actor->genetics.mGeneticRenderers.size(); a++) {
        
        MeshRenderer* geneRenderer = actor->genetics.mGeneticRenderers[a];
        
        geneRenderer->transform.position = actor->navigation.mPosition;
        
        glm::mat4 matrix = glm::translate(glm::mat4(1), geneRenderer->transform.position);
        
        ApplyScaleByAge(matrix, actor, a);
        ApplyRotation(matrix, actor, a);
        ApplyOffsetFromCenter(matrix, actor, a);
        UpdateAnimation(matrix, actor, a);
        
        geneRenderer->transform.matrix = glm::scale(matrix, geneRenderer->transform.scale);
    }
    
    return;
}

void ActorSystem::ApplyScaleByAge(glm::mat4& matrix, Actor* actor, unsigned int a) {
    
    float ageScalerValue = glm::clamp((float)actor->physical.mAge * 0.001f, 0.0f, 1.0f);
    
    float ageScale = Math.Lerp(actor->physical.mYouthScale, 
                               actor->physical.mAdultScale, 
                               ageScalerValue);
    
    matrix = glm::scale(matrix, glm::vec3(ageScale));
    
    return;
}

void ActorSystem::ApplyRotation(glm::mat4& matrix, Actor* actor, unsigned int a) {
    
    float orientationCenterMass = glm::length(actor->navigation.mRotation);
    
    if (orientationCenterMass != 0) {
        matrix = glm::rotate(matrix, glm::radians(orientationCenterMass), 
                             glm::normalize(actor->navigation.mRotation));
    }
    
    return;
}

void ActorSystem::ApplyOffsetFromCenter(glm::mat4& matrix, Actor* actor, unsigned int a) {
    matrix = glm::translate(matrix, glm::vec3(actor->genetics.mGenes[a].offset.x, actor->genetics.mGenes[a].offset.y, actor->genetics.mGenes[a].offset.z));
}

void ActorSystem::UpdateAnimation(glm::mat4& matrix, Actor* actor, unsigned int a) {
    
    // Set base rotation
    glm::vec3 baseRotation = glm::vec3(actor->genetics.mGenes[a].rotation.x, actor->genetics.mGenes[a].rotation.y, actor->genetics.mGenes[a].rotation.z);
    
    float rotationLength = glm::length(baseRotation);
    
    // Check head animation rotation
    if (actor->genetics.mGenes[a].doAnimateAsHead) {
        glm::vec3 headPos(0);
        headPos = actor->navigation.mPosition;
        headPos.x += actor->genetics.mGenes[a].offset.x + actor->genetics.mGenes[a].position.x;
        //headPos.y += actor->genetics.mGenes[a].offset.y + actor->genetics.mGenes[a].position.y;
        //headPos.z += actor->genetics.mGenes[a].offset.z + actor->genetics.mGenes[a].position.z;
        
        glm::vec3 lookDir(0.0f);
        lookDir.x = actor->navigation.mTargetLook.x - headPos.x;
        //lookDir.y = actor->navigation.mTargetLook.y - headPos.y;
        //lookDir.z = actor->navigation.mTargetLook.z - headPos.z;
        
        if (glm::length(lookDir) > 0.01f) {
            lookDir = glm::normalize(lookDir);
            
            glm::vec3 defaultForward(0.0f, 0.0f, 1.0f);
            float dot = glm::dot(defaultForward, lookDir);
            float angle = glm::acos(glm::clamp(dot, -1.0f, 1.0f)) - glm::radians(90.0f);
            
            glm::vec3 axis = glm::cross(defaultForward, lookDir);
            if (glm::length(axis) > 0.001f) {
                axis = glm::normalize(axis);
                matrix = glm::translate(matrix, glm::vec3(actor->genetics.mGenes[a].offset.x, actor->genetics.mGenes[a].offset.y, actor->genetics.mGenes[a].offset.z));
                matrix = glm::rotate(matrix, angle, axis);
                matrix = glm::translate(matrix, glm::vec3(actor->genetics.mGenes[a].position.x, actor->genetics.mGenes[a].position.y, actor->genetics.mGenes[a].position.z));
            } else {
                matrix = glm::translate(matrix, glm::vec3(actor->genetics.mGenes[a].offset.x, actor->genetics.mGenes[a].offset.y, actor->genetics.mGenes[a].offset.z));
                matrix = glm::translate(matrix, glm::vec3(actor->genetics.mGenes[a].position.x, actor->genetics.mGenes[a].position.y, actor->genetics.mGenes[a].position.z));    
            }
        }
        return;
    }
    
    // Check skip animation for this renderer
    if (!actor->genetics.mGenes[a].doAnimationCycle || !actor->state.mIsWalking) {
        matrix = glm::translate(matrix, glm::vec3(actor->genetics.mGenes[a].position.x, actor->genetics.mGenes[a].position.y, actor->genetics.mGenes[a].position.z));
        if (rotationLength != 0.0f) matrix = glm::rotate(matrix, rotationLength, glm::normalize(baseRotation));
        return;
    }
    
    // Translate then rotate for proper limb animations
    if (rotationLength != 0.0f) {
        matrix = glm::translate(matrix, glm::vec3(actor->genetics.mGenes[a].position.x, actor->genetics.mGenes[a].position.y, actor->genetics.mGenes[a].position.z));
        matrix = glm::rotate(matrix, rotationLength, glm::normalize(baseRotation));
        return;
    } else {
        ApplyAnimationRotation(matrix, actor, a);
        matrix = glm::translate(matrix, glm::vec3(actor->genetics.mGenes[a].position.x, actor->genetics.mGenes[a].position.y, actor->genetics.mGenes[a].position.z));
    }
    
    glm::vec4 animationFactor = glm::normalize(glm::vec4(actor->genetics.mGenes[a].animationAxis.x, 
    actor->genetics.mGenes[a].animationAxis.y, actor->genetics.mGenes[a].animationAxis.z, 0));
    
    float animationMaxSwingRange = actor->genetics.mGenes[a].animationRange;
    if (actor->state.mAnimation[a].w < 0) {
        HandleAnimationSwingForward(actor, a, animationFactor, animationMaxSwingRange);
    } else {
        HandleAnimationSwingBackward(actor, a, animationFactor, animationMaxSwingRange);
    }
    
    return;
}

void ActorSystem::HandleAnimationSwingForward(Actor* actor, unsigned int a, glm::vec4& animationFactor, float animationMaxSwingRange) {
    if (actor->genetics.mGenes[a].doInverseAnimation) {
        actor->state.mAnimation[a] += animationFactor;
        if (glm::any(glm::greaterThan(actor->state.mAnimation[a], glm::vec4(animationMaxSwingRange)))) {
            actor->state.mAnimation[a].w = actor->genetics.mGenes[a].doInverseAnimation ? 1 : -1;
        }
    } else {
        actor->state.mAnimation[a] -= animationFactor;
        if (glm::any(glm::lessThan(actor->state.mAnimation[a], glm::vec4(-animationMaxSwingRange)))) {
            actor->state.mAnimation[a].w = actor->genetics.mGenes[a].doInverseAnimation ? -1 : 1;
        }
    }
}

void ActorSystem::HandleAnimationSwingBackward(Actor* actor, unsigned int a, glm::vec4& animationFactor, float animationMaxSwingRange) {
    if (!actor->genetics.mGenes[a].doInverseAnimation) {
        actor->state.mAnimation[a] += animationFactor;
        if (glm::any(glm::greaterThan(actor->state.mAnimation[a], glm::vec4(animationMaxSwingRange)))) {
            actor->state.mAnimation[a].w = actor->genetics.mGenes[a].doInverseAnimation ? 1 : -1;
        }
    } else {
        actor->state.mAnimation[a] -= animationFactor;
        if (glm::any(glm::lessThan(actor->state.mAnimation[a], glm::vec4(-animationMaxSwingRange)))) {
            actor->state.mAnimation[a].w = actor->genetics.mGenes[a].doInverseAnimation ? -1 : 1;
        }
    }
}

void ActorSystem::ApplyAnimationRotation(glm::mat4& matrix, Actor* actor, unsigned int a) {
    EnsureNonZeroAnimationState(actor, a);
    glm::vec3 baseRotation = glm::vec3(actor->genetics.mGenes[a].rotation.x, 
                                       actor->genetics.mGenes[a].rotation.y, 
                                       actor->genetics.mGenes[a].rotation.z) + 0.0001f;
    
    float rotationLength = glm::length(baseRotation);
    matrix = glm::rotate(matrix, rotationLength, glm::normalize(baseRotation));
    
    float animationLength = glm::length(actor->state.mAnimation[a]);
    matrix = glm::rotate(matrix, glm::radians(animationLength), 
                         glm::normalize(glm::vec3(actor->state.mAnimation[a])));
    
    return;
}

void ActorSystem::EnsureNonZeroAnimationState(Actor* actor, unsigned int a) {
    if (actor->state.mAnimation[a].x == 0) 
        actor->state.mAnimation[a].x += 0.0001f;
    if (actor->state.mAnimation[a].y == 0) 
        actor->state.mAnimation[a].y += 0.0001f;
    if (actor->state.mAnimation[a].z == 0) 
        actor->state.mAnimation[a].z += 0.0001f;
}


void ActorSystem::UpdateTargetRotation(Actor* actor) {
    // Get target direction angle
    float xx = actor->navigation.mPosition.x - actor->navigation.mTargetPoint.x;
    float zz = actor->navigation.mPosition.z - actor->navigation.mTargetPoint.z;
    
    actor->navigation.mRotateTo.y = glm::degrees( glm::atan(xx, zz) ) + 180;
    
    // Check to invert facing direction
    if (!actor->state.mIsFacing) {
        
        actor->navigation.mRotateTo.y += 180;
        
        if (actor->navigation.mRotateTo.y > 360) 
            actor->navigation.mRotateTo.y -= 360;
    }
    
    // Check actor target direction
    
    // Wrap euler rotations
    if (actor->navigation.mRotation.y < 90) 
        if (actor->navigation.mRotateTo.y > 270) 
            actor->navigation.mRotation.y += 360;
    
    if (actor->navigation.mRotation.y > 270) 
        if (actor->navigation.mRotateTo.y < 90) 
            actor->navigation.mRotation.y -= 360;
    
    // Check actor target direction
    
    // Rotate actor toward the focal point
    if (actor->navigation.mRotation != actor->navigation.mRotateTo) {
        
        glm::vec3 fadeFrom( actor->navigation.mRotation ); // Current rotation
        glm::vec3 fadeTo( actor->navigation.mRotateTo );   // Where we want to rotate to
        
        glm::vec3 fadeValue;
        fadeValue.x = Math.Lerp(fadeFrom.x, fadeTo.x, actor->physical.mSnapSpeed);
        fadeValue.y = Math.Lerp(fadeFrom.y, fadeTo.y, actor->physical.mSnapSpeed);
        fadeValue.z = Math.Lerp(fadeFrom.z, fadeTo.z, actor->physical.mSnapSpeed);
        
        actor->navigation.mRotation = fadeValue;
    }
    
    return;
}

