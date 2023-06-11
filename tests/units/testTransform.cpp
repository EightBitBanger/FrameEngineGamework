#include <iostream>
#include <string>

#include "../framework.h"
#include "../../source/engine/types/transform.h"

void TestFramework::TestTransform(void) {
    if (hasTestFailed) return;
    
    std::cout << "Matrix transform........ ";
    
    Transform transform;
    
    // Test constructor
    if (transform.position    != glm::vec3(0, 0, 0))    Throw(msgFailedConstructor, __FILE__, __LINE__);
    if (transform.orientation != glm::quat(0, 0, 0, 1)) Throw(msgFailedConstructor, __FILE__, __LINE__);
    if (transform.scale       != glm::vec3(1, 1, 1))    Throw(msgFailedConstructor, __FILE__, __LINE__);
    
    // Test set/gets
    transform.SetPosition(10.0, 20.0, 30.0);
    transform.SetOrientation(90.0, 80.0, 70.0, 60.0);
    transform.SetScale(20.0, 30.0, 40.0);
    
    if (transform.GetPosition()    != glm::vec3(10.0, 20.0, 30.0))       Throw(msgFailedSetGet, __FILE__, __LINE__);
    if (transform.GetOrientation() != glm::quat(90.0, 80.0, 70.0, 60.0)) Throw(msgFailedSetGet, __FILE__, __LINE__);
    if (transform.GetScale()       != glm::vec3(20.0, 30.0, 40.0))       Throw(msgFailedSetGet, __FILE__, __LINE__);
    
    return;
}

