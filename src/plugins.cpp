#include <GameEngineFramework/Plugins/plugins.h>

enum class PluginComponent {
    Particle
};

void* buildParticle() {return (void*)Particle.CreateEmitter();}
void destructParticle(void* ptr) {Particle.DestroyEmitter((Emitter*)ptr);}
void updateParticle(unsigned int index) {Particle.Update();}

void RegisterPlugins(void) {
    
    Engine.RegisterPlugin<Emitter>((unsigned int)PluginComponent::Particle, "Particle", &buildParticle, &destructParticle, &updateParticle, ComponentUpdateType::PerObject);
    
}

