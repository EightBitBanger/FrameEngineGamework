#ifndef __DEFINITION_TYPE_TREE_
#define __DEFINITION_TYPE_TREE_

class ENGINE_API DefinitionTypeTree {
    
public:
    
    std::string name;
    
    float heightMin;
    float heightMax;
    
    unsigned int leafCountMin;
    unsigned int leafCountMax;
    
    float leafSpreadArea;
    float leafSpreadHeight;
    float leafWidth;
    float leafHeight;
    float trunkSize;
    
    Color trunkColorMin;
    Color trunkColorMax;
    
    Color leafColorMin;
    Color leafColorMax;
    
    std::string leafType;
    
    DefinitionTypeTree() : 
        name(""),
        heightMin(0.0f),
        heightMax(0.0f),
        
        leafCountMin(0.0f),
        leafCountMax(0.0f),
        
        leafSpreadArea(0.0f),
        leafSpreadHeight(0.0f),
        leafWidth(0.0f),
        leafHeight(0.0f),
        leafColorMin(0.0, 0.0, 0.0),
        leafColorMax(0.0, 0.0, 0.0),
        
        trunkSize(0.0f),
        trunkColorMin(0.0, 0.0, 0.0),
        trunkColorMax(0.0, 0.0, 0.0)
    {}
};

#endif
