#include <Physics/components/meshcollider.h>

MeshCollider::MeshCollider() : 
    
    heightFieldShape(nullptr),
    heightMapBuffer(nullptr),
    
    concaveMeshShape(nullptr),
    
    triangleArray(nullptr),
    triangleMesh(nullptr)
{
    heightMapBuffer = new float[10 * 10];
}

MeshCollider::~MeshCollider() {
    
    delete heightMapBuffer;
    
}

