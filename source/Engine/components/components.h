#ifndef ENGINE_COMPONENT
#define ENGINE_COMPONENT

#define COMPONENT_TYPE_UNDEFINED      0
#define COMPONENT_TYPE_RENDERER       100
#define COMPONENT_TYPE_RIGIDBODY      101
#define COMPONENT_TYPE_SCRIPT         102


class Component {
    
public:
    
    /// Component name.
    std::string name;
    
    /// Component type identifier.
    unsigned int type;
    
    Component();
    
private:
    
    // Component object pointer
    void* attachedType;
    
};




#endif
