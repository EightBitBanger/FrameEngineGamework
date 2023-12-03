#include <ActorAI/components/neuron.h>

Neuron::Neuron() {
    
    for (unsigned int i=0; i < 3; i++) in    [i] = 0.0f;
    for (unsigned int i=0; i < 4; i++) layerA[i] = 0.0f;
    for (unsigned int i=0; i < 4; i++) layerB[i] = 0.0f;
    for (unsigned int i=0; i < 2; i++) out   [i] = 0.0f;
    
    return;
}
