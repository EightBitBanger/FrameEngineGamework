#ifndef __DEFINITION_TYPE_GRASS_
#define __DEFINITION_TYPE_GRASS_

class ENGINE_API DefinitionTypeGrass {
    
public:
    
    std::string name;
    
    float width;
    float height;
    
    Color colorMin;
    Color colorMax;
    
    DefinitionTypeGrass() : 
        name(""),
        width(0.0f),
        height(0.0f),
        colorMin(0.0, 0.0, 0.0),
        colorMax(0.0, 0.0, 0.0)
    {}
};

#endif
