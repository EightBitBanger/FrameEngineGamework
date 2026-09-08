#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

extern UniversalConstants UniversalConst;
extern EngineSystemManager Engine;

bool ActorSystem::EvaluateEmotionalBehavior(Actor* actor, Actor* targetActor, float threshold, EmotionalEmbedding& emotion) {
    // Lock on combat target while active or recovering from cooldown
    if ((actor->state.mode == ActorState::Mode::MoveAttack || actor->counters.mAttackCoolDownCounter > 0) && 
        actor->navigation.mTargetActor != nullptr && 
        actor->navigation.mTargetActor->isActive && 
        !actor->navigation.mTargetActor->isGarbage) {
        targetActor = actor->navigation.mTargetActor;
    }
    
    // =========================================================================
    // SURVIVAL & COMBAT (Fear > Anger)
    
    // Fear (Fleeing)
    if (emotion.fear >= threshold && emotion.fear >= emotion.anger) {
        emotion.comfort *= 0.1f;
        actor->memories.ScaleEmotion(TriggerType::Comfort, 0.1f);
        
        if (actor->state.mode != ActorState::Mode::MoveFlee) {
            if (targetActor != nullptr && glm::distance(actor->navigation.mPosition, targetActor->navigation.mPosition) <= actor->behavior.GetDistanceToFlee()) {
                if (Random.Range(0.0f, 1.0f) < glm::pow(emotion.fear, threshold)) {
                    actor->state.mode = ActorState::Mode::MoveFlee;
                    actor->inventory.UnequipItem();
                    actor->navigation.mTargetActor = targetActor;
                    return true;
                }
            }
        } else {
            return true;
        }
    }
    
    // Anger (Attacking)
    bool isHostile = (emotion.anger >= threshold || actor->counters.mAttackCoolDownCounter > 0);
    if (emotion.anger >= threshold && emotion.anger > emotion.fear) {
        // Ensure a target exists before initiating combat navigation
        if (targetActor != nullptr && actor->counters.mAttackCoolDownCounter == 0) {
            actor->state.mode = ActorState::Mode::MoveAttack;
            emotion.comfort *= 0.3f;
            actor->memories.ScaleEmotion(TriggerType::Comfort, 0.3f);
            
            if (actor->inventory.inHandItemClass.empty()) 
                actor->inventory.EquipWeapon();
            actor->navigation.mTargetActor = targetActor;
            actor->navigation.mDistanceToTarget = glm::distance(
                glm::vec3(targetActor->navigation.mPosition.x, 0.0f, targetActor->navigation.mPosition.z),
                glm::vec3(actor->navigation.mPosition.x, 0.0f, actor->navigation.mPosition.z)
            );
            return true;
        } else {
            return false;
        }
    }
    
    // Block non-combat behaviors completely while hostile
    if (isHostile) 
        return false;
    
    // =========================================================================
    // SLEEP
    
    bool isSleepCycle = actor->sleep.IsSleepPeriod(mTimeOfDay);
    
    if (isSleepCycle) {
        // Do not sleep if actively engaged in combat or fleeing under threat
        bool isThreatened = (actor->counters.mAttackCoolDownCounter > 0) || 
                            (targetActor != nullptr && (emotion.fear >= threshold || emotion.anger >= threshold));
        
        if (!isThreatened) {
            bool hasBed = false;
            glm::vec3 bedPosition(0.0f);
            std::unordered_map<std::string, std::vector<MemoryTrigger>>::iterator bedTriggerIt = actor->memories.mMemoryTriggers.find("bed");
            if (bedTriggerIt != actor->memories.mMemoryTriggers.end() && !bedTriggerIt->second.empty()) {
                bedPosition = bedTriggerIt->second[0].vector;
                hasBed = true;
            } else {
                const std::string& bedMemory = actor->memories.Get("bed");
                if (!bedMemory.empty()) {
                    std::string coordsPart = bedMemory;
                    std::vector<std::string> tokens = String.Explode(bedMemory, ':');
                    if (tokens.size() >= 2) {
                        coordsPart = tokens[1];
                    }
                    
                    std::vector<std::string> coords = String.Explode(coordsPart, ',');
                    
                    if (coords.size() >= 3) {
                        bedPosition = glm::vec3(
                            String.ToFloat(coords[0]),
                            String.ToFloat(coords[1]),
                            String.ToFloat(coords[2])
                        );
                        hasBed = true;
                    }
                }
            }
            
            auto ApplySleepSuccess = [&]() {
                actor->state.mode = ActorState::Mode::Sleeping;
                actor->state.mIsWalking = false;
                actor->state.mIsRunning = false;
                actor->navigation.mVelocity *= glm::vec3(0.0f, 1.0f, 0.0f);
                actor->navigation.mTargetActor = nullptr;
                
                // Sleep recovery: clear fatigue and build comfort
                emotion.fatigue = 0.0f;
                actor->memories.ClearEmotion(TriggerType::Fatigue);
                actor->emotions.AddComfort(0.05f);
                
                // Entrain circadian rhythm back toward preferred cycle
                actor->sleep.Entrain(1.0f);
            };
            
            if (hasBed) {
                float distToBed = glm::distance(
                    glm::vec3(actor->navigation.mPosition.x, 0.0f, actor->navigation.mPosition.z),
                    glm::vec3(bedPosition.x, 0.0f, bedPosition.z)
                );
                
                if (distToBed <= actor->behavior.GetDistanceToInflict()) {
                    ApplySleepSuccess();
                    return true;
                } else {
                    actor->navigation.mTargetPoint = bedPosition;
                    actor->navigation.mTargetLook  = bedPosition;
                    actor->state.mIsFacing = true;
                    actor->state.mode = ActorState::Mode::WalkTo;
                    actor->navigation.mDistanceToTarget = distToBed;
                    return true;
                }
            } else {
                ApplySleepSuccess();
                return true;
            }
        } else {
            // Interrupted or unable to sleep: pushes circadian phase forward
            actor->sleep.ShiftRhythm(10.0f);
            emotion.fatigue = glm::clamp(emotion.fatigue + 0.01f, 0.0f, 1.0f);
            return false;
        }
    } else {
        // Wake up when the sleep cycle ends
        if (actor->state.mode == ActorState::Mode::Sleeping) {
            actor->state.mode = ActorState::Mode::Idle;
        }
    }
    // =========================================================================
    // SOCIALIZATION
    
    bool isSocialEligible = (emotion.social >= threshold && 
                            actor->counters.mSocialCoolDownCounter == 0 &&
                            emotion.stress < UniversalConst.emotionalThreshold && 
                            emotion.fear < UniversalConst.emotionalThreshold && 
                            emotion.anger < UniversalConst.emotionalThreshold);
    if (isSocialEligible && targetActor != nullptr) {
        // Verify target availability (Idle/Wandering OR social group size < 4)
        bool isTargetAvailable = false;
        if (targetActor->state.mode == ActorState::Mode::Idle || 
            targetActor->state.mode == ActorState::Mode::MoveRandom) {
            isTargetAvailable = true;
        } else if (targetActor->state.mode == ActorState::Mode::MoveSocialize) {
            if (GetSocialGroupSize(targetActor) < 4) {
                isTargetAvailable = true;
            }
        }
        
        if (isTargetAvailable && Random.Range(0.0f, 1.0f) < glm::pow(emotion.social, threshold)) {
            // Lock active actor into socializing toward targetActor
            actor->navigation.mTargetActor = targetActor;
            actor->state.mode = ActorState::Mode::MoveSocialize;
            
            glm::vec3 pos = actor->navigation.mPosition;
            glm::vec3 tarPos = targetActor->navigation.mPosition;
            float dist = glm::distance(glm::vec3(tarPos.x, 0.0f, tarPos.z), glm::vec3(pos.x, 0.0f, pos.z));
            actor->navigation.mDistanceToTarget = dist;
            
            // If target was unengaged, lock them back onto initiating actor
            if (targetActor->state.mode != ActorState::Mode::MoveSocialize) {
                targetActor->navigation.mTargetActor = actor;
                targetActor->state.mode = ActorState::Mode::MoveSocialize;
                targetActor->navigation.mDistanceToTarget = dist;
            }
            
            return true;
        }
    }
    
    // =========================================================
    // REPRODUCTION
    
    bool isStarving = (actor->biological.hunger > UniversalConst.biologicalThreshold && 
                    actor->biological.saturation < UniversalConst.biologicalThreshold);
    
    if (isStarving) {
        emotion.libido = 0.0f;
        actor->memories.ClearEmotion(TriggerType::Libido);
    }
    
    bool isBreedEligible = (!isStarving &&
                            emotion.libido >= threshold && 
                            actor->counters.mBreedingCoolDownCounter == 0 &&
                            emotion.fear < UniversalConst.emotionalThreshold &&
                            emotion.anger < UniversalConst.emotionalThreshold);
    
    if (isBreedEligible) {
        if (Random.Range(0.0f, 1.0f) < glm::pow(emotion.libido, threshold)) {
            Actor* viableMate = nullptr;
            
            // Evaluate candidate
            for (Actor* candidate : actor->mTargets) {
                if (!candidate || !candidate->isActive || candidate->isGarbage) continue;
                
                // Ensure prospective mate is also not starving
                bool mateStarving = (candidate->biological.hunger > UniversalConst.biologicalThreshold &&
                                    candidate->biological.saturation < UniversalConst.biologicalThreshold);
                if (mateStarving) continue;
                
                if (candidate->GetName() == actor->GetName() &&
                    candidate->state.mode != ActorState::Mode::MoveAttack &&
                    candidate->state.mode != ActorState::Mode::MoveFlee &&
                    candidate->physical.GetSexualOrientation() != actor->physical.GetSexualOrientation() &&
                    candidate->physical.mAge >= candidate->physical.mAgeAdult &&
                    candidate->physical.mAge < candidate->physical.mAgeSenior &&
                    candidate->counters.GetCoolDownBreeding() == 0 &&
                    candidate->counters.mBreedingCoolDownCounter == 0) {
                    viableMate = candidate;
                    break;
                }
            }
            
            if (viableMate != nullptr) {
                // Lock active actor onto viableMate
                actor->navigation.mTargetActor = viableMate;
                actor->state.mode = ActorState::Mode::MoveBreed;
                
                glm::vec3 pos = actor->navigation.mPosition;
                glm::vec3 tarPos = viableMate->navigation.mPosition;
                float dist = glm::distance(glm::vec3(tarPos.x, 0.0f, tarPos.z), glm::vec3(pos.x, 0.0f, pos.z));
                actor->navigation.mDistanceToTarget = dist;
                
                if (viableMate->state.mode != ActorState::Mode::MoveBreed) {
                    viableMate->navigation.mTargetActor = actor;
                    viableMate->state.mode = ActorState::Mode::MoveBreed;
                    viableMate->navigation.mDistanceToTarget = dist;
                }
                
                return true;
            } else {
                actor->counters.mBreedingCoolDownCounter = actor->behavior.mCooldownObserve;
            }
        }
    }
    
    // =========================================================================
    // PROFESSION / TRADE
    
    // Farming (Active during daylight hours: 6000 to 20000)
    if (mTimeOfDay >= 8000 && mTimeOfDay < 17000) {
        const std::string& trade = actor->memories.Get("trade");
        if (!trade.empty() && trade.rfind("farmer:", 0) == 0) {
            
            // Adult workers only
            if (actor->physical.mAge < actor->physical.mAgeAdult) 
                return false;
            
            // Finish planting action once the cooldown expires
            if (actor->state.mode == ActorState::Mode::MovePlanting) {
                if (actor->counters.mMovementCoolDownCounter == 0) {
                    actor->state.mode = ActorState::Mode::Idle;
                } else {
                    return true;
                }
            }
            
            std::vector<std::string> tradeTokens = String.Explode(trade, ':');
            
            if (tradeTokens.size() >= 2) {
                std::vector<std::string> coords = String.Explode(tradeTokens[1], ',');
                
                if (coords.size() == 3) {
                    glm::vec3 farmAnchor(
                        String.ToFloat(coords[0]),
                        String.ToFloat(coords[1]),
                        String.ToFloat(coords[2])
                    );
                    
                    float farmRadius = actor->behavior.GetDistanceToWalk() * 0.5f;
                    float distToFarm = glm::distance(
                        glm::vec3(actor->navigation.mPosition.x, 0.0f, actor->navigation.mPosition.z),
                        glm::vec3(farmAnchor.x, 0.0f, farmAnchor.z)
                    );
                    
                    // Match plantable inventory items (supporting wheat, crop, and seed classifications)
                    int plantItemIndex = -1;
                    int cropItemCount  = 0;
                    for (unsigned int i = 0; i < actor->inventory.itemClassList.size(); ++i) {
                        const std::string& itemStr = actor->inventory.itemClassList[i];
                        if (itemStr.find("crop") != std::string::npos || 
                            itemStr.find("wheat") != std::string::npos ||
                            itemStr.find("seed") != std::string::npos) {
                            if (plantItemIndex == -1) {
                                plantItemIndex = static_cast<int>(i);
                            }
                            cropItemCount++;
                        }
                    }
                    
                    // =========================================================
                    // Field Density Check
                    // Prevent planting if the farm already has a full yield
                    const int maxFieldCrops = 200;
                    int currentFieldCrops = 0;
                    
                    if (mWorldPickupQueryCallback != nullptr) {
                        std::vector<std::pair<std::string, glm::vec3>> staticCrops = 
                            mWorldPickupQueryCallback(farmAnchor, farmRadius);
                        for (const auto& crop : staticCrops) {
                            if (crop.first.find("crop") != std::string::npos) {
                                currentFieldCrops++;
                            }
                        }
                    }
                    
                    bool isFieldFull = (currentFieldCrops >= maxFieldCrops);
                    
                    // =========================================================
                    // Ground Pickup Gathering Routine
                    // Collect ripe drops if seed stock is depleted or the field is at capacity
                    if (mWorldPickupQueryCallback != nullptr && mWorldPickupRemoveCallback != nullptr) {
                        std::vector<std::pair<std::string, glm::vec3>> fieldPickups = 
                            mWorldPickupQueryCallback(farmAnchor, farmRadius);
                        
                        float closestPickupDist = 1e9f;
                        glm::vec3 closestPickupPos(0.0f);
                        bool foundPickup = false;
                        
                        for (const auto& pickup : fieldPickups) {
                            if (pickup.first.find("crop") != std::string::npos || 
                                pickup.first.find("wheat") != std::string::npos) {
                                
                                // Extract stackMax and item name from the pickup descriptor
                                unsigned int stackMax = 1;
                                std::string itemName = "";
                                std::vector<std::string> values = String.Explode(pickup.first, ';');
                                for (unsigned int v = 0; v < values.size(); ++v) {
                                    String.RemoveWhiteSpace(values[v]);
                                    std::vector<std::string> kvPair = String.Explode(values[v], ':');
                                    if (kvPair.size() >= 2) {
                                        if (kvPair[0] == "stackMax") {
                                            stackMax = String.ToUint(kvPair[1]);
                                        } else if (kvPair[0] == "name") {
                                            itemName = kvPair[1];
                                        }
                                    }
                                }
                                
                                // Count how many of this item the actor already possesses
                                unsigned int currentCount = 0;
                                for (const std::string& invItem : actor->inventory.itemClassList) {
                                    if (!itemName.empty()) {
                                        if (invItem.find(itemName) != std::string::npos) {
                                            currentCount++;
                                        }
                                    } else if (invItem == pickup.first) {
                                        currentCount++;
                                    }
                                }
                                
                                // Skip this ground drop if the actor is already at maximum capacity
                                if (currentCount >= stackMax) {
                                    continue;
                                }
                                
                                float d = glm::distance(
                                    glm::vec3(actor->navigation.mPosition.x, 0.0f, actor->navigation.mPosition.z),
                                    glm::vec3(pickup.second.x, 0.0f, pickup.second.z)
                                );
                                if (d < closestPickupDist) {
                                    closestPickupDist = d;
                                    closestPickupPos  = pickup.second;
                                    foundPickup       = true;
                                }
                            }
                        }
                    
                        if (foundPickup && (plantItemIndex == -1 || isFieldFull || cropItemCount < 3)) {
                            if (closestPickupDist <= actor->behavior.GetDistanceToInflict()) {
                                std::string collectedItem;
                                if (mWorldPickupRemoveCallback(closestPickupPos, collectedItem)) {
                                    actor->inventory.AddItem(collectedItem);
                                    actor->state.mode = ActorState::Mode::Idle;
                                    actor->counters.mMovementCoolDownCounter = actor->behavior.GetCooldownMove() / 2;
                                    return true;
                                }
                            } else {
                                actor->navigation.mTargetPoint = closestPickupPos;
                                actor->navigation.mTargetLook  = closestPickupPos;
                                actor->state.mIsFacing = true;
                                actor->state.mode = ActorState::Mode::WalkTo;
                                actor->navigation.mDistanceToTarget = closestPickupDist;
                                return true;
                            }
                        }
                    }
                    
                    // =========================================================
                    // Planting Routine
                    if (!isFieldFull && plantItemIndex != -1) {
                        
                        // Walk to the farm plot if currently outside its perimeter
                        if (distToFarm > farmRadius) {
                            actor->navigation.mTargetPoint = farmAnchor;
                            actor->navigation.mTargetLook  = farmAnchor;
                            actor->state.mIsFacing = true;
                            actor->state.mode = ActorState::Mode::WalkTo;
                            actor->navigation.mDistanceToTarget = distToFarm;
                            return true;
                        }
                        
                        const float minCropSpacing = 1.8f;
                        bool isCurrentSpotCrowded = false;
                        
                        if (mWorldStaticQueryCallback != nullptr) {
                            std::vector<std::pair<std::string, glm::vec3>> localCrops = 
                                mWorldStaticQueryCallback(actor->navigation.mPosition, minCropSpacing);
                            
                            for (const auto& crop : localCrops) {
                                if (crop.first.find("crop") != std::string::npos || 
                                    crop.first.find("wheat") != std::string::npos ||
                                    crop.first.find("plant") != std::string::npos) {
                                    isCurrentSpotCrowded = true;
                                    break;
                                }
                            }
                        }
                        
                        // Plant immediately if the current standing position is uncrowded
                        if (!isCurrentSpotCrowded && mWorldStaticPlaceCallback != nullptr) {
                            std::string rawItem = actor->inventory.itemClassList[plantItemIndex];
                            std::vector<std::string> tokens = String.Explode(rawItem, ';');
                            std::string plantItemClass = tokens[0];
                            if (plantItemClass.rfind("name:", 0) == 0) {
                                std::vector<std::string> kv = String.Explode(plantItemClass, ':');
                                if (kv.size() >= 2) plantItemClass = kv[1];
                            }
                            String.RemoveWhiteSpace(plantItemClass);
                            
                            glm::vec3 rotation(0.0f, Random.Range(0.0f, 360.0f), 0.0f);
                            glm::vec3 plantWorldPos = actor->navigation.mPosition;
                            
                            if (mWorldStaticPlaceCallback(plantItemClass, plantWorldPos, rotation)) {
                                actor->inventory.RemoveItem(static_cast<unsigned int>(plantItemIndex));
                                actor->state.mode = ActorState::Mode::MovePlanting;
                                actor->counters.mMovementCoolDownCounter = actor->behavior.GetCooldownMove();
                                return true;
                            }
                        }
                        
                        // If the current spot has a crop, locate an adjacent open position 2 to 4 units away
                        glm::vec3 nextPlantPos = farmAnchor;
                        bool foundOpenSpot = false;
                        
                        for (int attempt = 0; attempt < 8; ++attempt) {
                            float testAngle = Random.Range(0.0f, glm::two_pi<float>());
                            float testDist  = Random.Range(minCropSpacing + 0.4f, minCropSpacing + 2.5f);
                            glm::vec3 candidate(
                                actor->navigation.mPosition.x + std::cos(testAngle) * testDist,
                                farmAnchor.y,
                                actor->navigation.mPosition.z + std::sin(testAngle) * testDist
                            );
                            
                            float distCandidateToCenter = glm::distance(
                                glm::vec3(candidate.x, 0.0f, candidate.z),
                                glm::vec3(farmAnchor.x, 0.0f, farmAnchor.z)
                            );
                            if (distCandidateToCenter > farmRadius * 0.85f) {
                                continue;
                            }
                            
                            bool candidateCrowded = false;
                            if (mWorldStaticQueryCallback != nullptr) {
                                std::vector<std::pair<std::string, glm::vec3>> nearbyCrops = 
                                    mWorldStaticQueryCallback(candidate, minCropSpacing);
                                for (const auto& crop : nearbyCrops) {
                                    if (crop.first.find("crop") != std::string::npos || 
                                        crop.first.find("wheat") != std::string::npos ||
                                        crop.first.find("plant") != std::string::npos) {
                                        candidateCrowded = true;
                                        break;
                                    }
                                }
                            }
                            
                            if (!candidateCrowded) {
                                nextPlantPos = candidate;
                                foundOpenSpot = true;
                                break;
                            }
                        }
                        
                        // Fallback across the plot if adjacent spaces are occupied
                        if (!foundOpenSpot) {
                            float angle = Random.Range(0.0f, glm::two_pi<float>());
                            float radiusOffset = Random.Range(1.0f, farmRadius * 0.8f);
                            nextPlantPos = glm::vec3(
                                farmAnchor.x + std::cos(angle) * radiusOffset,
                                farmAnchor.y,
                                farmAnchor.z + std::sin(angle) * radiusOffset
                            );
                        }
                        
                        float distToTarget = glm::distance(
                            glm::vec3(actor->navigation.mPosition.x, 0.0f, actor->navigation.mPosition.z),
                            glm::vec3(nextPlantPos.x, 0.0f, nextPlantPos.z)
                        );
                        
                        actor->navigation.mTargetPoint = nextPlantPos;
                        actor->navigation.mTargetLook  = nextPlantPos;
                        actor->state.mIsFacing = true;
                        actor->state.mode = ActorState::Mode::WalkTo;
                        actor->navigation.mDistanceToTarget = distToTarget;
                        return true;
                    }
                    
                    // =========================================================
                    // Farm Patrol / Leisure Walk
                    if (actor->state.mode == ActorState::Mode::Idle || distToFarm > farmRadius || isFieldFull) {
                        float angle = Random.Range(0.0f, glm::two_pi<float>());
                        float radiusOffset = Random.Range(0.0f, farmRadius);
                        
                        actor->navigation.mTargetPoint.x = farmAnchor.x + std::cos(angle) * radiusOffset;
                        actor->navigation.mTargetPoint.y = farmAnchor.y;
                        actor->navigation.mTargetPoint.z = farmAnchor.z + std::sin(angle) * radiusOffset;
                        
                        actor->navigation.mTargetLook = actor->navigation.mTargetPoint;
                        actor->state.mIsFacing = true;
                        actor->state.mode = ActorState::Mode::WalkTo;
                        
                        actor->navigation.mDistanceToTarget = glm::distance(
                            glm::vec3(actor->navigation.mTargetPoint.x, 0.0f, actor->navigation.mTargetPoint.z),
                            glm::vec3(actor->navigation.mPosition.x, 0.0f, actor->navigation.mPosition.z)
                        );
                        
                        return true;
                    }
                }
            }
        }
    }
    
    // =========================================================================
    // WANDER / EXPLORATION
    
    if (emotion.curiosity >= threshold && actor->state.mode == ActorState::Mode::Idle) {
        if (Random.Range(0.0f, 1.0f) < glm::pow(emotion.curiosity, threshold)) {
            actor->state.mode = ActorState::Mode::MoveRandom;
            CalculateRandomLocalPoint(actor);
            actor->navigation.mDistanceToTarget = glm::distance(
                glm::vec3(actor->navigation.mTargetPoint.x, 0.0f, actor->navigation.mTargetPoint.z),
                glm::vec3(actor->navigation.mPosition.x, 0.0f, actor->navigation.mPosition.z)
            );
            return true;
        }
    }
    
    return false;
}
