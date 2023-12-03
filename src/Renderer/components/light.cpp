#include <Renderer/components/light.h>


Light::Light() : 
    isActive(true),
    renderDistance(30.0),
    
    position(glm::vec3(0)),
    direction(glm::vec3(0)),
    
    intensity(100),
    range(300),
    attenuation(0.008)
{
    color = Color(1, 1, 1);
}

