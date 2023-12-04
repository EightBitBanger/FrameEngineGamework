#ifndef _GENETICS_NODE__
#define _GENETICS_NODE__

#include <GameEngineFramework/configuration.h>


class ENGINE_API Gene {
    
public:
    
    bool isActive;  // Active state
    bool doExpress; // Update physical expression
    bool isLimb;    // Used as a limb
    
    float geneA[3]; // Offset
    float geneB[3]; // Position
    float geneC[3]; // Rotation
    float geneD[3]; // Scale
    float geneE[3]; // Color
    
    Gene();
    
private:
    
};




#endif
