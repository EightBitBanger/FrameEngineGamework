// Particle system management class

#include <GameEngineFramework/Plugins/ParticleSystem/ParticleSystem.h>

#include <GameEngineFramework/Engine/Engine.h>


ParticleSystem::ParticleSystem() 
{
}


void ParticleSystem::Initiate(void) {
    
    
    
    return;
}

void ParticleSystem::Update(void) {
    
    glm::vec3 playerPosition = AI.GetPlayerWorldPosition();
    
    unsigned int numberOfEmitters = mEmitters.Size();
    
    // Update emitters
    
    for (unsigned int e=0; e < numberOfEmitters; e++) {
        
        Emitter* emitterPtr = mEmitters[e];
        
        if (!emitterPtr->mIsActive) 
            continue;
        
        // Check to spawn in more particles
        
        if (emitterPtr->mNumberOfParticles < emitterPtr->maxParticles) {
            
            // Spawn from a point
            
            if (emitterPtr->type == EmitterType::Point) {
                
                emitterPtr->mSpawnRate++;
                
                if (emitterPtr->mSpawnRate > emitterPtr->spawnRate) {
                    
                    emitterPtr->mSpawnRate = 0;
                    
                    glm::vec3 spawnPosition = emitterPtr->position;
                    glm::vec3 spawnScale = emitterPtr->scale;
                    glm::vec3 spawnVelocity = emitterPtr->direction;
                    
                    emitterPtr->AddParticle(spawnPosition, spawnScale, spawnVelocity, emitterPtr->colorBegin, emitterPtr->colorEnd);
                    
                }
                
            }
            
            // Spawn within an area
            if (emitterPtr->type == EmitterType::AreaEffector) {
                
                while (emitterPtr->mNumberOfParticles < emitterPtr->maxParticles) {
                    
                    glm::vec3 spawnPosition = playerPosition;
                    glm::vec3 spawnScale = emitterPtr->scale;
                    glm::vec3 spawnVelocity = emitterPtr->direction;
                    
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
        
        // Point emitter
        
        if (emitterPtr->type == EmitterType::Point) {
            
            // Update emitter particles
            for (unsigned int p=0; p < emitterPtr->mNumberOfParticles; p++) {
                
                // Solve velocity
                
                emitterPtr->mParticlePositions[p].x += emitterPtr->mParticleVelocities[p].x;
                emitterPtr->mParticlePositions[p].y += emitterPtr->mParticleVelocities[p].y;
                emitterPtr->mParticlePositions[p].z += emitterPtr->mParticleVelocities[p].z;
                
                // Integrate velocities
                
                emitterPtr->mParticleVelocities[p].x = Float.Lerp(emitterPtr->mParticleVelocities[p].x, emitterPtr->velocity.x, emitterPtr->velocityBias);
                emitterPtr->mParticleVelocities[p].y = Float.Lerp(emitterPtr->mParticleVelocities[p].y, emitterPtr->velocity.y, emitterPtr->velocityBias);
                emitterPtr->mParticleVelocities[p].z = Float.Lerp(emitterPtr->mParticleVelocities[p].z, emitterPtr->velocity.z, emitterPtr->velocityBias);
                
                // Constraints
                
                if ((emitterPtr->mParticlePositions[p].x < emitterPtr->position.x - emitterPtr->width) | 
                    (emitterPtr->mParticlePositions[p].x > emitterPtr->position.x + emitterPtr->width) | 
                    (emitterPtr->mParticlePositions[p].y < emitterPtr->position.y - emitterPtr->height) | 
                    (emitterPtr->mParticlePositions[p].y > emitterPtr->position.y + emitterPtr->height) | 
                    (emitterPtr->mParticlePositions[p].z < emitterPtr->position.z - emitterPtr->width) | 
                    (emitterPtr->mParticlePositions[p].z > emitterPtr->position.z + emitterPtr->width)) {
                    
                    emitterPtr->ResetParticle(p);
                    
                }
                
                // Interpolate color
                
                Color sourceColor;
                
                sourceColor.r = emitterPtr->mParticleColors[p].x;
                sourceColor.g = emitterPtr->mParticleColors[p].y;
                sourceColor.b = emitterPtr->mParticleColors[p].z;
                
                Color blendedColor = Colors.Lerp(sourceColor, emitterPtr->colorEnd, emitterPtr->colorBias);
                
                emitterPtr->mParticleColors[p].x = blendedColor.r;
                emitterPtr->mParticleColors[p].y = blendedColor.g;
                emitterPtr->mParticleColors[p].z = blendedColor.b;
                
                emitterPtr->mMesh->ChangeSubMeshPosition(p, emitterPtr->mParticlePositions[p].x, 
                                                            emitterPtr->mParticlePositions[p].y, 
                                                            emitterPtr->mParticlePositions[p].z);
                
                emitterPtr->mMesh->ChangeSubMeshColor(p, blendedColor);
                
            }
            
            emitterPtr->mMesh->Load();
            
            continue;
        }
        
        // Area effector emitter
        
        if (emitterPtr->type == EmitterType::AreaEffector) {
            
            // Update emitter particles
            for (unsigned int p=0; p < emitterPtr->mNumberOfParticles; p++) {
                
                // Solve velocity
                emitterPtr->mParticlePositions[p].x += emitterPtr->mParticleVelocities[p].x;
                emitterPtr->mParticlePositions[p].y += emitterPtr->mParticleVelocities[p].y;
                emitterPtr->mParticlePositions[p].z += emitterPtr->mParticleVelocities[p].z;
                
                // Integrate velocities
                
                emitterPtr->mParticleVelocities[p].x = Float.Lerp(emitterPtr->mParticleVelocities[p].x, emitterPtr->velocity.x, emitterPtr->velocityBias);
                emitterPtr->mParticleVelocities[p].y = Float.Lerp(emitterPtr->mParticleVelocities[p].y, emitterPtr->velocity.y, emitterPtr->velocityBias);
                emitterPtr->mParticleVelocities[p].z = Float.Lerp(emitterPtr->mParticleVelocities[p].z, emitterPtr->velocity.z, emitterPtr->velocityBias);
                
                // Constraints
                if ((emitterPtr->mParticlePositions[p].x < playerPosition.x - emitterPtr->width) | 
                    (emitterPtr->mParticlePositions[p].x > playerPosition.x + emitterPtr->width)  | 
                    (emitterPtr->mParticlePositions[p].y < playerPosition.y - emitterPtr->height) | 
                    (emitterPtr->mParticlePositions[p].y > playerPosition.y + emitterPtr->height) | 
                    (emitterPtr->mParticlePositions[p].z < playerPosition.z - emitterPtr->width) | 
                    (emitterPtr->mParticlePositions[p].z > playerPosition.z + emitterPtr->width) | 
                    (emitterPtr->mParticlePositions[p].y > emitterPtr->heightMaximum) | 
                    (emitterPtr->mParticlePositions[p].y < emitterPtr->heightMinimum)) {
                    
                    float randomX = Random.Range(0.0f, emitterPtr->width) - Random.Range(0.0f, emitterPtr->width);
                    float randomY = Random.Range(0.0f, emitterPtr->height) - Random.Range(0.0f, emitterPtr->height);
                    float randomZ = Random.Range(0.0f, emitterPtr->width) - Random.Range(0.0f, emitterPtr->width);
                    
                    emitterPtr->mParticlePositions[p].x = playerPosition.x + randomX;
                    emitterPtr->mParticlePositions[p].y = playerPosition.y + randomY;
                    emitterPtr->mParticlePositions[p].z = playerPosition.z + randomZ;
                    
                    if (emitterPtr->mParticlePositions[p].y < emitterPtr->heightMinimum) 
                        emitterPtr->mParticlePositions[p].y = emitterPtr->heightMinimum + 200;
                    
                }
                
                emitterPtr->mMesh->ChangeSubMeshPosition(p, emitterPtr->mParticlePositions[p].x, 
                                                            emitterPtr->mParticlePositions[p].y, 
                                                            emitterPtr->mParticlePositions[p].z);
                
            }
            
            emitterPtr->mMesh->Load();
            
            continue;
        }
        
        continue;
    }
    
    return;
}


Emitter* ParticleSystem::CreateEmitter(void) {
    
    Emitter* newEmitter = mEmitters.Create();
    
    newEmitter->mParticleObject = Engine.Create<GameObject>();
    
    // Material
    
    Material* particleMaterial = Engine.Create<Material>();
    newEmitter->mMaterial = particleMaterial;
    particleMaterial->shader = Engine.shaders.colorUnlit;
    particleMaterial->isShared = false;
    
    particleMaterial->ambient = Colors.white;
    particleMaterial->diffuse = Colors.white;
    particleMaterial->DisableCulling();
    
    // Mesh
    
    Mesh* particleMesh = Engine.Create<Mesh>();
    newEmitter->mMesh = particleMesh;
    particleMesh->isShared = false;
    
    newEmitter->mParticleObject->AddComponent( Engine.CreateComponentMeshRenderer( particleMesh, particleMaterial ) );
    
    Engine.sceneMain->AddMeshRendererToSceneRoot( newEmitter->mParticleObject->GetComponent<MeshRenderer>(), RENDER_QUEUE_GEOMETRY );
    
    return newEmitter;
}

void ParticleSystem::DestroyEmitter(Emitter* emitterPtr) {
    
    MeshRenderer* particleRenderer = emitterPtr->mParticleObject->GetComponent<MeshRenderer>();
    
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( particleRenderer, RENDER_QUEUE_GEOMETRY );
    
    Engine.Destroy<GameObject>( emitterPtr->mParticleObject );
    
    return;
}

