#ifndef __CLASS_DEFINITION_ACTOR_TYPE_
#define __CLASS_DEFINITION_ACTOR_TYPE_

class ENGINE_API ClassActor {
public:
    
    std::string genome;
    
    Color colorVariant;
    
    ClassActor() : 
        genome(""),
        colorVariant(0.0, 0.0, 0.0) {}
};

#endif
