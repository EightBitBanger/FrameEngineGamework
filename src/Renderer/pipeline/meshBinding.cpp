#include <GameEngineFramework/Renderer/RenderSystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/Types.h>


bool RenderSystem::BindMesh(Mesh* meshPtr) {
    
    if (mCurrentMesh == meshPtr) 
        return false;
    
    mCurrentMesh = meshPtr;
    mCurrentMesh->Bind();
    return true;
}

