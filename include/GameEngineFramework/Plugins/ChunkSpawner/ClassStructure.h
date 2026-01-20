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
    
    class SubStructurePlace {
    public:
        std::string name;
        glm::vec3 position;
        
        SubStructurePlace() : 
            name(""),
            position({0.0f}) {}
    };
    
    class SubStructureFill {
    public:
        std::string name;
        glm::vec3 from;
        glm::vec3 to;
        
        SubStructureFill() : 
            name(""),
            from({0.0f}),
            to({0.0f}) {}
    };
    
    class SubStructurePattern {
    public:
        std::string name;
        std::string mesh;
        std::string pattern;
        glm::vec3 position;
        
        SubStructurePattern() : 
            name(""),
            mesh(""),
            pattern(""),
            position({0.0f}) {}
    };
    
    std::vector<SubStructureStack>    stacks;
    std::vector<SubStructurePlace>    places;
    std::vector<SubStructureFill>     fills;
    std::vector<SubStructurePattern>  patterns;
    
    ClassStructure() {}
};

#endif
