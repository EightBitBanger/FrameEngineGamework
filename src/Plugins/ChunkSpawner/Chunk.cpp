#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

StaticObject::StaticObject() : 
    x(0),
    y(0),
    z(0),
    
    yaw(0),
    pitch(0),
    
    red(0),
    green(0),
    blue(0),
    
    type(0),
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
