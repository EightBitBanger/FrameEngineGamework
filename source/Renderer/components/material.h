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

    void EnableDepthTest(void);
    void DisableDepthTest(void);
    void SetDepthFunction(GLint func);
    
    void EnableCulling(void);
    void DisableCulling(void);
    void SetFaceCullingSide(GLint side);
    void SetFaceWindingOrder(GLint direction);
    
    void EnableBlending(void);
    void DisableBlending(void);
    void SetBlending(GLint src, GLint dest);
    void SetBlendingAlpha(GLint srca, GLint desta);
    void SetBlendingFunction(GLint func);
    
    void Bind(void);
    void BindTextureSlot(unsigned int slot);
    void UpdateTextureBuffer(void* textureData);
    
private:
    
    unsigned int textureBuffer;
    
};



#endif
