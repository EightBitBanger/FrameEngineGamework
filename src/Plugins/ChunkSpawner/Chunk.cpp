#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

Chunk::Chunk() : 
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
