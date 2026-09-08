#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/ActorAI/ActorSystem.h>

extern UniversalConstants UniversalConst;
extern EngineSystemManager Engine;

bool ActorSystem::HandleBreedWith(Actor* actor, Actor* target) {
    if (actor->counters.GetCoolDownBreeding() > 0 || target->counters.GetCoolDownBreeding() > 0) 
        return false;
    if (actor->physical.mAge < actor->physical.mAgeAdult || actor->physical.mAge > actor->physical.mAgeSenior || 
        target->physical.mAge < target->physical.mAgeAdult || target->physical.mAge > target->physical.mAgeSenior) 
        return false;
    
    //
    // Temporary hard population limit
    //
    if (mActiveActors.size() > 200) 
        return false;
    
    // Reproduction impossible when starving
    if (actor->biological.hunger > UniversalConst.biologicalThreshold && 
        actor->biological.saturation < UniversalConst.biologicalThreshold) 
        return false;
    
    // Check to ensure actors are of opposite sexes
    if (actor->physical.mSexualOrientation == target->physical.mSexualOrientation) 
        return false;
    
    //
    // Check interaction distance
    
    float distance = glm::distance(actor->navigation.mPosition, target->navigation.mPosition);
    if (distance > actor->behavior.mDistanceToInflict) 
        return false;
    
    glm::vec3 spawnPoint = Math.Lerp(actor->navigation.mPosition, target->navigation.mPosition, 0.5f);
    
    Actor* offspring = CreateActor();
    offspring->Reset();
    // Genetic recombination
    genomes.BlendGenomes(actor, target, offspring);
    
    // Behavioral recombination (inherits behavior, sentience, and family from selected parent)
    CombineParentMemories(actor, target, offspring);
    
    // Identify parental roles (true = male, false = female)
    Actor* father = actor->physical.GetSexualOrientation() ? actor : target;
    Actor* mother = actor->physical.GetSexualOrientation() ? target : actor;
    
    std::string fatherName = father->memories.Get("name");
    std::string motherName = mother->memories.Get("name");
    
    // TODO New actor name 'language' should reflect 'culture' somehow
    
    // Generate a new first name
    int language  = 0; // Germanic example
    int type      = 0; // First name
    std::string newName = mWorldGetNameCallback(language, type);
    
    // Retrieve the inherited family name from the male parent
    std::string familyName = offspring->memories.Get("family");
    if (familyName.empty()) {
        familyName = father->memories.Get("family");
        if (familyName.empty()) {
            familyName = fatherName; // Seed family name from father if unassigned
        }
        offspring->memories.Add("family", familyName);
    }
    
    Engine.console.Print(fatherName + " and " + motherName + " begat " + newName);
    
    offspring->memories.Add("name", newName);
    
    // Log birth event chronologically
    RecordBirth(offspring, father, mother, familyName, spawnPoint);
    
    std::string homePosition = Float.ToString(spawnPoint.x) + "," + 
                               Float.ToString(spawnPoint.y) + "," + 
                               Float.ToString(spawnPoint.z);
    offspring->memories.Add("home", "home:" + homePosition);
    
    offspring->navigation.SetPosition(spawnPoint);
    offspring->navigation.SetTargetPoint(spawnPoint);
    
    offspring->physical.SetAge( Random.Range(1, 100) );
    offspring->RebuildGeneticExpression();
    offspring->CalculateBoundingRegionFromGenome();
    
    offspring->isActive = true;
    
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
