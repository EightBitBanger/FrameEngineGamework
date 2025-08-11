#include <GameEngineFramework/Plugins/ChunkSpawner/staticobject.h>

StaticObject::StaticObject() : 
    position(glm::vec3(0)),
    rotation(glm::vec3(0)),
    scale(glm::vec3(1)),
    color(glm::vec3(0)),
    type(0)
{
}
