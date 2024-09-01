//
// Update actor components

#include <GameEngineFramework/Engine/Engine.h>

ENGINE_API extern EngineComponents  Components;
ENGINE_API extern ColorPreset       Colors;
ENGINE_API extern NumberGeneration  Random;
ENGINE_API extern StringType        String;
ENGINE_API extern Logger            Log;
ENGINE_API extern Timer             PhysicsTime;
ENGINE_API extern Timer             Time;

ENGINE_API extern Serialization     Serializer;
ENGINE_API extern ResourceManager   Resources;
ENGINE_API extern ScriptSystem      Scripting;
ENGINE_API extern RenderSystem      Renderer;
ENGINE_API extern PhysicsSystem     Physics;
ENGINE_API extern AudioSystem       Audio;
ENGINE_API extern InputSystem       Input;
ENGINE_API extern MathCore          Math;
ENGINE_API extern ActorSystem       AI;

ENGINE_API extern PlatformLayer     Platform;


void EngineSystemManager::UpdateActorGenetics(unsigned int index) {
    
    if (!mStreamBuffer[index].actor->mDoUpdateGenetics) 
        return;
    
    int numberOfGenes = mStreamBuffer[index].actor->GetNumberOfGenes();
    
    //
    // Destroy and clear old genetic renderers
    
    mStreamBuffer[index].actor->mIsActorActiveInScene = false;
    
    for (unsigned int a=0; a < mStreamBuffer[index].actor->mGeneticRenderers.size(); a++) {
        
        MeshRenderer* geneRenderer = mStreamBuffer[index].actor->mGeneticRenderers[a];
        
        // Destroy old material
        if (geneRenderer->material != nullptr) 
            Renderer.DestroyMaterial(geneRenderer->material);
        
        sceneMain->RemoveMeshRendererFromSceneRoot( geneRenderer, RENDER_QUEUE_DEFAULT );
        
        Renderer.DestroyMeshRenderer( geneRenderer );
        
        continue;
    }
    
    mStreamBuffer[index].actor->mGeneticRenderers.clear();
    mStreamBuffer[index].actor->mAnimationStates.clear();
    
    
    //
    // Create and express genetic elements
    //
    
    for (int a=0; a < numberOfGenes; a++) {
        
        if (!mStreamBuffer[index].actor->mGenes[a].doExpress) 
            continue;
        
        // Generate a new mesh renderer for the gene
        
        Material* newMaterial = Renderer.CreateMaterial();
        newMaterial->shader = shaders.color;
        newMaterial->diffuse.r = mStreamBuffer[index].actor->mGenes[a].color.x;
        newMaterial->diffuse.g = mStreamBuffer[index].actor->mGenes[a].color.y;
        newMaterial->diffuse.b = mStreamBuffer[index].actor->mGenes[a].color.z;
        
        newMaterial->ambient = Colors.black;
        newMaterial->diffuse = Colors.white;
        
        MeshRenderer* newRenderer = Renderer.CreateMeshRenderer();
        newRenderer->isActive = false;
        
        newRenderer->mesh     = meshes.cube;
        newRenderer->material = newMaterial;
        
        newRenderer->EnableFrustumCulling();
        
        // Position
        newRenderer->transform.position = mStreamBuffer[index].actor->mPosition;
        
        glm::vec3 offset(0);
        glm::vec4 orientation(0);
        
        if (mStreamBuffer[index].actor->mGenes[a].attachmentIndex > 0) {
            
            unsigned int attachmentIndex = mStreamBuffer[index].actor->mGenes[a].attachmentIndex - 1;
            
            // Position offset
            mStreamBuffer[index].actor->mGenes[a].offset.x = mStreamBuffer[index].actor->mGenes[attachmentIndex].offset.x;
            mStreamBuffer[index].actor->mGenes[a].offset.y = mStreamBuffer[index].actor->mGenes[attachmentIndex].offset.y;
            mStreamBuffer[index].actor->mGenes[a].offset.z = mStreamBuffer[index].actor->mGenes[attachmentIndex].offset.z;
            
            // Color
            newMaterial->diffuse.r = mStreamBuffer[index].actor->mGenes[attachmentIndex].color.x;
            newMaterial->diffuse.g = mStreamBuffer[index].actor->mGenes[attachmentIndex].color.y;
            newMaterial->diffuse.b = mStreamBuffer[index].actor->mGenes[attachmentIndex].color.z;
            
            // Animation state
            mStreamBuffer[index].actor->mGenes[a].animationAxis = mStreamBuffer[index].actor->mGenes[attachmentIndex].animationAxis;
            mStreamBuffer[index].actor->mGenes[a].animationRange = mStreamBuffer[index].actor->mGenes[attachmentIndex].animationRange;
            mStreamBuffer[index].actor->mGenes[a].doAnimationCycle = mStreamBuffer[index].actor->mGenes[attachmentIndex].doAnimationCycle;
            mStreamBuffer[index].actor->mGenes[a].doInverseAnimation = mStreamBuffer[index].actor->mGenes[attachmentIndex].doInverseAnimation;
            
        }
        
        // Scale
        newRenderer->transform.scale.x = mStreamBuffer[index].actor->mGenes[a].scale.x;
        newRenderer->transform.scale.y = mStreamBuffer[index].actor->mGenes[a].scale.y;
        newRenderer->transform.scale.z = mStreamBuffer[index].actor->mGenes[a].scale.z;
        
        // Rotation
        newRenderer->transform.RotateAxis( mStreamBuffer[index].actor->mGenes[a].rotation.x, glm::vec3(1, 0, 0) );
        newRenderer->transform.RotateAxis( mStreamBuffer[index].actor->mGenes[a].rotation.y, glm::vec3(0, 1, 0) );
        newRenderer->transform.RotateAxis( mStreamBuffer[index].actor->mGenes[a].rotation.z, glm::vec3(0, 0, 1) );
        
        // Default animation orientation
        Transform transform;
        
        orientation = glm::vec4( transform.rotation.w, 
                                 transform.rotation.x, 
                                 transform.rotation.y, 
                                 transform.rotation.z );
        
        mStreamBuffer[index].actor->mGeneticRenderers.push_back( newRenderer );
        mStreamBuffer[index].actor->mAnimationStates .push_back( orientation );
        
        if (a == 0) 
            sceneMain->AddMeshRendererToSceneRoot( newRenderer, RENDER_QUEUE_DEFAULT );
        
        continue;
    }
    
    mStreamBuffer[index].actor->mDoUpdateGenetics = false;
    
    return;
}

