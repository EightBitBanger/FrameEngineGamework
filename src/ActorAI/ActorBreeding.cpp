#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/ActorAI/ActorSystem.h>

extern UniversalConstants UniversalConst;
extern EngineSystemManager Engine;

bool ActorSystem::HandleBreedWith(Actor* actor, Actor* target) {
    if (actor->counters.GetCoolDownBreeding() > 0 || target->counters.GetCoolDownBreeding() > 0) 
        return false;
    if (actor->physical.mAge < actor->physical.mAgeAdult || target->physical.mAge >= target->physical.mAgeSenior) 
        return false;
    
    // Check to ensure actors are of opposite sexes
    if (actor->physical.mSexualOrientation == target->physical.mSexualOrientation) 
        return false;
    
    // Check interaction distance
    float distance = glm::distance(actor->navigation.mPosition, target->navigation.mPosition);
    if (distance > actor->behavior.mDistanceToInflict) 
        return false;
    
    glm::vec3 spawnPoint = Math.Lerp(actor->navigation.mPosition, target->navigation.mPosition, 0.5f);
    
    Actor* offspring = CreateActor();
    offspring->Reset();
    
    // Genetic recombination
    genomes.BlendGenomes(actor, target, offspring);
    
    // Behavioral recombination
    CombineParentMemories(actor, target, offspring);
    
    std::string homePosition = Float.ToString(spawnPoint.x) + "`" +
                               Float.ToString(spawnPoint.y) + "`" +
                               Float.ToString(spawnPoint.z);
    offspring->memories.Add("home", "home:" + homePosition);
    
    offspring->navigation.SetPosition(spawnPoint);
    offspring->navigation.SetTargetPoint(spawnPoint);
    
    offspring->physical.SetAge( Random.Range(1, 100) );
    offspring->RebuildGeneticExpression();
    
    offspring->isActive = true;
    
    mActiveActors.push_back(offspring);
    
    actor->counters.SetCoolDownBreeding(actor->behavior.GetCooldownBreed());
    target->counters.SetCoolDownBreeding(target->behavior.GetCooldownBreed());
    
    actor->emotions.current.libido = 0.0f;
    target->emotions.current.libido = 0.0f;
    
    actor->memories.ClearEmotion(TriggerType::Libido);
    target->memories.ClearEmotion(TriggerType::Libido);
    
    actor->state.mode = ActorState::Mode::Idle;
    target->state.mode = ActorState::Mode::Idle;
    
    offspring->state.mode = ActorState::Mode::Idle;
    return true;
}
