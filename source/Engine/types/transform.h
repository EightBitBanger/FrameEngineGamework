#ifndef _TRANSFORM_TYPE__
#define _TRANSFORM_TYPE__

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>

class Transform {
    
public:
    
    /// Local translation
    glm::vec3 position;
    /// Local rotation
    glm::quat orientation;
    /// Local scale
    glm::vec3 scale;
    
    /// Local transform matrix
    glm::mat4 matrix;
    
    Transform();
    
    /// Update the model matrix from the current position, rotation and scale.
    void UpdateMatrix(void);
    
    /// Set the position of the transform matrix.
    void SetPosition(float x, float y, float z);
    /// Set the rotation of the transform matrix.
    void SetRotation(float w, float x, float y, float z);
    /// Set the scale of the transform matrix.
    void SetScale(float x, float y, float z);
    
    /// Set the position of the transform matrix.
    void SetPosition(glm::vec3 newPosition);
    /// Set the rotation of the transform matrix.
    void SetRotation(glm::quat newRotation);
    /// Set the scale of the transform matrix.
    void SetScale(glm::vec3 newScale);
    
    /// Get the position of the transform matrix.
    glm::vec3 GetPosition(void);
    /// Get the rotation of the transform matrix.
    glm::quat GetRotation(void);
    /// Get the scale of the transform matrix.
    glm::vec3 GetScale(void);
    
    /// Add a transform to this transform as a child transform.
    void ChildAdd(Transform* transform);
    /// Remove a child transform from this transform.
    bool ChildRemove(Transform* transform);
    
    /// Set the parent transform.
    void SetParentTransform(Transform* parentTransform);
    /// Return the parent transform pointer.
    Transform* GetParentTransform(void);
    
    
    /// Return an identity transform.
    Transform Identity(void);
    
    
private:
    
    // List of child transforms
    std::vector<Transform*> mChildList;
    
    // Parent transform
    Transform* mParent;
    
};


#endif
