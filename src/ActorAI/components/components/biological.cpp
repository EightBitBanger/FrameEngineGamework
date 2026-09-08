#include <GameEngineFramework/ActorAI/ActorSystem.h>

BiologicalSystem::BiologicalSystem() : 
    health(0),
    healthMax(0),
    hunger(0),
    saturation(0),
    strength(0),
    defense(0),
    mHungerCounter(0)
{}

void BiologicalSystem::Reset() {
    health          = 10.0f;
    healthMax       = 10.0f;
    hunger          = 0.0f;
    saturation      = 0.0f;
    defense         = 0.0f;
    strength        = 1.0f;
    
    mHungerCounter  = 0;
    
    mBiologics.clear();
}
