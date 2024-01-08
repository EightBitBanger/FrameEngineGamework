#ifndef __COMPONENT_MATERIAL
#define __COMPONENT_MATERIAL

#include <GameEngineFramework/Engine/types/color.h>

#include <GameEngineFramework/Renderer/enumerators.h>
#include <GameEngineFramework/Renderer/components/shader.h>


class ENGINE_API Material {
    
public:
    
    /// Texture image width.
    unsigned int width;
    
    /// Texture image height.
    unsigned int height;
    
    
    /// Ambient material color.
    Color ambient;
    
    /// Diffuse material color.
    Color diffuse;
    
    /// Specular material color.
    Color specular;
    
    
    /// Should depth testing be preformed during rendering.
    bool doDepthTest;
    
    /// Should this material be alpha blended with the scene.
    bool doBlending;
    
    /// Should back faces be rendered.
    bool doFaceCulling;
    
    /// Should a shadow be rendered on this material.
    bool doShadowPass;
    
    
    /// Shadow render distance.
    float shadowDistance;
    
    /// Shadow stencil cast length.
    float shadowStencilLength;
    
    
    /// Texture interpolation
    int textureFiltration;
    
    
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
    
    
    /// Shader used by this material for rendering.
    Shader* shader;
    
    
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
    
    
    /// Get texture filtration.
    int GetTextureFiltration(void);
    
    /// Set texture filtration.
    void SetTextureFiltration(int filtration);
    
    
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
    
    
    /// Bind the material texture for rendering.
    void Bind(void);
    
    /// Bind the texture slot for textured rendering.
    void BindTextureSlot(unsigned int slot);
    
    /// Reupload the texture buffer onto the GPU.
    void UpdateTextureBuffer(void* textureData);
    
    /// Generate mipmaps for the current texture.
    void GenerateMipMaps(void);
    
    
    friend class RenderSystem;
    
    Material();
    ~Material();
    
    
private:
    
    unsigned int mTextureBuffer;
    
};



#endif
