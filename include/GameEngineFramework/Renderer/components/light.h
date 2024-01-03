#ifndef __COMPONENT_LIGHT
#define __COMPONENT_LIGHT

#include <GameEngineFramework/Engine/types/color.h>
#include <GameEngineFramework/Transform/Transform.h>


class ENGINE_API Light {
    
public:
    
    /// State whether the light is active.
    bool isActive;
    
    /// Type of light. Point or directional.
    short int type;
    
    /// Distance beyond which the light will no longer be rendered.
    float renderDistance;
    
    /// Position of the light in the scene.
    glm::vec3 position;
    
    /// Offset from the position where the light will be rendered.
    glm::vec3 offset;
    
    /// Direction toward which the light will shine. 
    glm::vec3 direction;
    
    /// Color of the light.
    Color color;
    
    /// Brightness multiplier.
    float intensity;
    
    /// Distance from the position beyond which the light will be cut off.
    float range;
    
    /// Lighting drop off multiplier.
    float attenuation;
    
    
    friend class RenderSystem;
    
    Light();
    
};






#endif
