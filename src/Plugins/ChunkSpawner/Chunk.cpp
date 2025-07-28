#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

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
