#ifndef __PARTICLE_EMITTER_
#define __PARTICLE_EMITTER_

#include <GameEngineFramework/Engine/Engine.h>

enum class EmitterType {
    Point,
    AreaEffector
};

class ENGINE_API Emitter {
public:
    EmitterType type;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 scale;
    glm::vec3 scaleTo;
    
    Color colorBegin;
    Color colorEnd;
    float colorBias;
    
    float spawnRate;
    float angle;
    float spread;
    
    float width;
    float height;
    
    float heightMinimum;
    float heightMaximum;
    
    glm::vec3 velocity;
    float velocityBias;
    
    unsigned int maxParticles;
    
    Emitter();
    
    void AddParticle(glm::vec3 position, glm::vec3 initialScale, glm::vec3 force, Color colorBegin, Color colorEnd);
    unsigned int ResetParticle(unsigned int index, glm::vec3 initialScale, Color initialColor);
    void Activate(void);
    void Deactivate(void);
    Material* GetMaterial(void);
    
private:
    bool mIsActive;
    float mSpawnRate;
    unsigned int mNumberOfParticles;
    
    std::vector<glm::vec3> mParticlePositions;
    std::vector<glm::vec3> mParticleScale;
    std::vector<glm::vec4> mParticleColors;
    std::vector<glm::vec3> mParticleVelocities;
    
    Mesh* mMesh;
    Material* mMaterial;
    MeshRenderer* mMeshRenderer;
    
    friend class ParticleSystem;
};

#endif
