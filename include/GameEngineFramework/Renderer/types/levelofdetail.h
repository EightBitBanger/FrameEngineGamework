#ifndef _LEVEL_OF_DETAIL__
#define _LEVEL_OF_DETAIL__

#include <GameEngineFramework/Renderer/components/mesh.h>

class LevelOfDetail {
    
public:
    
    LevelOfDetail() : 
       offset(glm::vec3(0)),
       mesh(nullptr),
       distance(0)
    {}
    
    /// Position offset for this level of detail.
    glm::vec3 offset;
    
    /// Reference to the level of detail mesh.
    Mesh* mesh;
    
    /// Distance to the camera to transition 
    /// to this level of detail.
    float distance;
    
};

#endif
