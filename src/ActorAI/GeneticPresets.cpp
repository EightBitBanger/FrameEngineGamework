#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/ActorAI/ActorSystemInternal.h>

Gene GeneticPresets::GeneticBuilder::BuildAddLeg(float xx, float yy, float zz, float length, float width, Color color, bool invert, bool side, float canter, float tilt, float offset) {
    Gene gene;
    gene.position = Codon(0, offset, 0);
    zz -= offset;
    
    gene.offset = Codon(-xx, yy, zz);
    gene.rotation.x -= canter * 0.1f;
    
    if (side) {
        gene.rotation.y = -canter;
        gene.rotation.z = tilt;
    } else {
        gene.rotation.y = canter;
        gene.rotation.z = -tilt;
    }
    
    gene.scale   = Codon(width, length, width);
    gene.color.x = color.r;
    gene.color.y = color.g;
    gene.color.z = color.b;
    gene.animationType = ActorState::Animation::Limb;
    gene.animationAxis = Codon(3.5f, 0, 0);
    gene.animationRange = 15.0f;
    gene.doInverseAnimation = invert;
    return gene;
}
