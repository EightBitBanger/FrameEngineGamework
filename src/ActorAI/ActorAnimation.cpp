#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

static float WrapDeg360(float deg) {
    deg = std::fmod(deg, 360.0f);
    if (deg < 0.0f) deg += 360.0f;
    return deg;
}

static float ShortestDeltaDeg(float fromDeg, float toDeg) {
    float  a = WrapDeg360(fromDeg);
    float  b = WrapDeg360(toDeg);
    float  d = b - a;

    if (d > 180.0f)  d -= 360.0f;
    if (d < -180.0f) d += 360.0f;
    return d;
}

static float LerpAngleDeg(float fromDeg, float toDeg, float t) {
    float  d = ShortestDeltaDeg(fromDeg, toDeg);
    return WrapDeg360(fromDeg + d * t);
}

void ActorSystem::UpdateAnimationState(Actor* actor) {
    float distance = glm::distance(mPlayerPosition, actor->navigation.mPosition);
    float distanceMax = mActorRenderDistance / 2.0f;
    
    for (unsigned int a = 0; a < actor->genetics.mGeneticRenderers.size(); a++) {
        MeshRenderer* geneRenderer = actor->genetics.mGeneticRenderers[a];
        
        if (distance > distanceMax) {
            if (a > 0) {
                geneRenderer->isActive = false;
            }
        } else {
            geneRenderer->isActive = true;
        }
        
        //  Skip genes that should not express
        if (!actor->genetics.mGenes[a].doExpress) {
            geneRenderer->isActive = false;
            continue;
        }
        
        if (!geneRenderer->isActive) 
            continue;
        
        // Scale body by age
        float ageScaler = 1.0f;
        float specialScaler = 1.0f;
        if (actor->physical.mAge < actor->physical.mAgeAdult) 
            ageScaler = (float)actor->physical.mAge / (float)actor->physical.mAgeAdult;
        
        // Scale age specific expressions
        if (actor->genetics.mGenes[a].expressionAge > 0.0f) {
            if (actor->physical.mAge > actor->genetics.mGenes[a].expressionAge) {
                specialScaler = (actor->physical.mAge - actor->genetics.mGenes[a].expressionAge) * 0.001f;
                if (specialScaler > 1.0f) 
                    specialScaler = 1.0f;
            }
        }
        
        // Initial position
        geneRenderer->transform.position = actor->navigation.mPosition;
        geneRenderer->transform.matrix = glm::translate(glm::mat4(1), geneRenderer->transform.position);
        
        float ageScale = Math.Lerp((float)actor->physical.mYouthScale, (float)actor->physical.mAdultScale, ageScaler);
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
        geneRenderer->transform.matrix = glm::scale(geneRenderer->transform.matrix, geneRenderer->transform.scale * specialScaler);
    }
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
}

void ActorSystem::UpdateAnimationLimb(glm::mat4& matrix, Actor* actor, unsigned int a) {
    // Rotate
    float bodyRotationLength = glm::length(actor->navigation.mRotation);
    if (bodyRotationLength != 0.0f) 
        matrix = glm::rotate(matrix, glm::radians(bodyRotationLength), glm::normalize(actor->navigation.mRotation));
    
    // Translate
    matrix = glm::translate(matrix, glm::vec3(actor->genetics.mGenes[a].offset.x, actor->genetics.mGenes[a].offset.y, actor->genetics.mGenes[a].offset.z));
    
    if (actor->genetics.mGenes[a].animationType != ActorState::Animation::Limb || !actor->state.mIsWalking) {
        // Rotate gene
        glm::vec3 baseRotation = glm::vec3(actor->genetics.mGenes[a].rotation.x, actor->genetics.mGenes[a].rotation.y, actor->genetics.mGenes[a].rotation.z);
        float geneRotationLength = glm::length(baseRotation);
        if (geneRotationLength != 0.0f) 
            matrix = glm::rotate(matrix, geneRotationLength, glm::normalize(baseRotation));
        
        // Genetic translation
        matrix = glm::translate(matrix, glm::vec3(actor->genetics.mGenes[a].position.x, actor->genetics.mGenes[a].position.y, actor->genetics.mGenes[a].position.z));
        
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
}

void ActorSystem::UpdateTargetRotation(Actor* actor) {
    float  dx    = actor->navigation.mPosition.x - actor->navigation.mTargetPoint.x;
    float  dz    = actor->navigation.mPosition.z - actor->navigation.mTargetPoint.z;
    float  dist2 = (dx * dx) + (dz * dz);
    
    // Check if we're basically at the point, the direction is unstable
    if (dist2 < 0.000001f)
        return;
    
    float  desiredYaw = glm::degrees(glm::atan(dx, dz));
    
    if (actor->state.mIsFacing)
        desiredYaw += 180.0f;
    
    desiredYaw = WrapDeg360(desiredYaw);
    
    actor->navigation.mRotateTo.x = 0.0f;
    actor->navigation.mRotateTo.y = desiredYaw;
    actor->navigation.mRotateTo.z = 0.0f;
    
    float  currentYaw = WrapDeg360(actor->navigation.mRotation.y);
    float  deltaYaw   = ShortestDeltaDeg(currentYaw, desiredYaw);
    
    // Snap when close enough so we don't jitter forever on float error.
    const float kSnapEpsDeg = 0.25f;
    if (std::fabs(deltaYaw) <= kSnapEpsDeg) {
        actor->navigation.mRotation.y = desiredYaw;
        return;
    }
    
    float t = glm::clamp(actor->physical.mSnapSpeed, 0.0f, 1.0f);
    
    actor->navigation.mRotation.y = LerpAngleDeg(currentYaw, desiredYaw, t);
    
    // Keep your existing behavior of damping x/z back to 0 over time.
    actor->navigation.mRotation.x = Math.Lerp(actor->navigation.mRotation.x, 0.0f, t);
    actor->navigation.mRotation.z = Math.Lerp(actor->navigation.mRotation.z, 0.0f, t);
}

void ActorSystem::UpdateHeadRotation(glm::mat4& matrix, Actor* actor, unsigned int a) {
    float xx = actor->navigation.mPosition.x - actor->navigation.mTargetLook.x;
    float zz = actor->navigation.mPosition.z - actor->navigation.mTargetLook.z;
    
    actor->navigation.mLookAt.y = glm::degrees( glm::atan(xx, zz) ) + 180.0f;
    
    if (!actor->state.mIsFacing) 
        actor->navigation.mLookAt.y += 180.0f;
    
    // Rotate actor toward the focal point
    if (actor->navigation.mFacing != actor->navigation.mLookAt) {
        glm::vec3 fadeFrom( actor->navigation.mFacing ); // Current orientation
        glm::vec3 fadeTo( actor->navigation.mLookAt );   // Where we want to look at
        
        glm::vec3 fadeValue;
        fadeValue.x = Math.Lerp(fadeFrom.x, fadeTo.x, actor->physical.mSnapSpeed);
        fadeValue.y = Math.Lerp(fadeFrom.y, fadeTo.y, actor->physical.mSnapSpeed);
        fadeValue.z = Math.Lerp(fadeFrom.z, fadeTo.z, actor->physical.mSnapSpeed);
        
        actor->navigation.mFacing = fadeValue;
    }
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

