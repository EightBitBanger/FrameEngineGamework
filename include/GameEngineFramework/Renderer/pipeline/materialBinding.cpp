#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>


bool RenderSystem::BindMaterial(Material* materialPtr) {
    
    if (mCurrentMaterial == materialPtr) 
        return false;
    
    mCurrentMaterial = materialPtr;
    
    mCurrentMaterial->texture.Bind();
    mCurrentMaterial->texture.BindTextureSlot(0);
    
    // Depth testing
    
    if (mCurrentMaterial->mDoDepthTest) {
        
        glEnable(GL_DEPTH_TEST);
        
        glDepthMask(mCurrentMaterial->mDoDepthTest);
        
        glDepthFunc(mCurrentMaterial->mDepthFunc);
        
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnRender::Material::DepthTest::");
#endif
        
    } else {
        
        glDisable(GL_DEPTH_TEST);
        
    }
    
    // Face culling and winding
    
    if (mCurrentMaterial->mDoFaceCulling) {
        
        glEnable(GL_CULL_FACE);
        
        glCullFace(mCurrentMaterial->mFaceCullSide);
        
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnRender::Material::Culling::");
#endif
        
    } else {
        
        glDisable(GL_CULL_FACE);
        
    }
    
    // Face winding order
    glFrontFace(mCurrentMaterial->mFaceWinding);
    
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnRender::Material::FaceWinding::");
#endif
    
    // Blending
    
    if (mCurrentMaterial->mDoBlending) {
        
        glEnable(GL_BLEND);
        
        glBlendFuncSeparate(mCurrentMaterial->mBlendSource,
                            mCurrentMaterial->mBlendDestination,
                            mCurrentMaterial->mBlendAlphaSource,
                            mCurrentMaterial->mBlendAlphaDestination);
        
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnRender::Material::Blending::");
#endif
        
    } else {
        
        glDisable(GL_BLEND);
        
    }
    
    return true;
}

