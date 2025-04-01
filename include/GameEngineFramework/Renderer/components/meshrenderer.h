#ifndef __COMPONENT_ENTITY
#define __COMPONENT_ENTITY

#include <GameEngineFramework/Transform/Transform.h>

#include <GameEngineFramework/Renderer/components/material.h>
#include <GameEngineFramework/Renderer/components/mesh.h>



class ENGINE_API MeshRenderer {
    
public:
    
    /// Should this renderer be included in the scene.
    bool isActive;
    
    /// Pointer to the mesh component.
    Mesh* mesh;
    
    /// Pointer to the material component.
    Material* material;
    
    /// Transformation element.
    Transform transform;
    
    /// Level of detail distance to switch models
    float distance;
    
    /// Levels of detail.
    std::vector<Mesh*> lods;
    
    /// Enable culling for this entity
    void EnableFrustumCulling(void);
    
    /// Disable culling for this entity
    void DisableFrustumCulling(void);
    
    /// Set the frustum culling bounding box min corner.
    void SetBoundingBoxMin(glm::vec3 min);
    
    /// Set the frustum culling bounding box max corner.
    void SetBoundingBoxMax(glm::vec3 max);
    
    /// Get the frustum culling bounding box min corner.
    glm::vec3 GetBoundingBoxMin(void);
    
    /// Get the frustum culling bounding box max corner.
    glm::vec3 GetBoundingBoxMax(void);
    
    MeshRenderer();
    
private:
    
    // Is this renderer being culled
    bool mDoCulling;
    
    // Bounding box area
    glm::vec3 mBoundingBoxMin;
    glm::vec3 mBoundingBoxMax;
    
    friend class RenderSystem;
    
};

#endif
