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
    
    
    /// Ambient color for this material.
    Color ambient;
    
    /// Diffuse color for this material.
    Color diffuse;
    
    /// Specular color for this material.
    Color specular;
    
    
    /// Should depth testing be preformed during rendering.
    bool doDepthTest;
    
    /// Should this material be alpha blended with the scene.
    bool doBlending;
    
    /// Should the back side of faces be rendered.
    bool doFaceCulling;
    
    /// Should a shadow pass be rendered with this material.
    bool doShadowPass;
    
    
    /// Shadow stencil cast length.
    float shadowStencilLength;
    
    /// Shadow stencil intensity low.
    float shadowStencilIntensityLow;
    
    /// Shadow stencil intensity high.
    float shadowStencilIntensityHigh;
    
    /// Shadow stencil color intensity multiplier.
    float shadowStencilColorIntensity;
    
    /// Shadow stencil visible angle.
    float shadowStencilAngleOfView;
    
    /// Shadow stencil color.
    Color shadowStencilColor;
    
    
    /// Depth function used for rendering with the depth buffer.
    int depthFunc;
    
    /// The direction in which the triangles will be rendered.
    int faceWinding;
    
    /// The side of the triangle to skip drawing.
    int faceCullSide;
    
    
    /// Blending source.
    int blendSource;
    
    /// Blending destination.
    int blendDestination;
    
    /// The alpha blend source.
    int blendAlphaSource;
    
    /// The alpha blend destination.
    int blendAlphaDestination;
    
    /// The function used to blend colors.
    int blendFunction;
    
    
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
    
    
    /// Enable the shadow pass.
    void EnableShadowPass(void);
    
    /// Disable the shadow pass.
    void DisableShadowPass(void);
    
    /// Set the length of the shadow stencil.
    void SetShadowStencilLength(float length);
    
    /// Set the intensity of the alpha blend for the shadow stencil at the tail end of the stencil trail.
    void SetShadowStencilIntensityLow(float intensity);
    
    /// Set the intensity of the alpha blend for the shadow stencil at the head end of the stencil trail.
    void SetShadowStencilIntensityHigh(float intensity);
    
    /// Set the intensity of the color for the shadow stencil.
    void SetShadowStencilColorIntensity(float intensity);
    
    /// Set the visible angle to render the shadow stencil.
    void SetShadowStencilAngleOfView(float angle);
    
    /// Set the color modifier of the shadow stencil.
    void SetShadowStencilColor(Color color);
    
    
    friend class RenderSystem;
    
    Material();
    
};



#endif
