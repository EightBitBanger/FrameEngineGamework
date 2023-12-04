#ifndef __COMPONENT_ENTITY
#define __COMPONENT_ENTITY

#include "../../engine/types/transform.h"

#include "material.h"
#include "mesh.h"


class ENGINE_API MeshRenderer {
    
public:
    
    friend class RenderSystem;
    
    /// Pointer to the mesh component.
    Mesh*     mesh;
    /// Pointer to the material component.
    Material* material;
    
    /// Transformation element.
    Transform transform;
    
    MeshRenderer();
    
};


#endif
