#ifndef __COMPONENT_ENTITY
#define __COMPONENT_ENTITY

#include "../../engine/types/transform.h"

#include "material.h"
#include "mesh.h"


class Entity {
    
public:
    
    /// Transformation matrix.
    Transform transform;
    
    Entity();
    
    /// Attach a mesh object to the entity.
    void AttachMesh(Mesh* meshPtr);
    /// Attach the mesh object from the entity.
    void DetachMesh(void);
    /// Return the attached mesh object.
    Mesh* GetAttachedMesh(void);
    
    /// Attach a material object to the entity.
    void AttachMaterial(Material* materialPtr);
    /// Attach the material object from the entity.
    void DetachMaterial(void);
    /// Return the attached material object.
    Material* GetAttachedMaterial(void);
    
    
private:
    
    /// Pointer to a mesh component.
    Mesh*     mMesh;
    /// Pointer to a material component.
    Material* mMaterial;
    
};


#endif
