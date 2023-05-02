#ifndef _TRANSFORM_TYPE__
#define _TRANSFORM_TYPE__

#include <vector>

#include <glm/glm.hpp>


class Transform {
    
public:
    
    glm::vec3 position;
    glm::vec4 rotation;
    glm::vec3 scale;
    
    Transform* parent;
    
    glm::mat4 matrix;
    
    std::vector<Transform*> child;
    
    Transform();
    
    /// Add a transform to this transform as a child transform.
    void ChildAdd(Transform* transform);
    /// Remove a child transform from this transform.
    bool ChildRemove(Transform* transform);
    
    /// Return an identity transform.
    Transform Identity(void);
    
};


#endif
