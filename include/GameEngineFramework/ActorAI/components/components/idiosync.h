#ifndef AI_ACTOR_MEMORIES
#define AI_ACTOR_MEMORIES

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/ActorAI/components/components/memorytrigger.h>

class ENGINE_API MemorySystem {
    friend class Actor;
    friend class ActorSystem;
    friend class GeneticPresets;
    friend class EngineSystemManager;
public:
    
    /// Add a memory to this actor.
    void Add(const std::string& name, const std::string& memory);
    /// Remove a memory from this actor.
    void Remove(const std::string& name);
    /// Get a memory from this actor.
    std::string Get(const std::string& name);
    
    /// Get the number of memories in this actor.
    unsigned int GetNumberOfMemories(void);
    /// Return the name of a memory by its index.
    std::string GetMemoryNameByIndex(unsigned int index);
    /// Return the value of a memory by its index.
    std::string GetMemoryValueByIndex(unsigned int index);
    
    /// Clear a specific emotion across the thought matrix.
    void ClearEmotion(const std::string& emotionName);
    void ClearEmotion(TriggerType type);
    /// Set a specific emotion to a fixed value across the thought matrix.
    void SetEmotion(const std::string& emotionName, float value);
    void SetEmotion(TriggerType type, float value);
    /// Scale a specific emotion across the thought matrix.
    void ScaleEmotion(const std::string& emotionName, float factor);
    void ScaleEmotion(TriggerType type, float factor);
    
    /// Clear all the actor memories.
    void Clear(void);
    
    /// Check if a memory exists in this actor.
    bool CheckExists(const std::string& memory);
    
    /// Update the optimized list of memories.
    void UpdateMemories(void);
    
    MemorySystem();
    
private:
    
    bool mDoUpdateMemories;
    
    // List of memories collected by this entity
    std::unordered_map<std::string, std::string> mMemories;
    
    // List of optimized memories for internal operation
    std::unordered_map<std::string, std::vector<MemoryTrigger>> mMemoryTriggers;
    
    // List of emotional states forming the thought process matrix through forward projection
    std::deque<EmotionalEmbedding> thoughtProcessProjectionMatrix;
};

#endif
