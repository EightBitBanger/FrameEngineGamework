#include <GameEngineFramework/Physics/components/meshcollider.h>

MeshCollider::MeshCollider() : 
    
    heightFieldShape(nullptr),
    heightMapBuffer(nullptr),
    
    triangleMesh(nullptr),
    vertexBuffer(nullptr),
    indexBuffer(nullptr) {}

MeshCollider::~MeshCollider() {}
