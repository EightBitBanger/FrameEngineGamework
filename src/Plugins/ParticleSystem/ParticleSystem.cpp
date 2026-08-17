#include <GameEngineFramework/Plugins/ParticleSystem/ParticleSystem.h>
#include <GameEngineFramework/Engine/Engine.h>

CustomAllocator particlePools{1000, 1};

ParticleSystem::ParticleSystem() : 
    mEmitters(particlePools) {}

void ParticleSystem::Initiate(void) {}

void ParticleSystem::Update(void) {
    glm::vec3 playerPosition = AI.GetPlayerWorldPosition();
    unsigned int numberOfEmitters = mEmitters.Size();
    
    for (unsigned int e = 0; e < numberOfEmitters; e++) {
        Emitter* emitterPtr = mEmitters[e];
        if (!emitterPtr->mIsActive) 
            continue;
        
        if (emitterPtr->mNumberOfParticles < emitterPtr->maxParticles) {
            
            // Point emitter
            if (emitterPtr->type == EmitterType::Point) {
                emitterPtr->mSpawnRate++;
                
                if (emitterPtr->mSpawnRate > emitterPtr->spawnRate) {
                    emitterPtr->mSpawnRate = 0;
                    
                    // Calculate spread offset
                    float initSpread = emitterPtr->spread;
                    glm::vec3 randomOffset(0);
                    
                    if (initSpread != 0.0f) {
                        randomOffset.x = (Random.Range(0, 100) * 0.001f - Random.Range(0, 100) * 0.001f) * initSpread;
                        randomOffset.y = (Random.Range(0, 100) * 0.001f - Random.Range(0, 100) * 0.001f) * initSpread;
                        randomOffset.z = (Random.Range(0, 100) * 0.001f - Random.Range(0, 100) * 0.001f) * initSpread;
                    }

                    // Calculate angle velocity offset
                    float initAngle = emitterPtr->angle;
                    glm::vec3 randomVelocity(0);
                    
                    if (initAngle != 0.0f) {
                        randomVelocity.x = (Random.Range(0, 100) * 0.0001f - Random.Range(0, 100) * 0.0001f) * initAngle;
                        randomVelocity.y = (Random.Range(0, 100) * 0.0001f - Random.Range(0, 100) * 0.0001f) * initAngle;
                        randomVelocity.z = (Random.Range(0, 100) * 0.0001f - Random.Range(0, 100) * 0.0001f) * initAngle;
                    }
                    
                    glm::vec3 spawnPosition = emitterPtr->position + randomOffset;
                    glm::vec3 spawnVelocity = emitterPtr->direction + randomVelocity;
                    glm::vec3 spawnScale    = emitterPtr->scale;
                    
                    emitterPtr->AddParticle(spawnPosition, spawnScale, spawnVelocity, emitterPtr->colorBegin, emitterPtr->colorEnd);
                }
            }
            
            // Area emitter
            if (emitterPtr->type == EmitterType::AreaEffector) {
                while (emitterPtr->mNumberOfParticles < emitterPtr->maxParticles) {
                    glm::vec3 spawnPosition = playerPosition;
                    glm::vec3 spawnVelocity = emitterPtr->direction;
                    glm::vec3 spawnScale    = emitterPtr->scale;
                    
                    float randomX = Random.Range(0.0f, emitterPtr->width) - Random.Range(0.0f, emitterPtr->width);
                    float randomY = Random.Range(0.0f, emitterPtr->height) - Random.Range(0.0f, emitterPtr->height);
                    float randomZ = Random.Range(0.0f, emitterPtr->width) - Random.Range(0.0f, emitterPtr->width);
                    
                    spawnPosition.x += randomX;
                    spawnPosition.y += randomY;
                    spawnPosition.z += randomZ;
                    
                    emitterPtr->AddParticle(spawnPosition, spawnScale, spawnVelocity, emitterPtr->colorBegin, emitterPtr->colorEnd);
                }
            }
        }
        
        // Point emitter update loop
        if (emitterPtr->type == EmitterType::Point) {
            for (unsigned int p = 0; p < emitterPtr->mNumberOfParticles; p++) {
                emitterPtr->mParticlePositions[p] += emitterPtr->mParticleVelocities[p];
                
                emitterPtr->mParticleVelocities[p] = glm::mix(emitterPtr->mParticleVelocities[p], emitterPtr->velocity, emitterPtr->velocityBias);
                
                if ((emitterPtr->mParticlePositions[p].x < emitterPtr->position.x - emitterPtr->width) || 
                    (emitterPtr->mParticlePositions[p].x > emitterPtr->position.x + emitterPtr->width) || 
                    (emitterPtr->mParticlePositions[p].y < emitterPtr->position.y - emitterPtr->height) || 
                    (emitterPtr->mParticlePositions[p].y > emitterPtr->position.y + emitterPtr->height) || 
                    (emitterPtr->mParticlePositions[p].z < emitterPtr->position.z - emitterPtr->width) || 
                    (emitterPtr->mParticlePositions[p].z > emitterPtr->position.z + emitterPtr->width)) {
                    
                    emitterPtr->ResetParticle(p, emitterPtr->scale, emitterPtr->colorBegin);
                }
                
                Color sourceColor(
                    emitterPtr->mParticleColors[p].x,
                    emitterPtr->mParticleColors[p].y,
                    emitterPtr->mParticleColors[p].z,
                    emitterPtr->mParticleColors[p].w
                );
                Color blendedColor = Colors.Lerp(sourceColor, emitterPtr->colorEnd, emitterPtr->colorBias);
                
                emitterPtr->mParticleColors[p] = glm::vec4(blendedColor.r, blendedColor.g, blendedColor.b, blendedColor.a);
                
                emitterPtr->mMesh->ChangeSubMeshPosition(p, emitterPtr->mParticlePositions[p].x, emitterPtr->mParticlePositions[p].y, emitterPtr->mParticlePositions[p].z);
                emitterPtr->mMesh->ChangeSubMeshScale(p, emitterPtr->scaleTo.x, emitterPtr->scaleTo.y, emitterPtr->scaleTo.z);
                emitterPtr->mMesh->ChangeSubMeshColor(p, blendedColor);
            }
            
            emitterPtr->mMesh->Load();
        }
        
        // Area effector emitter update loop
        if (emitterPtr->type == EmitterType::AreaEffector) {
            for (unsigned int p=0; p < emitterPtr->mNumberOfParticles; p++) {
                emitterPtr->mParticlePositions[p] += emitterPtr->mParticleVelocities[p];
                emitterPtr->scale *= emitterPtr->scaleTo;
                
                emitterPtr->mParticleVelocities[p] = glm::mix(emitterPtr->mParticleVelocities[p], emitterPtr->velocity, emitterPtr->velocityBias);
                
                if ((emitterPtr->mParticlePositions[p].x < playerPosition.x - emitterPtr->width) || 
                    (emitterPtr->mParticlePositions[p].x > playerPosition.x + emitterPtr->width) || 
                    (emitterPtr->mParticlePositions[p].y < playerPosition.y - emitterPtr->height) || 
                    (emitterPtr->mParticlePositions[p].y > playerPosition.y + emitterPtr->height) || 
                    (emitterPtr->mParticlePositions[p].z < playerPosition.z - emitterPtr->width) || 
                    (emitterPtr->mParticlePositions[p].z > playerPosition.z + emitterPtr->width) || 
                    (emitterPtr->mParticlePositions[p].y > emitterPtr->heightMaximum) || 
                    (emitterPtr->mParticlePositions[p].y < emitterPtr->heightMinimum)) {
                    
                    float randomX = Random.Range(0.0f, emitterPtr->width)  - Random.Range(0.0f, emitterPtr->width);
                    float randomY = Random.Range(0.0f, emitterPtr->height) - Random.Range(0.0f, emitterPtr->height);
                    float randomZ = Random.Range(0.0f, emitterPtr->width)  - Random.Range(0.0f, emitterPtr->width);
                    
                    emitterPtr->mParticlePositions[p] = playerPosition + glm::vec3(randomX, randomY, randomZ);
                }
                
                emitterPtr->mMesh->ChangeSubMeshPosition(p, emitterPtr->mParticlePositions[p].x, emitterPtr->mParticlePositions[p].y, emitterPtr->mParticlePositions[p].z);
            }
            
            emitterPtr->mMesh->Load();
        }
    }
}

Emitter* ParticleSystem::CreateEmitter(void) {
    Emitter* newEmitter = mEmitters.Create();
    
    // Material
    Material* particleMaterial = Engine.Create<Material>();
    newEmitter->mMaterial = particleMaterial;
    
    particleMaterial->shader = Resources.shaders.color;
    particleMaterial->isShared = false;
    particleMaterial->ambient = Colors.white;
    particleMaterial->diffuse = Colors.white;
    particleMaterial->DisableCulling();
    
    // Enable alpha blending
    particleMaterial->EnableBlending();
    //particleMaterial->DisableDepthTest();
    //particleMaterial->SetBlending(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
    
    // Mesh
    Mesh* particleMesh = Engine.Create<Mesh>();
    newEmitter->mMesh = particleMesh;
    particleMesh->isShared = false;
    
    newEmitter->mMeshRenderer = Engine.Create<MeshRenderer>();
    newEmitter->mMeshRenderer->isActive = true;
    newEmitter->mMeshRenderer->mesh = particleMesh;
    newEmitter->mMeshRenderer->material = particleMaterial;
    
    Engine.sceneMain->AddMeshRendererToSceneRoot(newEmitter->mMeshRenderer, RENDER_QUEUE_GEOMETRY);
    
    return newEmitter;
}

void ParticleSystem::DestroyEmitter(Emitter* emitterPtr) {
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot(emitterPtr->mMeshRenderer, RENDER_QUEUE_GEOMETRY);
    Engine.Destroy<MeshRenderer>(emitterPtr->mMeshRenderer);
    
    mEmitters.Destroy(emitterPtr);
}
