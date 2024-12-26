#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

StaticObject::StaticObject() : 
    x(0),
    y(0),
    z(0),
    r(0),
    g(0),
    b(0),
    
    type(0),
    rigidBody(nullptr)
{
}

Chunk::Chunk() : 
    isActive(false),
    seed(0),
    x(0),
    y(0),
    gameObject(nullptr),
    staticObject(nullptr),
    waterObject(nullptr),
    rigidBody(nullptr),
    bodyCollider(nullptr),
    meshCollider(nullptr)
{
}
