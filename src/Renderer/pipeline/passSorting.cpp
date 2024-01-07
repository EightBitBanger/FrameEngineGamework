#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>


void RenderSystem::SortingPass(glm::vec3& eye, std::vector<MeshRenderer*>* renderQueueGroup, unsigned int queueGroupIndex) {
    
    // Synchronize the sorting list
    if (mRenderQueueSorter[queueGroupIndex].size() != renderQueueGroup->size()) {
        
        mRenderQueueSorter[queueGroupIndex].clear();
        
        for (unsigned int i=0; i < renderQueueGroup->size(); i++) {
            
            MeshRenderer* meshRenderer = *(renderQueueGroup->data() + i);
            
            std::pair<float, MeshRenderer*> distancePair;
            
            distancePair.first  = meshRenderer->distance;
            distancePair.second = meshRenderer;
            
            mRenderQueueSorter[queueGroupIndex].push_back( distancePair );
            
            continue;
        }
        
    }
    
    std::sort(mRenderQueueSorter[queueGroupIndex].begin(), 
              mRenderQueueSorter[queueGroupIndex].end(), 
              [](std::pair<float, MeshRenderer*> a, std::pair<float, MeshRenderer*> b) {
        return a.first > b.first;
    });
    
    
    // Restore the sorted renderers
    for (unsigned int i=0; i < renderQueueGroup->size(); i++) {
        
        MeshRenderer* meshRenderer = *(renderQueueGroup->data() + i);
        
        // Calculate object distance
        meshRenderer->distance = glm::distance( eye, meshRenderer->transform.position );
        mRenderQueueSorter[queueGroupIndex][i].first = meshRenderer->distance;
        
        *(renderQueueGroup->data() + i) = mRenderQueueSorter[queueGroupIndex][i].second;
        
        continue;
    }
    
    return;
}

