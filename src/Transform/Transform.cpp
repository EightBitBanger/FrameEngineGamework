#include <GameEngineFramework/Transform/Transform.h>


Transform::Transform() : 
    
    matrix(glm::mat4(1)) 
{
    
    SetIdentity();
}

void Transform::operator= (const Transform& transform) {
    this->position    = transform.position;
    this->orientation = transform.orientation;
    this->scale       = transform.scale;
    this->matrix      = transform.matrix;
}

Transform Transform::operator* (const Transform& transform) {
    Transform result;
    result.position    = this->position    + transform.position;
    result.orientation = this->orientation * transform.orientation;
    result.scale       = this->scale       * transform.scale;
    result.matrix      = this->matrix      * transform.matrix;
    return result;
}

bool Transform::operator== (const Transform& transform) {
    return ((this->position == transform.position) && (this->orientation == transform.orientation));
}

bool Transform::operator!= (const Transform& transform) {
    return ((this->position != transform.position) || (this->orientation != transform.orientation));
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
    orientation = glm::quat(w, x, y, z);
    return;
}

void Transform::SetOrientation(glm::quat newRotation) {
    orientation = newRotation;
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
    return orientation;
}

glm::vec3 Transform::GetScale(void){
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
    orientation = glm::quat_cast(matrix);
}

void Transform::RotateWorldAxis(float angle, glm::vec3 axis, glm::vec3 worldPosition) {
    matrix = glm::rotate(matrix, glm::radians(angle), glm::normalize(axis));
    matrix = glm::translate(matrix, worldPosition);
    orientation = glm::quat_cast(matrix);
}

void Transform::RotateEuler(glm::vec3 eulerAngle) {
    orientation *= glm::quat(glm::radians(eulerAngle));
    matrix *= glm::toMat4(orientation);
}

void Transform::RotateEuler(float yaw, float pitch, float roll) {
    orientation *= glm::quat( glm::radians( glm::vec3(yaw, pitch, roll) ) );
    matrix *= glm::toMat4(orientation);
}

void Transform::Scale(glm::vec3 scaler) {
    matrix = glm::scale(matrix, scaler);
    scale *= scaler;
}

void Transform::Scale(float x, float y, float z) {
    matrix = glm::scale(matrix, glm::vec3(x, y, z) );
    scale *= glm::vec3(x, y, z);
}

glm::vec3 Transform::EulerAngles(void) {
    return glm::degrees( glm::eulerAngles(orientation) );
}

void Transform::SetIdentity(void) {
    position     = glm::vec3(0, 0, 0);
    orientation  = glm::identity<glm::quat>();
    scale        = glm::vec3(1, 1, 1);
    matrix       = glm::mat4(1);
}

void Transform::UpdateMatrix(void) {
    
    glm::mat4 modelTranslation = glm::translate(glm::mat4(1), glm::vec3( position.x, position.y, position.z ));
    glm::mat4 modelRotation    = glm::toMat4(orientation);
    glm::mat4 modelScale       = glm::scale(glm::mat4(1), glm::vec3( scale.x, scale.y, scale.z ));
    
    matrix = modelTranslation * modelRotation * modelScale;
}
