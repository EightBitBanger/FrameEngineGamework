#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>


void RenderSystem::BindMesh(Mesh* meshPtr) {
    
    if (mCurrentMesh == meshPtr) 
        return;
    
    mCurrentMesh = meshPtr;
    
    mCurrentMesh->Bind();
    
    return;
}

