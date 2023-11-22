#include "Genetics.h"

Gene::Gene() : 
    isActive(false),
    doExpress(false),
    isLimb(false)
{
    for (int i=0; i < 3; i++) {
        geneA[i] = 0.0f;
        geneB[i] = 0.0f;
        geneC[i] = 0.0f;
        geneD[i] = 0.0f;
        geneE[i] = 0.0f;
    }
    
}

