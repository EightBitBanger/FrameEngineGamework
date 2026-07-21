#include <GameEngineFramework/Physics/components/meshcollider.h>

MeshCollider::MeshCollider() : 
    heightFieldShape(nullptr),
    heightMapBuffer(nullptr),
    triangleMesh(nullptr),
    triangleArray(nullptr),
    
    concaveShape(nullptr),
    vertexBuffer(nullptr),
    indexBuffer(nullptr) 
{}

MeshCollider::~MeshCollider() {}
