#ifndef __PARTICLE_EMITTER_
#define __PARTICLE_EMITTER_

#include <GameEngineFramework/Engine/Engine.h>


enum class EmitterType {
    
    Point,
    AreaEffector
    
};


class ENGINE_API Emitter {
    
public:
    
    /// Does this emitter effect an area within "width" and "height".
    /// False means its a point emitter starting at "position" spraying particles in "direction".
    EmitterType type;
    
    
    /// World position of this emitter.
    glm::vec3 position;
    
    /// Initial direction the particles will travel when spawned into the world.
    glm::vec3 direction;
    
    /// Size of the particles.
    glm::vec3 scale;
    
    /// Scale to which the particle should add per frame.
    glm::vec3 scaleTo;
    
    /// Color range between which the particles will fade.
    Color colorBegin;
    Color colorEnd;
    
    /// Rate to which the particles should fade from color "begin" to "end".
    float colorBias;
    
    /// Rate at which the particles will spawn.
    float spawnRate;
    
    /// Angle offset added to a particles initial velocity.
    float angle;
    
    /// Spread offset added to a particles initial spawn point.
    float spread;
    
    /// Width and height area within which the particles will spawn.
    float width;
    float height;
    
    /// Minimum and maximum height into which these particles will spawn.
    float heightMinimum;
    float heightMaximum;
    
    /// Direction of force the particles will travel through their lifetimes.
    glm::vec3 velocity;
    
    /// Rate to which the velocity will shift from "direction" to "velocity"
    float velocityBias;
    
    /// Maximum number of particles allowed to be spawned be this emitter.
    unsigned int maxParticles;
    
    Emitter();
    
    /// Add a particle to this emitter.
    void AddParticle(glm::vec3 position, glm::vec3 initialScale, glm::vec3 force, Color colorBegin, Color colorEnd);
    
    /// Reset a particle by its index.
    unsigned int ResetParticle(unsigned int index);
    
    /// Enable the rendering and updates for this emitter.
    void Activate(void);
    
    /// Disable the rendering and updates for this emitter.
    void Deactivate(void);
    
    /// Get the current material used by this emitter.
    Material* GetMaterial(void);
    
private:
    
    // Is this emitter currently active
    bool mIsActive;
    
    // Internal spawn rate conter
    float mSpawnRate;
    
    //  Current number of particles in the particle emitter
    unsigned int mNumberOfParticles;
    
    std::vector<glm::vec3> mParticlePositions;
    std::vector<glm::vec3> mParticleVelocities;
    std::vector<glm::vec3> mParticleColors;
    
    GameObject* mParticleObject;
    
    Mesh* mMesh;
    Material* mMaterial;
    
public:
    
    friend class ParticleSystem;
    
};


#endif
