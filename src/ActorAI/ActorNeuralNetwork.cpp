#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>


void ActorSystem::HandleNeuralNetwork(Actor* actor) {
    
    // Height preference max below water suggests
    // the actor is an aquatic actor
    bool isAquatic = actor->behavior.mHeightPreferenceMax < mWorldWaterLevel;
    
    // Extract neural network outputs and 
    // apply actions to the actor
    actor->idiosyncrasies.mNeuralNetwork.FeedForward( Encode( NeuralState::walk ) );
    std::vector<float> resultVals = actor->idiosyncrasies.mNeuralNetwork.GetResults();
    
    // High order decision
    float walkChance    = resultVals[0];
    float focusChance   = resultVals[1];
    float attackChance  = resultVals[2];
    float fleeChance    = resultVals[3];
    
    // Calculate decisions
    float decisionChanceToWalk   = walkChance   * 100.0f;
    float decisionChanceToFocus  = focusChance  * 100.0f;
    float decisionChanceToAttack = attackChance * 100.0f;
    float decisionChanceToFlee   = fleeChance   * 100.0f;
    
    // Decrement cool down counter
    if (actor->counters.mBreedingCoolDownCounter > 0) 
        actor->counters.mBreedingCoolDownCounter--;
    
    // Walk toward target point
    if (Random.Range(0.0f, decisionChanceToWalk) < 1.0f) 
        HandleWalkingChance(actor, isAquatic);
    
    // Interest in nearby actors
    if (Random.Range(0.0f, decisionChanceToFocus) < 1.0f) {
        
        // Focus on actor
        HandleFocusOnNearbyActor(actor);
        
    }
    
    // Breeding state
    if (actor->navigation.mTargetBreeding != nullptr) 
        HandleBreedingState(actor);
    
    // Check target actor
    if (actor->navigation.mTargetActor != nullptr) {
        
        // Face toward the actor
        actor->state.mIsFacing = true;
        
        // Focus on the target actor
        if (actor->navigation.mTargetActor) {
            
            actor->navigation.mTargetPoint.x = actor->navigation.mTargetActor->navigation.mPosition.x;
            actor->navigation.mTargetPoint.y = 0.0f;
            actor->navigation.mTargetPoint.z = actor->navigation.mTargetActor->navigation.mPosition.z;
            
            // Walk towards the target actor
            actor->state.mIsWalking = glm::distance(actor->navigation.mPosition, actor->navigation.mTargetActor->navigation.mPosition) > DISTANCE_MINIMUM_TARGET_BREEDING;
            
            return;
        }
        
    }
    
    return;
}


std::vector<float> ActorSystem::Encode(const NeuralState neuralState) {
    
    switch (neuralState) {
        
    default:
        
        case NeuralState::idle:    return {0.5f, 0.5f};
        case NeuralState::walk:    return {0.3f, 0.5f};
        
        case NeuralState::avoid:   return {0.1f, 0.8f};
        case NeuralState::dodge:   return {0.1f, 0.5f};
        
        case NeuralState::attack:  return {0.8f, 0.2f};
        case NeuralState::follow:  return {0.5f, 0.2f};
        
        case NeuralState::flee:    return {0.2f, 0.2f};
        case NeuralState::defend:  return {0.8f, 0.8f};
        
    }
    
    return {0.5f, 0.5f};
}

