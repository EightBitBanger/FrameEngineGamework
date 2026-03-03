#include <GameEngineFramework/Engine/Engine.h>
extern EngineSystemManager Engine;

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

void ActorSystem::UpdateActorState(Actor* actor) {
    float decayMul = 0.0f;
    
    float fear      = actor->emotions.GetFear() * decayMul;
    float anger     = actor->emotions.GetAnger() * decayMul;
    float comfort   = actor->emotions.GetComfort() * decayMul;
    float curiosity = actor->emotions.GetCuriosity() * decayMul;
    float fatigue   = actor->emotions.GetFatigue() * decayMul;
    float stress    = actor->emotions.GetStress() * decayMul;
    float libido    = actor->emotions.GetLibido() * decayMul;
    
    
    // Handle actions
    
    const std::string& styleString = actor->memories.mMemories["action"];
    if (styleString != "") {
        std::vector<std::string> styleSplit = String.Explode(styleString, ',');
        
        for (unsigned int i=0; i < styleSplit.size(); i++) {
            String.RemoveWhiteSpace( styleSplit[i] );
            
            std::vector<std::string> amount = String.Explode(styleSplit[i], ':');
            if (amount.empty()) 
                continue;
            
            if (amount[0] == "curiosity") {
                if (amount.size() == 2) {
                    float chance = String.ToFloat(amount[1]);
                    if (Random.Range(0.0f, 1000.0f) < glm::pow(chance, 4.0f)) {
                        curiosity = glm::max(curiosity, chance);
                    }
                }
            }
            
            if (amount[0] == "speak") {
                if (amount.size() == 3) {
                    float chance = String.ToFloat(amount[2]);
                    
                    if (Random.Range(0.0f, 1.0f) < glm::pow(chance, 4.0f)) {
                        Sound* voice = actor->voice.GetVoice(amount[1]);
                        Playback* playback;
                        if (voice != nullptr) {
                            playback = Audio.Play(voice);
                            playback->isGarbage = true;
                        }
                    }
                }
            }
            
        }
    }
    
    // Handle nearby actors
    
    unsigned int numberOfTargets = actor->mTargets.size();
    for (unsigned int i=0; i < numberOfTargets; i++) {
        Actor* targetActor = actor->mTargets[i];
        if (!targetActor->isActive || targetActor->isGarbage) 
            continue;
        
        const std::string& targetName = targetActor->GetName();
        const std::string& memory = actor->memories.mMemories[targetName];
        
        if (memory != "") {
            
            std::vector<std::string> memories = String.Explode(memory, ',');
            for (unsigned int i=0; i < memories.size(); i++) {
                std::vector<std::string> amount = String.Explode(memories[i], ':');
                if (amount.size() == 2) {
                    if (amount[0] == "fear")      fear      = glm::max(fear,      String.ToFloat(amount[1]));
                    if (amount[0] == "anger")     anger     = glm::max(anger,     String.ToFloat(amount[1]));
                    if (amount[0] == "comfort")   comfort   = glm::max(comfort,   String.ToFloat(amount[1]));
                    if (amount[0] == "curiosity") curiosity = glm::max(curiosity, String.ToFloat(amount[1]));
                    if (amount[0] == "fatigue")   fatigue   = glm::max(fatigue,   String.ToFloat(amount[1]));
                    if (amount[0] == "stress")    stress    = glm::max(stress,    String.ToFloat(amount[1]));
                    if (amount[0] == "libido")    libido    = glm::max(libido,    String.ToFloat(amount[1]));
                }
            }
            
            // Apply emotional state
            actor->emotions.SetFear( fear );
            actor->emotions.SetAnger( anger );
            actor->emotions.SetComfort( comfort );
            actor->emotions.SetCuriosity( curiosity );
            actor->emotions.SetFatigue( fatigue );
            actor->emotions.SetStress( stress );
            actor->emotions.SetLibido( libido );
        }
        
        // Handle emotional state
        
        if (anger > fear) {
            if (anger > 0.0f) {
                if (Random.Range(0.0f, 1.0f) < anger) {
                    if (actor->counters.mAttackCoolDownCounter == 0) {
                        actor->state.mode = ActorState::Mode::MoveAttack;
                        actor->navigation.mTargetActor = targetActor;
                    }
                }
                break;
            }
        } else {
            if (Random.Range(0.0f, 1.0f) < fear) {
                if (fear > 0.0f) {
                    actor->state.mode = ActorState::Mode::MoveFlee;
                    actor->navigation.mTargetActor = targetActor;
                    break;
                }
            }
        }
        
        if (curiosity > 0.0f && anger == 0.0f && fear == 0.0f) {
            if (Random.Range(0.0f, 1.0f) < curiosity) {
                actor->state.mode = ActorState::Mode::MoveRandom;
                
            } else if (Random.Range(0.0f, 1.0f) < curiosity) {
                if (actor->state.mode == ActorState::Mode::Idle)
                    actor->state.mode = ActorState::Mode::MoveRandom;
                break;
            }
        }
    }
}

/*
bool ActorSystem::HandleFocusState(Actor* actor, Actor* target, float distance) {
    // Observation cool down
    if (actor->counters.mObservationCoolDownCounter > 0) return false;
    
    // Focus on the selected target
    actor->state.mode    = ActorState::Mode::Idle;
    actor->state.current = ActorState::State::Look;
    actor->navigation.mTargetActor = target;
    target->navigation.mTargetActor = actor;
    
    actor->counters.mObservationCoolDownCounter = actor->behavior.GetCooldownObserve();
    return true;
}


bool ActorSystem::HandleBreedingState(Actor* actor, Actor* target, float distance) {
    if (!actor->isActive || !target->isActive) 
        return false;
    
    if (actor->counters.mBreedingCoolDownCounter > 0 || 
        target->counters.mBreedingCoolDownCounter > 0) 
        return false;
    
    if (actor->physical.mAge < actor->physical.mAgeAdult || 
        actor->physical.mAge > actor->physical.mAgeSenior) 
        return false;
    
    if (actor->navigation.mPosition.y > actor->behavior.mHeightPreferenceMax || 
        actor->navigation.mPosition.y < actor->behavior.mHeightPreferenceMin) 
        if (Random.Range(0, 100) > 20) 
            return false;
    
    if (distance > actor->behavior.GetDistanceToAttack()) {
        actor->state.current = ActorState::State::None;
        return false;
    }
    
    if (actor->physical.GetSexualOrientation() == target->physical.GetSexualOrientation()) 
        return false;
    
    // Focus on the selected target
    actor->state.mode    = ActorState::Mode::WalkTo;
    actor->state.current = ActorState::State::Breed;
    
    target->state.mode    = ActorState::Mode::WalkTo;
    target->state.current = ActorState::State::Breed;
    
    actor->navigation.mTargetActor = target;
    target->navigation.mTargetActor = actor;
    return true;
}
*/

void ActorSystem::UpdateProximityList(Actor* actor) {
    /*
    // Ignore if the actor is engaging with its target
    if (actor->state.current == ActorState::State::Attack || 
        actor->state.current == ActorState::State::Flee || 
        actor->state.current == ActorState::State::Observe) 
        return;
    
    if (actor->navigation.mTargetActor != nullptr) 
        return;
    
    std::vector<Actor*> actorList;
    for (unsigned int i=0; i < mActiveActors.size(); i++) {
        mActiveActors;
        
        // Select a random actor
        
        unsigned int index = Random.Range((unsigned int)0, mActors.Size()-1);
        if (index >= mActors.Size()) 
            return;
    }
    //Actor* targetActor = mActors[index];
    
    if (!targetActor->isActive || targetActor->isGarbage) 
        return;
    
    // Actor cannot target itself
    if (targetActor == actor) 
        return;
    
    // Can the actor see the target
    if (glm::distance(actor->navigation.mPosition, targetActor->navigation.mPosition) > actor->behavior.mDistanceToFocus) 
        return;
    
    // Target the actor
    actor->navigation.mTargetActor = targetActor;
    */
}

