#ifndef __COMPONENT_MATERIAL
#define __COMPONENT_MATERIAL

#define GLEW_STATIC
#include <gl/glew.h>

#include "shader.h"
#include "../../engine/types/color.h"
#include "../enumerators.h"


class Material {
    
public:
    
    /// Texture image width.
    unsigned int width;
    /// Texture image height.
    unsigned int height;
    
    /// Global illumination value.
    Color ambient;
    /// The color with which the texture will be rendered.
    Color diffuse;
    
    /// State whether depth testing will be used.
    bool doDepthTest;
    /// State whether blending will be used.
    bool doBlending;
    /// State whether face culling will be used.
    bool doFaceCulling;
    
    /// Texture interpolation
    int textureFiltration;
    
    /// Depth function used for rendering with the depth buffer.
    GLint depthFunc;
    /// The direction in which the triangles will be rendered.
    GLint faceWinding;
    /// The side of the triangle to skip drawing.
    GLint faceCullSide;
    
    /// Blending source.
    GLint blendSource;
    /// Blending destination.
    GLint blendDestination;
    /// The alpha blend source.
    GLint blendAlphaSource;
    /// The alpha blend destination.
    GLint blendAlphaDestination;
    /// The function used to blend colors.
    GLint blendFunction;
    
    Material();
    ~Material();
    
    /// Enable depth testing.
    void EnableDepthTest(void);
    /// Disable depth testing.
    void DisableDepthTest(void);
    /// Set the depth testing function.
    void SetDepthFunction(GLint func);
    
    /// Enable face culling.
    void EnableCulling(void);
    /// Disable face culling.
    void DisableCulling(void);
    /// Set the size which will be culled during rendering.
    void SetFaceCullingSide(GLint side);
    /// Set the winding direction for drawing triangles.
    void SetFaceWindingOrder(GLint direction);
    
    /// Enable blending.
    void EnableBlending(void);
    /// Disable blending.
    void DisableBlending(void);
    /// Set blending source and destination.
    void SetBlending(GLint src, GLint dest);
    /// Set blending source and destination with alpha.
    void SetBlendingAlpha(GLint srca, GLint desta);
    /// Set blending function.
    void SetBlendingFunction(GLint func);
    
    /// Bind the material texture for rendering.
    void Bind(void);
    /// Bind the texture slot for textured rendering.
    void BindTextureSlot(unsigned int slot);
    /// Reupload the texture buffer onto the GPU.
    void UpdateTextureBuffer(void* textureData);
    
    /// Set the current shader for this material.
    Shader* GetShader(void);
    /// Set the current shader for this material.
    void SetShader(Shader* shader);
    
private:
    
    unsigned int mTextureBuffer;
    
    Shader* shader;
    
};



#endif
