#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>


bool RenderSystem::SortingPass(glm::vec3& eye, std::vector<MeshRenderer*>* renderQueueGroup, unsigned int queueGroupIndex) {
    
    std::vector<std::pair<float, MeshRenderer*>> sortList(1024);
    
    std::vector<MeshRenderer*> renderQueue = *renderQueueGroup;
    
    for (unsigned int i=0; i < renderQueue.size(); i++) {
        
        std::pair<float, MeshRenderer*> renderPair;
        
        renderPair.first = glm::distance( eye, renderQueue[i]->transform.position );
        renderPair.second = renderQueue[i];
        
        sortList.push_back( renderPair );
        
    }
    
    std::sort(sortList.begin(), sortList.end(), [](std::pair<float, MeshRenderer*> a, std::pair<float, MeshRenderer*> b) {
        return a.first < b.first;
    });
    
    for (unsigned int i=0; i < renderQueue.size(); i++) {
        renderQueue[i] = sortList[i].second;
    }
    
    return true;
}

