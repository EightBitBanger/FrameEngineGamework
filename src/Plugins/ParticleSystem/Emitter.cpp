// Particle system management class

#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/Plugins/ParticleSystem/Emitter.h>

Emitter::Emitter() : 
    type(EmitterType::Point),
    
    position(glm::vec3(0)),
    direction(glm::vec3(0)),
    scale(glm::vec3(1)),
    
    colorBegin(Colors.black),
    colorEnd(Colors.white),
    
    colorBias(0.04f),
    
    spawnRate(8),
    
    angle(0),
    spread(0),
    
    width(20),
    height(20),
    
    heightMinimum(-1000),
    heightMaximum(1000),
    
    velocity(glm::vec3(0)),
    
    velocityBias(0.01f),
    
    maxParticles(20),
    
    mIsActive(true),
    
    mSpawnRate(0),
    
    mNumberOfParticles(0),
    
    mParticleObject(nullptr),
    
    mMesh(nullptr),
    mMaterial(nullptr)
{
}

void Emitter::AddParticle(glm::vec3 position, glm::vec3 initialScale, glm::vec3 force, Color colorBegin, Color colorEnd) {
    
    mNumberOfParticles++;
    
    // Add sub mesh
    
    SubMesh particleSubMesh;
    Engine.meshes.cube->GetSubMesh(0, particleSubMesh);
    
    unsigned int numberOfVerticees = particleSubMesh.vertexBuffer.size();
    
    for (unsigned int i=0; i < numberOfVerticees; i++) {
        
        particleSubMesh.vertexBuffer[i].x *= initialScale.x;
        particleSubMesh.vertexBuffer[i].y *= initialScale.y;
        particleSubMesh.vertexBuffer[i].z *= initialScale.z;
        
        particleSubMesh.vertexBuffer[i].r = colorBegin.r;
        particleSubMesh.vertexBuffer[i].g = colorBegin.g;
        particleSubMesh.vertexBuffer[i].b = colorBegin.b;
        
    }
    
    mMesh->AddSubMesh(position.x, position.y, position.z, particleSubMesh, false);
    
    mParticlePositions.push_back(position);
    mParticleVelocities.push_back(force);
    mParticleColors.push_back(position);
    
    ResetParticle(mNumberOfParticles - 1);
    
    return;
}


unsigned int Emitter::ResetParticle(unsigned int index) {
    
    // Spawn spread
    
    float initSpread = spread;
    
    float randomX = 0;
    float randomY = 0;
    float randomZ = 0;
    
    if (initSpread != 0.0f) {
        
        randomX = ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100) * 0.001f)) * initSpread;
        randomY = ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100) * 0.001f)) * initSpread;
        randomZ = ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100) * 0.001f)) * initSpread;
        
    }
    
    mParticlePositions[index].x = position.x + randomX;
    mParticlePositions[index].y = position.y + randomY;
    mParticlePositions[index].z = position.z + randomZ;
    
    // Spawn angle
    
    float initAngle = angle;
    
    float velocityX = 0;
    float velocityY = 0;
    float velocityZ = 0;
    
    if (initAngle != 0.0f) {
        
        velocityX = ((Random.Range(0, 100) * 0.0001f) - (Random.Range(0, 100) * 0.0001f)) * initAngle;
        velocityY = ((Random.Range(0, 100) * 0.0001f) - (Random.Range(0, 100) * 0.0001f)) * initAngle;
        velocityZ = ((Random.Range(0, 100) * 0.0001f) - (Random.Range(0, 100) * 0.0001f)) * initAngle;
        
    }
    
    mParticleVelocities[index].x = direction.x + velocityX;
    mParticleVelocities[index].y = direction.y + velocityY;
    mParticleVelocities[index].z = direction.z + velocityZ;
    
    mParticleColors[index].r = colorBegin.r;
    mParticleColors[index].g = colorBegin.g;
    mParticleColors[index].b = colorBegin.b;
    
    return index;
}


/// Enable the rendering and updates for this emitter.
void Emitter::Activate(void) {
    
    if (mIsActive) 
        return;
    
    MeshRenderer* particleRenderer = mParticleObject->GetComponent<MeshRenderer>();
    
    Engine.sceneMain->AddMeshRendererToSceneRoot( particleRenderer, RENDER_QUEUE_GEOMETRY );
    
    mIsActive = true;
    
    return;
}

/// Disable the rendering and updates for this emitter.
void Emitter::Deactivate(void) {
    
    if (!mIsActive) 
        return;
    
    MeshRenderer* particleRenderer = mParticleObject->GetComponent<MeshRenderer>();
    
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( particleRenderer, RENDER_QUEUE_GEOMETRY );
    
    mIsActive = false;
    
    return;
}

Material* Emitter::GetMaterial(void) {
    
    return mMaterial;
}

