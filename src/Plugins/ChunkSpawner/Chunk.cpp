#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

Chunk::Chunk() : 
    flags(0x00),
    //isActive(false),
    isGenerated(false),
    isComplete(false),
    doUpdate(false),
    seed(0),
    x(0),
    y(0),
    fadeIn(0),
    gameObject(nullptr),
    staticObject(nullptr),
    pickupObject(nullptr),
    rigidBody(nullptr),
    bodyCollider(nullptr),
    meshCollider(nullptr),
    heightField(nullptr),
    colorField(nullptr),
    colorAdditive(nullptr)
{}
