#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>


bool RenderSystem::BindMesh(Mesh* meshPtr) {
    
    if (mCurrentMesh == meshPtr) 
        return false;
    
    mCurrentMesh = meshPtr;
    
    mCurrentMesh->Bind();
    
    return true;
}

