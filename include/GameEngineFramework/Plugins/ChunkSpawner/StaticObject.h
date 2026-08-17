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

// Structure for saving static elements to file
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
