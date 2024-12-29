#ifndef __PARTICLE_SYSTEM_
#define __PARTICLE_SYSTEM_

#include <GameEngineFramework/Plugins/ParticleSystem/ParticleSystem.h>
#include <GameEngineFramework/Plugins/ParticleSystem/Emitter.h>

#include <GameEngineFramework/Engine/Engine.h>


class ENGINE_API ParticleSystem {
    
public:
    
    ParticleSystem();
    
    void Initiate(void);
    
    void Update(void);
    
    
    Emitter* CreateEmitter(void);
    
    void DestroyEmitter(Emitter* emitterPtr);
    
    
private:
    
    PoolAllocator<Emitter> mEmitters;
    
};


#endif
