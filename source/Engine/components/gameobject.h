#ifndef ENGINE_GAME_OBJECT
#define ENGINE_GAME_OBJECT

#include "../../Renderer/components/entity.h"


class GameObject {
    
public:
    
    GameObject();
    
    
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
    
    /// Attach a script object to the entity.
    void AttachScript(Script* scriptPtr);
    /// Attach the script object from the entity.
    void DetachScript(void);
    /// Return the attached script object.
    Script* GetAttachedScript(void);
    
    /// Attach a rigid body object to the entity.
    void AttachRidigBody(rp3d::RigidBody* rigidBodyPtr);
    /// Attach the rigid body object from the entity.
    void DetachRidigBody(void);
    /// Return the attached rigid body object.
    rp3d::RigidBody* GetAttachedRidigBody();
    
    
private:
    
    Mesh*            mesh;
    Material*        material;
    Script*          script;
    rp3d::RigidBody* rigidBody;
    
};

#endif
