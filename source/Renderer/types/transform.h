#ifndef _TRANSFORM_TYPE__
#define _TRANSFORM_TYPE__

#include <vector>

#include <glm/glm.hpp>


class Transform {
    
public:
    
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    
    Transform* parent;
    
    std::vector<Transform*> child;
    
    Transform();
    
    void ChildAdd(Transform* transform);
    
    bool ChildRemove(Transform* transform);
    
    Transform Identity(void);
    
};


#endif
