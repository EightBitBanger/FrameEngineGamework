#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>


void ActorSystem::UpdateAnimationState(Actor* actor) {
    for (unsigned int a = 0; a < actor->genetics.mGeneticRenderers.size(); a++) {
        // Update the renderers position
        MeshRenderer* geneRenderer = actor->genetics.mGeneticRenderers[a];
        std::lock_guard<std::mutex> (geneRenderer->mux);
        
        //  Skip genes that should not express
        if (!actor->genetics.mGenes[a].doExpress) {
            geneRenderer->isActive = false;
            continue;
        }
        
        // Initial position
        geneRenderer->transform.position = actor->navigation.mPosition;
        geneRenderer->transform.matrix = glm::translate(glm::mat4(1), geneRenderer->transform.position);
        
        // Scale body by age
        float ageScalerValue = glm::clamp((float)actor->physical.mAge * 0.001f, 0.0f, 1.0f);
        float ageScale = Math.Lerp(actor->physical.mYouthScale, actor->physical.mAdultScale, ageScalerValue);
        geneRenderer->transform.matrix = glm::scale(geneRenderer->transform.matrix, glm::vec3(ageScale));
        
        // Determine animation
        switch (actor->genetics.mGenes[a].animationType) {
            
        case ActorState::Animation::Body:
            UpdateAnimationBody(geneRenderer->transform.matrix, actor, geneRenderer, a);
            break;
            
        case ActorState::Animation::Head:
            UpdateAnimationHead(geneRenderer->transform.matrix, actor, geneRenderer, a);
            break;
            
        case ActorState::Animation::Limb:
            UpdateAnimationLimb(geneRenderer->transform.matrix, actor, a);
            break;
            
        }
        
        // Final render scale
        geneRenderer->transform.matrix = glm::scale(geneRenderer->transform.matrix, geneRenderer->transform.scale);
    }
    return;
}


void ActorSystem::UpdateAnimationHead(glm::mat4& matrix, Actor* actor, MeshRenderer* geneRenderer, unsigned int a) {
    // Rotate to body orientation
    float bodyRotationLength = glm::length(actor->navigation.mRotation);
    if (bodyRotationLength != 0.0f) 
        matrix = glm::rotate(matrix, glm::radians(bodyRotationLength), glm::normalize(actor->navigation.mRotation));
    
    // Offset head
    matrix = glm::translate(matrix, glm::vec3(actor->genetics.mGenes[a].offset.x, actor->genetics.mGenes[a].offset.y, actor->genetics.mGenes[a].offset.z));
    
    // Reverse the body rotation
    if (bodyRotationLength != 0.0f) 
        matrix = glm::rotate(matrix, -glm::radians(bodyRotationLength), glm::normalize(actor->navigation.mRotation));
    
    // Rotate head
    UpdateHeadRotation(matrix, actor, a);
    float headRotationLength = glm::length(actor->navigation.mFacing);
    if (headRotationLength != 0.0f) 
        matrix = glm::rotate(matrix, glm::radians(headRotationLength), glm::normalize(actor->navigation.mFacing));
    
    // Genetic translation
    matrix = glm::translate(matrix, glm::vec3(actor->genetics.mGenes[a].position.x, actor->genetics.mGenes[a].position.y, actor->genetics.mGenes[a].position.z));
    
    // Rotate gene
    glm::vec3 baseRotation = glm::vec3(actor->genetics.mGenes[a].rotation.x, actor->genetics.mGenes[a].rotation.y, actor->genetics.mGenes[a].rotation.z);
    float geneRotationLength = glm::length(baseRotation);
    if (geneRotationLength != 0.0f) 
        matrix = glm::rotate(matrix, geneRotationLength, glm::normalize(baseRotation));
    return;
}

void ActorSystem::UpdateAnimationBody(glm::mat4& matrix, Actor* actor, MeshRenderer* geneRenderer, unsigned int a) {
    // Rotate body
    float bodyRotationLength = glm::length(actor->navigation.mRotation);
    if (bodyRotationLength != 0.0f) 
        matrix = glm::rotate(matrix, glm::radians(bodyRotationLength), glm::normalize(actor->navigation.mRotation));
    
    // Translate body
    matrix = glm::translate(matrix, glm::vec3(actor->genetics.mGenes[a].offset.x, actor->genetics.mGenes[a].offset.y, actor->genetics.mGenes[a].offset.z));
    
    // Genetic translation
    matrix = glm::translate(matrix, glm::vec3(actor->genetics.mGenes[a].position.x, actor->genetics.mGenes[a].position.y, actor->genetics.mGenes[a].position.z));
    
    // Rotate gene
    glm::vec3 baseRotation = glm::vec3(actor->genetics.mGenes[a].rotation.x, actor->genetics.mGenes[a].rotation.y, actor->genetics.mGenes[a].rotation.z);
    float geneRotationLength = glm::length(baseRotation);
    if (geneRotationLength != 0.0f) 
        matrix = glm::rotate(matrix, geneRotationLength, glm::normalize(baseRotation));
    return;
}

void ActorSystem::UpdateAnimationLimb(glm::mat4& matrix, Actor* actor, unsigned int a) {
    // Rotate body
    float bodyRotationLength = glm::length(actor->navigation.mRotation);
    if (bodyRotationLength != 0.0f) 
        matrix = glm::rotate(matrix, glm::radians(bodyRotationLength), glm::normalize(actor->navigation.mRotation));
    
    // Translate body
    matrix = glm::translate(matrix, glm::vec3(actor->genetics.mGenes[a].offset.x, actor->genetics.mGenes[a].offset.y, actor->genetics.mGenes[a].offset.z));
    
    if (actor->genetics.mGenes[a].animationType != ActorState::Animation::Limb || !actor->state.mIsWalking) {
        // Genetic translation
        matrix = glm::translate(matrix, glm::vec3(actor->genetics.mGenes[a].position.x, actor->genetics.mGenes[a].position.y, actor->genetics.mGenes[a].position.z));
        
        // Rotate gene
        glm::vec3 baseRotation = glm::vec3(actor->genetics.mGenes[a].rotation.x, actor->genetics.mGenes[a].rotation.y, actor->genetics.mGenes[a].rotation.z);
        float geneRotationLength = glm::length(baseRotation);
        if (geneRotationLength != 0.0f) 
            matrix = glm::rotate(matrix, geneRotationLength, glm::normalize(baseRotation));
        return;
    }
    
    // Apply animation
    ApplyAnimationRotation(matrix, actor, a);
    
    matrix = glm::translate(matrix, glm::vec3(actor->genetics.mGenes[a].position.x, actor->genetics.mGenes[a].position.y, actor->genetics.mGenes[a].position.z));
    
    // Update animation state
    glm::vec4 animationFactor = glm::vec4(actor->genetics.mGenes[a].animationAxis.x, 
                                          actor->genetics.mGenes[a].animationAxis.y, 
                                          actor->genetics.mGenes[a].animationAxis.z, 0);
    // Calculate limb swing
    float animationMaxSwingRange = actor->genetics.mGenes[a].animationRange;
    if (actor->state.mAnimation[a].w < 0) {
        HandleAnimationSwing(actor, a, animationFactor, animationMaxSwingRange, false);
    } else {
        HandleAnimationSwing(actor, a, animationFactor, animationMaxSwingRange, true);
    }
    return;
}

void ActorSystem::UpdateTargetRotation(Actor* actor) {
    // Get target direction angle
    float xx = actor->navigation.mPosition.x - actor->navigation.mTargetPoint.x;
    float zz = actor->navigation.mPosition.z - actor->navigation.mTargetPoint.z;
    
    actor->navigation.mRotateTo.y = glm::degrees( glm::atan(xx, zz) ) + 180.0f;
    
    // Check to invert facing direction
    if (!actor->state.mIsFacing) {
        
        actor->navigation.mRotateTo.y += 180.0f;
        
        if (actor->navigation.mRotateTo.y > 360.0f) 
            actor->navigation.mRotateTo.y -= 360.0f;
    }
    
    // Check actor target direction
    
    // Wrap euler rotations
    if (actor->navigation.mRotation.y < 90.0f) 
        if (actor->navigation.mRotateTo.y > 270.0f) 
            actor->navigation.mRotation.y += 360.0f;
    
    if (actor->navigation.mRotation.y > 270.0f) 
        if (actor->navigation.mRotateTo.y < 90.0f) 
            actor->navigation.mRotation.y -= 360.0f;
    
    // Check actor target direction
    
    // Rotate actor toward the focal point
    if (actor->navigation.mRotation != actor->navigation.mRotateTo) {
        
        glm::vec3 fadeFrom( actor->navigation.mRotation ); // Current rotation
        glm::vec3 fadeTo( actor->navigation.mRotateTo );   // Where we want to rotate towards
        
        glm::vec3 fadeValue;
        fadeValue.x = Math.Lerp(fadeFrom.x, fadeTo.x, actor->physical.mSnapSpeed);
        fadeValue.y = Math.Lerp(fadeFrom.y, fadeTo.y, actor->physical.mSnapSpeed);
        fadeValue.z = Math.Lerp(fadeFrom.z, fadeTo.z, actor->physical.mSnapSpeed);
        
        actor->navigation.mRotation = fadeValue;
    }
    return;
}

void ActorSystem::UpdateHeadRotation(glm::mat4& matrix, Actor* actor, unsigned int a) {
    float xx = actor->navigation.mPosition.x - actor->navigation.mTargetLook.x;
    float zz = actor->navigation.mPosition.z - actor->navigation.mTargetLook.z;
    
    actor->navigation.mLookAt.y = glm::degrees( glm::atan(xx, zz) ) + 180.0f;
    
    // Check to invert facing direction
    if (!actor->state.mIsFacing) {
        
        actor->navigation.mLookAt.y += 180.0f;
        
        if (actor->navigation.mLookAt.y > 360.0f) 
            actor->navigation.mLookAt.y -= 360.0f;
    }
    
    // Check actor target direction
    
    // Wrap euler rotations
    /*
    if (actor->navigation.mFacing.y < 90.0f) 
        if (actor->navigation.mLookAt.y > 270.0f) 
            actor->navigation.mFacing.y += 360.0f;
    
    if (actor->navigation.mFacing.y > 270.0f) 
        if (actor->navigation.mLookAt.y < 90.0f) 
            actor->navigation.mFacing.y -= 360.0f;
    */
    
    // Check actor target direction
    
    // Rotate actor toward the focal point
    if (actor->navigation.mRotation != actor->navigation.mRotateTo) {
        glm::vec3 fadeFrom( actor->navigation.mFacing ); // Current orientation
        glm::vec3 fadeTo( actor->navigation.mLookAt );   // Where we want to look at
        
        glm::vec3 fadeValue;
        fadeValue.x = Math.Lerp(fadeFrom.x, fadeTo.x, actor->physical.mSnapSpeed);
        fadeValue.y = Math.Lerp(fadeFrom.y, fadeTo.y, actor->physical.mSnapSpeed);
        fadeValue.z = Math.Lerp(fadeFrom.z, fadeTo.z, actor->physical.mSnapSpeed);
        
        actor->navigation.mFacing = fadeValue;
    }
    return;
}


void ActorSystem::ApplyAnimationRotation(glm::mat4& matrix, Actor* actor, unsigned int a) {
    EnsureNonZeroAnimationState(actor, a);
    glm::vec3 geneRotation = glm::vec3(actor->genetics.mGenes[a].rotation.x, 
                                       actor->genetics.mGenes[a].rotation.y, 
                                       actor->genetics.mGenes[a].rotation.z) + 0.0001f;
    // Apply genetic orientation
    float geneRotationLength = glm::length(geneRotation);
    matrix = glm::rotate(matrix, geneRotationLength, glm::normalize(geneRotation));
    
    // Apply animation rotation
    float animationLength = glm::length(actor->state.mAnimation[a]);
    matrix = glm::rotate(matrix, glm::radians(animationLength), 
                         glm::normalize(glm::vec3(actor->state.mAnimation[a])));
    return;
}

void ActorSystem::HandleAnimationSwing(Actor* actor, unsigned int a, glm::vec4& animationFactor, float animationMaxSwingRange, bool animationDirection) {
    bool doInverseAnimation = actor->genetics.mGenes[a].doInverseAnimation ^ animationDirection;
    if (doInverseAnimation) {
        actor->state.mAnimation[a] += animationFactor;
        if (glm::any(glm::greaterThan(actor->state.mAnimation[a], glm::vec4(animationMaxSwingRange)))) 
            actor->state.mAnimation[a].w = actor->genetics.mGenes[a].doInverseAnimation ? 1 : -1;
    } else {
        actor->state.mAnimation[a] -= animationFactor;
        if (glm::any(glm::lessThan(actor->state.mAnimation[a], glm::vec4(-animationMaxSwingRange)))) 
            actor->state.mAnimation[a].w = actor->genetics.mGenes[a].doInverseAnimation ? -1 : 1;
    }
}

void ActorSystem::EnsureNonZeroAnimationState(Actor* actor, unsigned int a) {
    if (actor->state.mAnimation[a].x == 0.0f) 
        actor->state.mAnimation[a].x += 0.0001f;
    if (actor->state.mAnimation[a].y == 0.0f) 
        actor->state.mAnimation[a].y += 0.0001f;
    if (actor->state.mAnimation[a].z == 0.0f) 
        actor->state.mAnimation[a].z += 0.0001f;
}

