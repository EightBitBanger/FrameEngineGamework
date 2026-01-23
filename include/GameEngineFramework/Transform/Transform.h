#ifndef _TRANSFORM_TYPE__
#define _TRANSFORM_TYPE__

#include <GameEngineFramework/configuration.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtx/quaternion.hpp>
#include <glm/ext/quaternion_transform.hpp>

#include <vector>
#include <mutex>


class ENGINE_API Transform {
    
public:
    
    glm::vec3 position;          // Position in world space.
    glm::quat rotation;          // Rotation in world space.
    glm::vec3 scale;             // Scale in world space.
    
    glm::quat localRotation;     // Local rotation relative to the parent rotation.
    glm::vec3 localPosition;     // Local translation relative to the parent translation.
    glm::vec3 localScale;        // Local scale relative to the parent scale.
    
    glm::mat4 matrix;            // Transform matrix
    
    Transform* parent;           // Pointer to a parent transform.
    
    bool inheritParentRotation;  // Should the rotation inherit from the parent transform
    bool inheritParentScale;     // Should the scale inherit from the parent transform
    
    Transform();
    Transform(glm::vec3 init_position);
    
    /// Assignment
    void operator= (const Transform& transform);
    /// Evaluation equal
    bool operator== (const Transform& transform);
    /// Evaluation not equal
    bool operator!= (const Transform& transform);
    /// Multiply two transforms into a resulting transform.
    Transform operator* (const Transform& transform);
    
    /// Set the position of the transform matrix.
    void SetPosition(float x, float y, float z);
    /// Set the rotation of the transform matrix.
    void SetOrientation(float w, float x, float y, float z);
    /// Set the scale of the transform matrix.
    void SetScale(float x, float y, float z);
    
    /// Set the position of the transform matrix.
    void SetPosition(glm::vec3 newPosition);
    /// Set the rotation of the transform matrix.
    void SetOrientation(glm::quat newRotation);
    /// Set the scale of the transform matrix.
    void SetScale(glm::vec3 newScale);
    
    /// Get the position of the transform matrix.
    glm::vec3 GetPosition(void);
    /// Get the rotation of the transform matrix.
    glm::quat GetOrientation(void);
    /// Get the scale of the transform matrix.
    glm::vec3 GetScale(void);
    
    /// Translate the transform by the given amount.
    void Translate(float x, float y, float z);
    /// Translate the transform by the given amount.
    void Translate(glm::vec3 translation);
    
    /// Rotate the transform on the axis by the given angle amount.
    void RotateAxis(float angle, glm::vec3 axis);
    /// Rotate the transform by the given Euler angles.
    void RotateEuler(float yaw, float pitch, float roll);
    /// Rotate the transform by the given Euler angles.
    void RotateEuler(glm::vec3 eulerAngle);
    /// Rotate and translate the transform by the given amounts.
    void RotateWorldAxis(float angle, glm::vec3 axis, glm::vec3 worldPosition);
    
    /// Scale the transform by the given amount.
    void Scale(float x, float y, float z);
    /// Scale the transform by the given amount.
    void Scale(glm::vec3 eulerAngle);
    
    /// Get the rotation in euler angles.
    glm::vec3 EulerAngles(void);
    
    /// Return an identity transform.
    void SetIdentity(void);
    
    /// Update the model matrix from the current position, rotation and scale.
    void UpdateMatrix(void);
    
};


#endif
