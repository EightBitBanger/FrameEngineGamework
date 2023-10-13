#ifndef __COMPONENT_ENTITY
#define __COMPONENT_ENTITY

#include "../../engine/types/transform.h"

#include "material.h"
#include "mesh.h"


class MeshRenderer {
    
public:
    
    /// Transformation matrix.
    Transform transform;
    
    MeshRenderer();
    
    /// Attach a mesh object to the mesh renderer.
    void SetMesh(Mesh* meshPtr);
    /// Return the attached mesh object.
    Mesh* GetMesh(void);
    
    /// Attach a material object to the mesh renderer.
    void SetMaterial(Material* materialPtr);
    /// Return the attached material object.
    Material* GetMaterial(void);
    
    
private:
    
    /// Pointer to a mesh component.
    Mesh*     mMesh;
    /// Pointer to a material component.
    Material* mMaterial;
    
};


#endif
