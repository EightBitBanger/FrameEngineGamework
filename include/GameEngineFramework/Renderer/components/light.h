#ifndef __COMPONENT_LIGHT
#define __COMPONENT_LIGHT

#include <GameEngineFramework/Engine/types/color.h>
#include <GameEngineFramework/Transform/Transform.h>


class ENGINE_API Light {
    
public:
    
    friend class RenderSystem;
    
    bool isActive;
    
    short int type;
    
    float renderDistance;
    
    glm::vec3 position;
    glm::vec3 offset;
    glm::vec3 direction;
    
    Color color;
    
    float intensity;
    float range;
    float attenuation;
    
    Light();
    
    
    
private:
    
    
    
    
    
    
};






#endif
