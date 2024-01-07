#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>


void RenderSystem::BindMaterial(Material* materialPtr) {
    
    if (mCurrentMaterial == materialPtr) 
        return;
    
    mCurrentMaterial = materialPtr;
    
    mCurrentMaterial->Bind();
    mCurrentMaterial->BindTextureSlot(0);
    
    // Depth testing
    
    if (mCurrentMaterial->doDepthTest) {
        
        glEnable(GL_DEPTH_TEST);
        
        glDepthMask(mCurrentMaterial->doDepthTest);
        
        glDepthFunc(mCurrentMaterial->depthFunc);
        
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnRender::Material::DepthTest::");
#endif
        
    } else {
        
        glDisable(GL_DEPTH_TEST);
        
    }
    
    // Face culling and winding
    
    if (mCurrentMaterial->doFaceCulling) {
        
        glEnable(GL_CULL_FACE);
        
        glCullFace(mCurrentMaterial->faceCullSide);
        
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnRender::Material::Culling::");
#endif
        
    } else {
        
        glDisable(GL_CULL_FACE);
        
    }
    
    // Face winding order
    glFrontFace(mCurrentMaterial->faceWinding);
    
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnRender::Material::FaceWinding::");
#endif
    
    // Blending
    
    if (mCurrentMaterial->doBlending) {
        
        glEnable(GL_BLEND);
        
        glBlendFuncSeparate(mCurrentMaterial->blendSource,
                            mCurrentMaterial->blendDestination,
                            mCurrentMaterial->blendAlphaSource,
                            mCurrentMaterial->blendAlphaDestination);
        
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnRender::Material::Blending::");
#endif
        
    } else {
        
        glDisable(GL_BLEND);
        
    }
    
    return;
}

