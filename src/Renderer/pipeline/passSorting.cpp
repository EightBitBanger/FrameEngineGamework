#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>


bool RenderSystem::SortingPass(glm::vec3& eye, std::vector<MeshRenderer*>* renderQueueGroup) {
    
    std::vector< std::pair<float, MeshRenderer*> > sortList;
    sortList.reserve(renderQueueGroup->size());
    
    // Accumulate renderer list
    std::transform(renderQueueGroup->begin(), renderQueueGroup->end(), std::back_inserter(sortList),
                   [&eye](MeshRenderer* renderer) {
                       std::lock_guard<std::mutex>(renderer->mux);
                       return std::make_pair(glm::distance(eye, renderer->transform.position), renderer);
                   });
    
    std::sort(sortList.begin(), sortList.end(), [](const std::pair<float, MeshRenderer*>& a, const std::pair<float, MeshRenderer*>& b) {
        return a.first < b.first;
    });
    
    renderQueueGroup->clear();
    renderQueueGroup->reserve(sortList.size());
    
    // Replace renderers with sorted renderers
    std::transform(sortList.begin(), sortList.end(), std::back_inserter(*renderQueueGroup),
                   [](const std::pair<float, MeshRenderer*>& pair) {
                       return pair.second;
                   });
    
    return true;
}

