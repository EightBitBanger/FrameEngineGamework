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
    
    if (HandleBreedingState(actor)) 
        return;
    
    if (!isAquatic && HandleWalkingChance(actor)) 
        return;
    
    HandleObservationCooldown(actor);
    HandleFocusOnNearbyActor(actor);
    HandleMovementCooldown(actor, isAquatic);
    HandleStopWalkingChance(actor);
    HandleNeuralNetwork(actor);
    
    return;
}

void ActorSystem::HandleNeuralNetwork(Actor* actor) {
    
    /*
    
    // Define the topology of the neural network
    std::vector<int> topology = { 3, 2, 1 };
    NeuralNetwork myNet(topology);
    
    // Define input values
    std::vector<double> inputVals = { 1.0, 0.5, -1.5 };
    myNet.feedForward(inputVals);
    
    // Define target values
    std::vector<double> targetVals = { 0.8 };
    myNet.backProp(targetVals);
    
    // Get the results
    std::vector<double> resultVals;
    myNet.getResults(resultVals);
    
    */
    
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
    
    if (actor->mTargetPoint.y > mWorldWaterLevel && Random.Range(0, DECISION_CHANCE_TO_WALK) < actor->mChanceToWalk) {
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
            float randA = Random.Range(0.0f, actor->mDistanceToWalk) + (actor->mMovementCoolDownCounter + 1);
            float randB = Random.Range(0.0f, actor->mDistanceToWalk) + (actor->mMovementCoolDownCounter + 1);
            float randC = Random.Range(0.0f, actor->mDistanceToWalk) + (actor->mMovementCoolDownCounter + 1);
            float randD = Random.Range(0.0f, actor->mDistanceToWalk) + (actor->mMovementCoolDownCounter + 1);
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
