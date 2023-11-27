#ifndef _GENETICS_NODE__
#define _GENETICS_NODE__


class __declspec(dllexport) Gene {
    
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