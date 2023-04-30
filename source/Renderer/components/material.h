#ifndef COMPONENT_MATERIAL
#define COMPONENT_MATERIAL

#include <SDKDDKVer.h>
#include <windows.h>

#define GLEW_STATIC
#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../types/color.h"
#include "../enumerators.h"


class Material {
    
public:
    
    unsigned int width;
    unsigned int height;
    
    Color color;
    
    bool doDepthTest;
    bool doBlending;
    bool doFaceCulling;
    
    GLint depthFunc;
    GLint faceWinding;
    GLint faceCullSide;
    GLint blendSource;
    GLint blendDestination;
    GLint blendAlphaSource;
    GLint blendAlphaDestination;
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
    
private:
    
    unsigned int textureBuffer;
    
};



#endif
