#include <iostream>
#include <string>

#include "../framework.h"
#include <GameEngineFramework/engine/types/transform.h>

void TestFramework::TestTransform(void) {
    if (hasTestFailed) return;
    
    std::cout << "Matrix transform........ ";
    
    Transform transform;
    
    // Test constructor
    if (transform.position    != glm::vec3(0, 0, 0))         Throw(msgFailedConstructor, __FILE__, __LINE__);
    if (transform.orientation != glm::identity<glm::quat>()) Throw(msgFailedConstructor, __FILE__, __LINE__);
    if (transform.scale       != glm::vec3(1, 1, 1))         Throw(msgFailedConstructor, __FILE__, __LINE__);
    
    // Test set/gets
    transform.SetPosition(10.0, 20.0, 30.0);
    transform.SetOrientation(90.0, 80.0, 70.0, 60.0);
    transform.SetScale(20.0, 30.0, 40.0);
    
    if (transform.GetPosition()    != glm::vec3(10.0, 20.0, 30.0))       Throw(msgFailedSetGet, __FILE__, __LINE__);
    if (transform.GetOrientation() != glm::quat(90.0, 80.0, 70.0, 60.0)) Throw(msgFailedSetGet, __FILE__, __LINE__);
    if (transform.GetScale()       != glm::vec3(20.0, 30.0, 40.0))       Throw(msgFailedSetGet, __FILE__, __LINE__);
    
    // Test identity
    transform.SetIdentity();
    if (transform.position    != glm::vec3(0, 0, 0))         Throw(msgFailedConstructor, __FILE__, __LINE__);
    if (transform.orientation != glm::identity<glm::quat>()) Throw(msgFailedConstructor, __FILE__, __LINE__);
    if (transform.scale       != glm::vec3(1, 1, 1))         Throw(msgFailedConstructor, __FILE__, __LINE__);
    
    // Test translation
    transform.SetIdentity();
    transform.Translate(10.0, 30.0, 50.0);
    if (transform.position != glm::vec3(10.0, 30.0, 50.0)) Throw(msgFailedSetGet, __FILE__, __LINE__);
    
    // Test rotation
    transform.SetIdentity();
    transform.RotateAxis(10.0, glm::vec3(0, 1, 0));
    
    glm::quat targetValue = glm::quat(0.996195, 0.0, 0.0871557, 0.0);
    float quatDiffX = transform.orientation.x - targetValue.x;
    float quatDiffY = transform.orientation.y - targetValue.y;
    float quatDiffZ = transform.orientation.z - targetValue.z;
    float quatDiffW = transform.orientation.w - targetValue.w;
    
    float margin = 0.0001;
    if ((quatDiffX > margin) | (quatDiffX < -margin)) Throw(msgFailedSetGet, __FILE__, __LINE__);
    if ((quatDiffY > margin) | (quatDiffY < -margin)) Throw(msgFailedSetGet, __FILE__, __LINE__);
    if ((quatDiffZ > margin) | (quatDiffZ < -margin)) Throw(msgFailedSetGet, __FILE__, __LINE__);
    if ((quatDiffW > margin) | (quatDiffW < -margin)) Throw(msgFailedSetGet, __FILE__, __LINE__);
    
    // Test scaling
    transform.SetIdentity();
    transform.Scale(20.0, 40.0, 60.0);
    if (transform.scale != glm::vec3(20.0, 40.0, 60.0)) Throw(msgFailedSetGet, __FILE__, __LINE__);
    
    // Test assignment operator
    Transform transformA;
    Transform transformB;
    Transform transformC;
    transformA.SetIdentity();
    transformB.SetIdentity();
    transformC.SetIdentity();
    
    transformB.position    = glm::vec3(10.0, 10.0, 10.0);
    transformB.orientation = glm::quat(2.0, 2.0, 2.0, 2.0);
    transformB.scale       = glm::vec3(10.0, 10.0, 10.0);
    
    transformA = transformB;
    
    if (transformA.position    != glm::vec3(10.0, 10.0, 10.0))   Throw(msgFailedOperator, __FILE__, __LINE__);
    if (transformA.orientation != glm::quat(2.0, 2.0, 2.0, 2.0)) Throw(msgFailedOperator, __FILE__, __LINE__);
    if (transformA.scale       != glm::vec3(10.0, 10.0, 10.0))   Throw(msgFailedOperator, __FILE__, __LINE__);
    
    // Test multiplication operator
    transformB.position    = glm::vec3(10.0, 10.0, 10.0);
    transformB.orientation = glm::quat(2.0, 2.0, 2.0, 2.0);
    transformB.scale       = glm::vec3(2.0, 2.0, 2.0);
    
    transformC = transformA * transformB;
    
    if (transformC.position    != glm::vec3(20.0, 20.0, 20.0))     Throw(msgFailedOperator, __FILE__, __LINE__);
    if (transformC.orientation != glm::quat(-8.0, 8.0, 8.0, 8.0))  Throw(msgFailedOperator, __FILE__, __LINE__);
    if (transformC.scale       != glm::vec3(20.0, 20.0, 20.0))     Throw(msgFailedOperator, __FILE__, __LINE__);
    
    // Test equivalence operators
    if (transformA == transformC) Throw(msgFailedOperator, __FILE__, __LINE__);
    transformA = transformC;
    if (transformA != transformC) Throw(msgFailedOperator, __FILE__, __LINE__);
    
    return;
}


