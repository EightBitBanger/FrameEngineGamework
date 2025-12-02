#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Plugins/ParticleSystem/Emitter.h>

Emitter::Emitter() : 
    type(EmitterType::Point),
    position(glm::vec3(0)),
    direction(glm::vec3(0)),
    scale(glm::vec3(1)),
    scaleTo(glm::vec3(1)),
    
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
    
    mMesh(nullptr),
    mMaterial(nullptr),
    mMeshRenderer(nullptr) {}

void Emitter::AddParticle(glm::vec3 position, glm::vec3 initialScale, glm::vec3 force, Color colorBegin, Color colorEnd) {
    mNumberOfParticles++;
    
    SubMesh particleSubMesh;
    Engine.meshes.cube->GetSubMesh(0, particleSubMesh);
    
    unsigned int numberOfVertices = particleSubMesh.vertexBuffer.size();
    
    for (unsigned int i = 0; i < numberOfVertices; i++) {
        
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
    mParticleScale.push_back(glm::vec3(1.0f));
    mParticleColors.push_back(glm::vec3(colorBegin.r, colorBegin.g, colorBegin.b));
}

unsigned int Emitter::ResetParticle(unsigned int index, glm::vec3 initialScale, Color initialColor) {
    // Reset mesh fully
    SubMesh particleSubMesh;
    Engine.meshes.cube->GetSubMesh(0, particleSubMesh);
    
    unsigned int numberOfVertices = particleSubMesh.vertexBuffer.size();
    
    for (unsigned int i = 0; i < numberOfVertices; i++) {
        
        particleSubMesh.vertexBuffer[i].x *= initialScale.x;
        particleSubMesh.vertexBuffer[i].y *= initialScale.y;
        particleSubMesh.vertexBuffer[i].z *= initialScale.z;
        
        particleSubMesh.vertexBuffer[i].r = initialColor.r;
        particleSubMesh.vertexBuffer[i].g = initialColor.g;
        particleSubMesh.vertexBuffer[i].b = initialColor.b;
    }
    mMesh->ReplaceSubMesh(index, particleSubMesh);
    
    // Spawn spread
    float initSpread = spread;
    glm::vec3 randomOffset(0);
    
    if (initSpread != 0.0f) {
        randomOffset.x = (Random.Range(0, 100) * 0.001f - Random.Range(0, 100) * 0.001f) * initSpread;
        randomOffset.y = (Random.Range(0, 100) * 0.001f - Random.Range(0, 100) * 0.001f) * initSpread;
        randomOffset.z = (Random.Range(0, 100) * 0.001f - Random.Range(0, 100) * 0.001f) * initSpread;
    }
    mParticlePositions[index] = position + randomOffset;
    
    // Spawn angle
    float initAngle = angle;
    glm::vec3 randomVelocity(0);
    
    if (initAngle != 0.0f) {
        randomVelocity.x = (Random.Range(0, 100) * 0.0001f - Random.Range(0, 100) * 0.0001f) * initAngle;
        randomVelocity.y = (Random.Range(0, 100) * 0.0001f - Random.Range(0, 100) * 0.0001f) * initAngle;
        randomVelocity.z = (Random.Range(0, 100) * 0.0001f - Random.Range(0, 100) * 0.0001f) * initAngle;
    }
    mParticleVelocities[index] = direction + randomVelocity;
    
    mParticleColors[index] = glm::vec3(colorBegin.r, colorBegin.g, colorBegin.b);
    return index;
}

void Emitter::Activate(void) {
    if (mIsActive) 
        return;
    Engine.sceneMain->AddMeshRendererToSceneRoot(mMeshRenderer, RENDER_QUEUE_GEOMETRY);
    mIsActive = true;
}

void Emitter::Deactivate(void) {
    if (!mIsActive) 
        return;
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot(mMeshRenderer, RENDER_QUEUE_GEOMETRY);
    mIsActive = false;
}

Material* Emitter::GetMaterial(void) {
    return mMaterial;
}
