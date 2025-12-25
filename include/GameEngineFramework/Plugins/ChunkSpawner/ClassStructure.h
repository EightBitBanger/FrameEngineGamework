#ifndef __CLASS_STRUCTURE_
#define __CLASS_STRUCTURE_

class ENGINE_API ClassStructure {
public:
    
    class SubStructureStack {
    public:
        std::string name;
        unsigned int heightMin;
        unsigned int heightMax;
        glm::vec3 position;
        
        SubStructureStack() : 
            name(""),
            heightMin(0),
            heightMax(0),
            position({0.0f}) {}
    };
    
    class SubStructureSpread {
    public:
        std::string name;
        std::string mesh;
        std::string pattern;
        glm::vec3 position;
        
        SubStructureSpread() : 
            name(""),
            mesh(""),
            pattern(""),
            position({0.0f}) {}
    };
    
    std::vector<SubStructureStack>  stacks;
    std::vector<SubStructureSpread> spread;
    
    ClassStructure() {}
};

#endif
