#ifndef __NAME_GENERATION_SYSTEM_
#define __NAME_GENERATION_SYSTEM_

#include <GameEngineFramework/Configuration.h>

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <mutex>

// Embedded Xorshift32 PRNG — Fast, completely isolated, and deterministic
struct InternalPRNG {
    uint32_t state = 0x853C49E6u;
    
    void seed(uint32_t seedValue) {
        // Prevent state 0 which locks up Xorshift
        state = (seedValue == 0) ? 0x853C49E6u : seedValue;
    }
    
    uint32_t next() {
        uint32_t x = state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        return state = x;
    }
    
    // Helper to pick a uniform integer in range [minVal, maxVal]
    size_t range(size_t minVal, size_t maxVal) {
        if (minVal >= maxVal) return minVal;
        return minVal + (next() % (maxVal - minVal + 1));
    }
};

class ENGINE_API NameGenerator {
public:
    NameGenerator();
    explicit NameGenerator(uint32_t seed);
    
    // Random seeding
    void SetSeed(uint32_t seed);
    void SetSeed(double doubleTime);
    
    // Phonetic groups for mutation
    const std::vector<char> vowels = {'a', 'e', 'i', 'o', 'u', 'y'};
    const std::vector<char> consonants = {
        'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 
        'n', 'p', 'q', 'r', 's', 't', 'v', 'w', 'x', 'z'
    };
    
    // Generate a shorter first name
    std::string GenerateFirstName(
        const std::vector<std::string>& prefixes,
        const std::vector<std::string>& names,
        const std::vector<std::string>& suffixes = {},
        const std::vector<std::string>& coreSuffixes = {}
    );
    
    // Mutates a single source name with optional custom core suffixes
    std::string GenerateIncarnation(
        const std::string& sourceName,
        const std::vector<std::string>& coreSuffixes = {}
    );
    
    // Reconglomerates multiple source names together using dynamic slices
    std::string GenerateIncarnation(
        const std::vector<std::string>& sourceNames,
        const std::vector<std::string>& coreSuffixes = {}
    );
    
    // Assembles names from component lists (Prefix + Mutated Core + Suffix)
    std::string GenerateIncarnation(
        const std::vector<std::string>& prefixes,
        const std::vector<std::string>& names,
        const std::vector<std::string>& suffixes,
        const std::vector<std::string>& coreSuffixes = {}
    );
    
private:
    std::mutex mux;
    
    InternalPRNG prng;
    
    bool IsVowel(char c) const;
};

#endif
