#ifndef __CLASS_DEFINITION_TYPE_
#define __CLASS_DEFINITION_TYPE_

class ENGINE_API ClassDefinition {
public:
    unsigned int id;
    
    std::string mesh;
    
    float width;
    float height;
    
    Color colorMin;
    Color colorMax;
    
    // Inventory
    unsigned int stackMax;       // Maximum stack depth
    std::string bitmap;          // Inventory item image
    
    unsigned int alignment;      // How the object is placed
    
    ClassDefinition() : 
        id(0),
        mesh(""),
        width(0.0f),
        height(0.0f),
        colorMin(0.0, 0.0, 0.0),
        colorMax(0.0, 0.0, 0.0),
        stackMax(1),
        bitmap("")
    {}
};

#endif
