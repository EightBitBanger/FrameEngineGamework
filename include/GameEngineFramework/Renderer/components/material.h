#ifndef __COMPONENT_MATERIAL
#define __COMPONENT_MATERIAL

#include <GameEngineFramework/Engine/types/color.h>

#include <GameEngineFramework/Renderer/enumerators.h>
#include <GameEngineFramework/Renderer/components/shader.h>
#include <GameEngineFramework/Renderer/components/texture.h>


class ENGINE_API Material {
    
public:
    
    /// Texture associated with this material.
    Texture texture;
    
    /// Shader used by this material for rendering.
    Shader* shader;
    
    /// Is this material shared between multiple mesh renderers. (True to prevent garbage collection)
    bool isShared;
    
    
    /// Ambient color for this material.
    Color ambient;
    
    /// Diffuse color for this material.
    Color diffuse;
    
    /// Specular color for this material.
    Color specular;
    
    
    /// Enable depth testing.
    void EnableDepthTest(void);
    
    /// Disable depth testing.
    void DisableDepthTest(void);
    
    /// Set the depth testing function.
    void SetDepthFunction(int func);
    
    
    /// Enable face culling.
    void EnableCulling(void);
    
    /// Disable face culling.
    void DisableCulling(void);
    
    /// Set the side which will be culled during rendering.
    void SetFaceCullingSide(int side);
    
    /// Set the winding direction for drawing triangles.
    void SetFaceWindingOrder(int direction);
    
    
    /// Enable blending.
    void EnableBlending(void);
    
    /// Disable blending.
    void DisableBlending(void);
    
    /// Set blending source and destination.
    void SetBlending(int src, int dest);
    
    /// Set blending source and destination with alpha.
    void SetBlendingAlpha(int srca, int desta);
    
    /// Set blending function.
    void SetBlendingFunction(int func);
    
    
    /// Enable the shadow volume pass.
    void EnableShadowVolumePass(void);
    
    /// Disable the shadow volume pass.
    void DisableShadowVolumePass(void);
    
    /// Set the length of the shadow volume.
    void SetShadowVolumeLength(float length);
    
    /// Set the intensity of the alpha blending for the shadow volume at the tail end of the volume trail.
    void SetShadowVolumeIntensityLow(float intensity);
    
    /// Set the intensity of the alpha blend for the shadow volume at the head end of the volume trail.
    void SetShadowVolumeIntensityHigh(float intensity);
    
    /// Set the intensity of the color for the shadow volume.
    void SetShadowVolumeColorIntensity(float intensity);
    
    /// Set the visible angle within which the shadow volume will be rendered.
    void SetShadowVolumeAngleOfView(float angle);
    
    /// Set the color modifier of the shadow volume.
    void SetShadowVolumeColor(Color color);
    
    
    friend class RenderSystem;
    
    Material();
    
    
private:
    
    // Should depth testing be preformed during rendering.
    bool mDoDepthTest;
    
    // Should this material be alpha blended with the scene.
    bool mDoBlending;
    
    // Should the back side of faces be rendered.
    bool mDoFaceCulling;
    
    // Should a shadow pass be rendered with this material.
    bool mDoShadowPass;
    
    
    // Shadow stencil cast length.
    float mShadowVolumeLength;
    
    // Shadow stencil intensity low.
    float mShadowVolumeIntensityLow;
    
    // Shadow stencil intensity high.
    float mShadowVolumeIntensityHigh;
    
    // Shadow stencil color intensity multiplier.
    float mShadowVolumeColorIntensity;
    
    // Shadow stencil visible angle.
    float mShadowVolumeAngleOfView;
    
    // Shadow stencil color.
    Color mShadowVolumeColor;
    
    
    // Depth function used for rendering with the depth buffer.
    int mDepthFunc;
    
    // The direction in which the triangles will be rendered.
    int mFaceWinding;
    
    // The side of the triangle to skip drawing.
    int mFaceCullSide;
    
    
    // Blending source.
    int mBlendSource;
    
    // Blending destination.
    int mBlendDestination;
    
    // The alpha blend source.
    int mBlendAlphaSource;
    
    // The alpha blend destination.
    int mBlendAlphaDestination;
    
    // The function used to blend colors.
    int mBlendFunction;
    
};



#endif
