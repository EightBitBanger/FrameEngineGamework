#include <GameEngineFramework/Transform/Transform.h>


Transform::Transform() : 
    position(glm::vec3(0, 0, 0)),
    rotation(glm::identity<glm::quat>()),
    scale(glm::vec3(1, 1, 1)),
    
    localPosition(glm::vec3(0, 0, 0)),
    localRotation(glm::identity<glm::quat>()),
    localScale(glm::vec3(1, 1, 1)),
    
    matrix(glm::mat4(1)),
    
    parent(nullptr),
    
    inheritParentRotation(true),
    inheritParentScale(true)
{
}

void Transform::operator= (const Transform& transform) {
    this->position  = transform.position;
    this->rotation  = transform.rotation;
    this->scale     = transform.scale;
    this->matrix    = transform.matrix;
    return;
}

Transform Transform::operator* (const Transform& transform) {
    Transform result;
    result.position  = this->position  + transform.position;
    result.rotation  = this->rotation  * transform.rotation;
    result.scale     = this->scale     * transform.scale;
    result.matrix    = this->matrix    * transform.matrix;
    return result;
}

bool Transform::operator== (const Transform& transform) {
    return ((this->position == transform.position) && (this->rotation == transform.rotation));
}

bool Transform::operator!= (const Transform& transform) {
    return ((this->position != transform.position) || (this->rotation != transform.rotation));
}

void Transform::SetPosition(float x, float y, float z) {
    position = glm::vec3(x, y, z);
    return;
}

void Transform::SetPosition(glm::vec3 newPosition) {
    position = glm::vec3(newPosition.x, newPosition.y, newPosition.z);
    return;
}

void Transform::SetOrientation(float w, float x, float y, float z) {
    rotation = glm::quat(w, x, y, z);
    return;
}

void Transform::SetOrientation(glm::quat newRotation) {
    rotation = newRotation;
    return;
}

void Transform::SetScale(float x, float y, float z) {
    scale = glm::vec3(x, y, z);
    return;
}

void Transform::SetScale(glm::vec3 newScale) {
    scale = glm::vec3(newScale.x, newScale.y, newScale.z);
    return;
}

glm::vec3 Transform::GetPosition(void) {
    return position;
}

glm::quat Transform::GetOrientation(void) {
    return rotation;
}

glm::vec3 Transform::GetScale(void){
    return scale;
}

void Transform::Translate(glm::vec3 translation) {
    matrix = glm::translate(matrix, translation);
    position += translation;
    return;
}

void Transform::Translate(float x, float y, float z) {
    matrix = glm::translate( matrix, glm::vec3(x, y, z) );
    position += glm::vec3(x, y, z);
    return;
}

void Transform::RotateAxis(float angle, glm::vec3 axis) {
    matrix = glm::rotate( matrix, glm::radians(angle), glm::normalize(axis) );
    rotation = glm::quat_cast( matrix );
    return;
}

void Transform::RotateWorldAxis(float angle, glm::vec3 axis, glm::vec3 worldPosition) {
    matrix = glm::rotate( matrix, glm::radians(angle), glm::normalize(axis) );
    matrix = glm::translate( matrix, worldPosition );
    rotation = glm::quat_cast( matrix );
    return;
}

void Transform::RotateEuler(glm::vec3 eulerAngle) {
    RotateEuler( eulerAngle.x, eulerAngle.y, eulerAngle.z );
    return;
}

void Transform::RotateEuler(float yaw, float pitch, float roll) {
    rotation *= glm::quat( glm::radians( glm::vec3(yaw, pitch, roll) ) );
    matrix *= glm::toMat4( rotation );
    return;
}

void Transform::Scale(glm::vec3 scaler) {
    Scale( scaler.x, scaler.y, scaler.z );
    return;
}

void Transform::Scale(float x, float y, float z) {
    matrix = glm::scale(matrix, glm::vec3(x, y, z) );
    scale *= glm::vec3(x, y, z);
    return;
}

glm::vec3 Transform::EulerAngles(void) {
    return glm::degrees( glm::eulerAngles( rotation ) );
}

void Transform::SetIdentity(void) {
    position   = glm::vec3(0, 0, 0);
    rotation   = glm::identity<glm::quat>();
    scale      = glm::vec3(1, 1, 1);
    matrix     = glm::mat4(1);
    return;
}

void Transform::UpdateMatrix(void) {
    matrix = glm::translate(glm::mat4(1), glm::vec3( position.x, position.y, position.z )) * 
             glm::toMat4(rotation) * 
             glm::scale(glm::mat4(1), glm::vec3( scale.x, scale.y, scale.z ));
    return;
}
