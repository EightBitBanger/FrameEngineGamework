#include "meshcollider.h"

MeshCollider::MeshCollider() : 
    
    concaveMeshShape(nullptr),
    heightFieldShape(nullptr),
    
    triangleArray(nullptr),
    triangleMesh(nullptr)
{
    heightMapBuffer = new float[10 * 10];
}

MeshCollider::~MeshCollider() {
    
    delete heightMapBuffer;
    
}

