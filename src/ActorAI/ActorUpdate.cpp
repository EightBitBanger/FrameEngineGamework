#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

extern Logger Log;
extern ActorSystem AI;
extern NumberGeneration Random;
extern MathCore Math;

extern bool isActorThreadActive;
extern bool doUpdate;

extern int actorCounter;
extern int tickCounter;

#define NUMBER_OF_TICKS  10000000

#define DECISION_CHANCE_TO_WALK                  1000
#define DECISION_CHANCE_TO_CHANGE_DIRECTION      1000
#define DECISION_CHANCE_TO_FOCUS_NEARBY          10000
#define DECISION_CHANCE_TO_STOP_MOVING           10000
#define COOLDOWN_OBSERVATION_MAX                 100
#define COOLDOWN_MOVEMENT_MAX                    100

#define DISTANCE_MINIMUM_TARGET_REACHED          1.5f
#define DISTANCE_MINIMUM_TARGET_BREEDING         1.0f

void ActorSystem::Update(void) {
    // Tick counter
    tickCounter++;
    if (tickCounter < NUMBER_OF_TICKS) 
        return;
    
    tickCounter = 0;
    int numberOfActors = mActors.Size();
    int numberOfActorsPerCycle = (numberOfActors > 10) ? (numberOfActors / 10) : 1;
    
    mux.lock();
    for (int i = 0; i < numberOfActorsPerCycle; i++) {
        
        if (actorCounter >= numberOfActors) {
            actorCounter = 0;
            doUpdate = false;
            break;
        }
        
        Actor* actor = mActors[actorCounter++];
        if (!actor->mIsActive) 
            continue;
        
        actor->mux.lock();
        
        actor->mDistance = glm::distance(mPlayerPosition, actor->mPosition);
        
        if (actor->mDistance > mActorUpdateDistance) {
            actor->mux.unlock();
            continue;
        }
        
        UpdateActorState(actor);
        
        actor->mux.unlock();
    }
    mux.unlock();
}

void ActorSystem::UpdateActorState(Actor* actor) {
    
    actor->mAge++;
    
    bool isAquatic = actor->mHeightPreferenceMax < mWorldWaterLevel;
    
    HandleWalkingChance(actor);
    HandleObservationCooldown(actor);
    HandleFocusOnNearbyActor(actor);
    HandleMovementCooldown(actor, isAquatic);
    HandleStopWalkingChance(actor);
    
    //HandleNeuralNetwork(actor);
    
    return;
}

void ActorSystem::HandleNeuralNetwork(Actor* actor) {
    
    // Send in some input values
    glm::vec3 vel = actor->mVelocity;
    glm::vec3 rot = actor->mRotation;
    float speed = actor->mSpeed;
    
    std::vector<float> inputVals = {vel.x, vel.y, vel.z,
                                    rot.x, rot.y, rot.z,
                                    speed};
    
    // Feed through the network
    actor->mNeuralNetwork.FeedForward(inputVals);
    
    // Extract results
    std::vector<float> resultVals = actor->mNeuralNetwork.GetResults();
    
    unsigned int numberOfOutputs = resultVals.size();
    if (numberOfOutputs > 6) {
        
        if (resultVals[0] > 0.9999f) HandleObservationCooldown(actor);
        if (resultVals[2] > 0.9999f) HandleFocusOnNearbyActor(actor);
        if (resultVals[3] > 0.9999f) HandleMovementCooldown(actor, false);
        if (resultVals[5] > 0.9999f) HandleStopWalkingChance(actor);
        if (resultVals[6] > 0.9999f) HandleBreedingState(actor);
        
    }
    
    
    return;
}




bool ActorSystem::HandleBreedingState(Actor* actor) {
    
    if ((actor->mTargetPoint.y < actor->mHeightPreferenceMax) | (actor->mTargetPoint.y > actor->mHeightPreferenceMin)) {
        
        if (actor->mBreedWithActor) {
            
            actor->mTargetPoint = actor->mBreedWithActor->mPosition;
            actor->mIsWalking = glm::distance(actor->mPosition, actor->mBreedWithActor->mPosition) > DISTANCE_MINIMUM_TARGET_BREEDING;
            
            return true;
        }
    }
    
    return false;
}

bool ActorSystem::HandleWalkingChance(Actor* actor) {
    
    if (actor->mTargetPoint.y > mWorldWaterLevel) {
        
        if (Random.Range(0, DECISION_CHANCE_TO_WALK) < actor->mChanceToWalk) 
            actor->mIsWalking = true;
        
        return true;
    }
    
    return false;
}

void ActorSystem::HandleObservationCooldown(Actor* actor) {
    
    if (actor->mObservationCoolDownCounter < COOLDOWN_OBSERVATION_MAX) 
        actor->mObservationCoolDownCounter++;
    
    return;
}

void ActorSystem::HandleFocusOnNearbyActor(Actor* actor) {
    
    if (Random.Range(0, DECISION_CHANCE_TO_FOCUS_NEARBY) < actor->mChanceToFocusOnActor) {
        
        if (actor->mDistance < actor->mDistanceToFocusOnActor) {
            
            actor->mTargetPoint = mPlayerPosition;
            actor->mObservationCoolDownCounter = 0;
            
            actor->mIsWalking = false;
            actor->mIsRunning = false;
            
        } else {
            
            // TODO: Focus on nearby actor.
            
        }
    }
    
    return;
}

void ActorSystem::HandleMovementCooldown(Actor* actor, bool isAquatic) {
    
    if (!actor->mIsWalking && actor->mObservationCoolDownCounter != 0) {
        
        actor->mObservationCoolDownCounter = 0;
        
        if (Random.Range(0, DECISION_CHANCE_TO_CHANGE_DIRECTION) < actor->mChanceToChangeDirection) {
            
            float randA = Random.Range(0.0f, actor->mDistanceToWalk);
            float randB = Random.Range(0.0f, actor->mDistanceToWalk);
            float randC = Random.Range(0.0f, actor->mDistanceToWalk);
            float randD = Random.Range(0.0f, actor->mDistanceToWalk);
            
            // Add cool down to expand random range distance
            randA += actor->mMovementCoolDownCounter + 1;
            randB += actor->mMovementCoolDownCounter + 1;
            randC += actor->mMovementCoolDownCounter + 1;
            randD += actor->mMovementCoolDownCounter + 1;
            
            actor->mTargetPoint.x = (randA - randB) + actor->mPosition.x;
            actor->mTargetPoint.z = (randC - randD) + actor->mPosition.z;
        }
        
    } else {
        
        if ((actor->mTargetPoint.y > actor->mHeightPreferenceMax) | (actor->mTargetPoint.y < actor->mHeightPreferenceMin)) {
            
            if (++actor->mMovementCoolDownCounter > COOLDOWN_MOVEMENT_MAX) 
                actor->mMovementCoolDownCounter = COOLDOWN_MOVEMENT_MAX;
            
            if (!isAquatic) {
                
                if (actor->mPosition.y < mWorldWaterLevel && actor->mTargetPoint.y < actor->mPosition.y) 
                    actor->mIsWalking = false;
                
            } else {
                
                if (actor->mPosition.y > mWorldWaterLevel && actor->mTargetPoint.y > actor->mPosition.y) 
                    actor->mIsWalking = false;
            }
        }
        
        if (glm::distance(actor->mTargetPoint, actor->mPosition) < DISTANCE_MINIMUM_TARGET_REACHED) {
            actor->mMovementCoolDownCounter = 0;
            actor->mIsWalking = false;
        }
    }
    
    return;
}

void ActorSystem::HandleStopWalkingChance(Actor* actor) {
    
    if (Random.Range(0, DECISION_CHANCE_TO_STOP_MOVING) < actor->mChanceToStopWalking) 
        actor->mIsWalking = false;
    
    return;
}
