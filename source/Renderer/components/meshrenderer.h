#ifndef __COMPONENT_ENTITY
#define __COMPONENT_ENTITY

#include "../../engine/types/transform.h"

#include "material.h"
#include "mesh.h"


class MeshRenderer {
    
public:
    
    /// Pointer to the mesh component.
    Mesh*     mesh;
    /// Pointer to the material component.
    Material* material;
    
    /// Transformation matrix.
    Transform transform;
    
    MeshRenderer();
    
};


#endif
