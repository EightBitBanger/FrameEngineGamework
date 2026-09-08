#ifndef _STATIC_OBJECT__
#define _STATIC_OBJECT__

#include <GameEngineFramework/Engine/Engine.h>

class ENGINE_API StaticAnimation {
public:
    size_t staticIndex;      // Index into the chunk->statics vector
};

class ENGINE_API StaticObject {
public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 color;
    unsigned int mesh;
    unsigned int type;
    short function;
    
    StaticObject();
};

class ENGINE_API StaticPickup {
public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    std::string classification;
    
    MeshRenderer* renderer;
    
    unsigned int subMeshStartIndex;
    unsigned int subMeshCount;
    
    StaticPickup();
};

struct StaticElement {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 color;
    unsigned int mesh;
    unsigned int type;
    short function;
};

#endif
