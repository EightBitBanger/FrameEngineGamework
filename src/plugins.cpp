#include <GameEngineFramework/Plugins/plugins.h>

void* buildParticle() {return (void*)Particle.CreateEmitter();}
void destructParticle(void* ptr) {Particle.DestroyEmitter((Emitter*)ptr);}
void updateParticle(unsigned int index) {Particle.Update();}

void* buildVehicle() {return (void*)Vehicular.CreateVehicle();}
void destructVehicle(void* ptr) {Vehicular.DestroyVehicle((Vehicle*)ptr);}
void updateVehicle(unsigned int index) {Vehicular.Update(1.0f / PHYSICS_UPDATES_PER_SECOND);}

void RegisterPlugins(void) {
    
    Engine.RegisterPlugin<Emitter>("Particle", &buildParticle, &destructParticle, &updateParticle, ComponentUpdateType::PerFrame);
    
    Engine.RegisterPlugin<Vehicle>("Vehicle", &buildVehicle, &destructVehicle, &updateVehicle, ComponentUpdateType::PerFrame);
    
}
