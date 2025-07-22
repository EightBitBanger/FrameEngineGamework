#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

StaticObject::StaticObject() : 
    position(glm::vec3(0)),
    rotation(glm::vec3(0)),
    scale(glm::vec3(1)),
    color(glm::vec3(0)),
    collisionBody(nullptr)
{
}

Chunk::Chunk() : 
    isActive(false),
    seed(0),
    x(0),
    y(0),
    fadeIn(0),
    gameObject(nullptr),
    staticObject(nullptr),
    rigidBody(nullptr),
    bodyCollider(nullptr),
    meshCollider(nullptr)
{
}
