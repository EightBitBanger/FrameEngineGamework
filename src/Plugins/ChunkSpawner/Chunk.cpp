#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

Chunk::Chunk() : 
    isActive(false),
    isGenerated(false),
    isComplete(false),
    seed(0),
    x(0),
    y(0),
    fadeIn(0),
    gameObject(nullptr),
    staticObject(nullptr),
    rigidBody(nullptr),
    bodyCollider(nullptr),
    meshCollider(nullptr),
    heightField(nullptr),
    colorField(nullptr)
{
}
