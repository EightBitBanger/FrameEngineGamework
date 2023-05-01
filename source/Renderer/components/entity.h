#ifndef COMPONENT_ENTITY
#define COMPONENT_ENTITY

#define GLEW_STATIC
#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ReactPhysics3d/ReactPhysics3d.h>

#include "../../std/timer.h"

#include "../types/bufferlayout.h"
#include "../types/transform.h"
#include "../types/color.h"
#include "../types/viewport.h"

#include "../../Input/InputSystem.h"
#include "../../Physics/PhysicsSystem.h"
#include "../../Resources/assets/colliderTag.h"
#include "../../Scripting/components/script.h"

#include "shader.h"
#include "material.h"
#include "mesh.h"


extern PhysicsSystem Physics;


class Entity {
    
public:
    
    std::string name;
    
    Transform transform;
    
    Entity();
    ~Entity();
    
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
    Mesh*     mesh;
    /// Pointer to a material component.
    Material* material;
    
};


#endif
