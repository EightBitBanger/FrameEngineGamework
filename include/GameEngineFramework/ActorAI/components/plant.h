#ifndef _AI_PLANT__
#define _AI_PLANT__

#include <GameEngineFramework/ActorAI/ActorStates.h>

#include <GameEngineFramework/ActorAI/Genetics/Gene.h>
#include <GameEngineFramework/ActorAI/neurons/NeuralLayer.h>
#include <GameEngineFramework/ActorAI/neurons/WeightedLayer.h>

#include <GameEngineFramework/Physics/PhysicsSystem.h>
#include <GameEngineFramework/Renderer/RenderSystem.h>

#include <glm/glm.hpp>

#include <vector>
#include <mutex>

#include <string>


class ENGINE_API Plant {
    
public:
    
    friend class ActorSystem;
    friend class EngineSystemManager;
    
    /// Set the name of the plant.
    void SetName(std::string newName);
    /// Get the name of the plant.
    std::string GetName(void);
    
    /// Set the active state of the plant.
    void SetActive(bool state);
    /// Get the active state of the plant.
    bool GetActive(void);
    
    /// Set the age of the plant.
    void SetAge(unsigned long int newAge);
    /// Get the age of the plant.
    unsigned long int GetAge(void);
    
    /// Set the position of the plant.
    void SetPosition(glm::vec3 position);
    /// Get the position of the plant.
    glm::vec3 GetPosition(void);
    
    
    Plant();
    
private:
    
    // Plant name string
    std::string mName;
    
    // Number of ticks this plant has accumulated in its lifetime
    unsigned long int mAge;
    
    // Is this plant active in the simulation
    bool mIsActive;
    
    // Should the genetic data be re-expressed
    bool mDoUpdateGenetics;
    
    // Position of the plant in the world
    glm::vec3 mPosition;
    
    std::mutex mux;
    
};





#endif
