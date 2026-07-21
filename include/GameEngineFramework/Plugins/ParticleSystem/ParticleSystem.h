#ifndef __PARTICLE_SYSTEM_
#define __PARTICLE_SYSTEM_

#include <GameEngineFramework/Plugins/ParticleSystem/ParticleSystem.h>
#include <GameEngineFramework/Plugins/ParticleSystem/Emitter.h>

#include <GameEngineFramework/Engine/Engine.h>


class ENGINE_API ParticleSystem {
    
public:
    
    void Initiate(void);
    void Update(void);
    
    /// Create a new particle emitter and return its pointer.
    Emitter* CreateEmitter(void);
    
    /// Destroy an emitter object.
    void DestroyEmitter(Emitter* emitterPtr);
    
    ParticleSystem();
    
private:
    
    PoolAllocator<Emitter> mEmitters;
};


#endif
