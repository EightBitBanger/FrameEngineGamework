#include <GameEngineFramework/Transform/Transform.h>


Transform::Transform() : 
    position(glm::vec3(0.0f, 0.0f, 0.0f)),
    rotation(glm::identity<glm::quat>()),
    scale(glm::vec3(1.0f, 1.0f, 1.0f)),
    
    localPosition(glm::vec3(0.0f, 0.0f, 0.0f)),
    localRotation(glm::identity<glm::quat>()),
    localScale(glm::vec3(1.0f, 1.0f, 1.0f)),
    
    matrix(glm::mat4(1.0f)),
    
    parent(nullptr),
    
    inheritParentRotation(true),
    inheritParentScale(true) {}

Transform::Transform(glm::vec3 init_position) : 
    position(init_position),
    rotation(glm::identity<glm::quat>()),
    scale(glm::vec3(1.0f, 1.0f, 1.0f)),
    
    localPosition(glm::vec3(0.0f, 0.0f, 0.0f)),
    localRotation(glm::identity<glm::quat>()),
    localScale(glm::vec3(1.0f, 1.0f, 1.0f)),
    
    matrix(glm::mat4(1.0f)),
    
    parent(nullptr),
    
    inheritParentRotation(true),
    inheritParentScale(true) {}

void Transform::operator=(const Transform& transform) {
    this->position  = transform.position;
    this->rotation  = transform.rotation;
    this->scale     = transform.scale;
    this->matrix    = transform.matrix;
}

Transform Transform::operator*(const Transform& transform) {
    Transform result;
    result.position  = this->position  + transform.position;
    result.rotation  = this->rotation  * transform.rotation;
    result.scale     = this->scale     * transform.scale;
    result.matrix    = this->matrix    * transform.matrix;
    return result;
}

bool Transform::operator==(const Transform& transform) {
    return ((this->position == transform.position) && (this->rotation == transform.rotation));
}

bool Transform::operator!=(const Transform& transform) {
    return ((this->position != transform.position) || (this->rotation != transform.rotation));
}

void Transform::SetPosition(float x, float y, float z) {
    position = glm::vec3(x, y, z);
}

void Transform::SetPosition(glm::vec3 newPosition) {
    position = newPosition;
}

void Transform::SetOrientation(float w, float x, float y, float z) {
    rotation = glm::quat(w, x, y, z);
}

void Transform::SetOrientation(glm::quat newRotation) {
    rotation = newRotation;
}

void Transform::SetScale(float x, float y, float z) {
    scale = glm::vec3(x, y, z);
}

void Transform::SetScale(glm::vec3 newScale) {
    scale = newScale;
}

glm::vec3 Transform::GetPosition(void) {
    return position;
}

glm::quat Transform::GetOrientation(void) {
    return rotation;
}

glm::vec3 Transform::GetScale(void) {
    return scale;
}

void Transform::Translate(glm::vec3 translation) {
    matrix = glm::translate(matrix, translation);
    position += translation;
}

void Transform::Translate(float x, float y, float z) {
    matrix = glm::translate(matrix, glm::vec3(x, y, z));
    position += glm::vec3(x, y, z);
}

void Transform::RotateAxis(float angle, glm::vec3 axis) {
    matrix = glm::rotate(matrix, glm::radians(angle), glm::normalize(axis));
    rotation = glm::quat_cast(matrix);
}

void Transform::RotateWorldAxis(float angle, glm::vec3 axis, glm::vec3 worldPosition) {
    matrix = glm::rotate(matrix, glm::radians(angle), glm::normalize(axis));
    matrix = glm::translate(matrix, worldPosition);
    rotation = glm::quat_cast(matrix);
}

void Transform::RotateEuler(glm::vec3 eulerAngle) {
    RotateEuler(eulerAngle.x, eulerAngle.y, eulerAngle.z);
}

void Transform::RotateEuler(float yaw, float pitch, float roll) {
    rotation *= glm::quat(glm::radians(glm::vec3(yaw, pitch, roll)));
    matrix *= glm::toMat4(rotation);
}

void Transform::Scale(glm::vec3 scaler) {
    Scale(scaler.x, scaler.y, scaler.z);
}

void Transform::Scale(float x, float y, float z) {
    matrix = glm::scale(matrix, glm::vec3(x, y, z));
    scale *= glm::vec3(x, y, z);
}

glm::vec3 Transform::EulerAngles(void) {
    return glm::degrees(glm::eulerAngles(rotation));
}

void Transform::SetIdentity(void) {
    position = glm::vec3(0, 0, 0);
    rotation = glm::identity<glm::quat>();
    scale    = glm::vec3(1, 1, 1);
    matrix   = glm::mat4(1);
}

void Transform::UpdateMatrix(void) {
    matrix = glm::translate(glm::mat4(1), position) * 
             glm::toMat4(rotation) * 
             glm::scale(glm::mat4(1), scale);
}
