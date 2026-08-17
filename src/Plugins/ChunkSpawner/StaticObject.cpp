#include <GameEngineFramework/Plugins/ChunkSpawner/StaticObject.h>

StaticObject::StaticObject() : 
    position(glm::vec3(0)),
    rotation(glm::vec3(0)),
    scale(glm::vec3(1)),
    color(glm::vec3(0)),
    mesh(0),
    type(0),
    function(0) {}
