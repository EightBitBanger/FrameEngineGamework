#include <GameEngineFramework/Physics/components/meshcollider.h>

MeshCollider::MeshCollider() : 
    
    heightFieldShape(nullptr),
    
    heightMapBuffer(nullptr)
{
}

MeshCollider::~MeshCollider() {
    
}
