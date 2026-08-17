#include <GameEngineFramework/plugins/NameSystem/NameSystem.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <chrono>

NameGenerator::NameGenerator() {
    // Default seed using system clock to ensure fresh state on startup
    uint32_t defaultSeed = static_cast<uint32_t>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()
    );
    prng.seed(defaultSeed);
}

NameGenerator::NameGenerator(uint32_t seed) {
    prng.seed(seed);
}

void NameGenerator::SetSeed(uint32_t seed) {
    prng.seed(seed);
}

void NameGenerator::SetSeed(double doubleTime) {
    // Simple hash mixer so small floating variations produce drastically different seeds
    uint64_t bits;
    memcpy(&bits, &doubleTime, sizeof(bits));
    uint32_t hashedSeed = static_cast<uint32_t>(bits ^ (bits >> 32));
    prng.seed(hashedSeed);
}

bool NameGenerator::IsVowel(char c) const {
    c = std::tolower(static_cast<unsigned char>(c));
    return std::find(vowels.begin(), vowels.end(), c) != vowels.end();
}

std::string NameGenerator::GenerateIncarnation(const std::string& sourceName, const std::vector<std::string>& coreSuffixes) {
    if (sourceName.empty()) return "";
    
    std::string result = sourceName;
    
    // Character Type Mutation (35% chance per letter)
    for (size_t i = 0; i < result.length(); ++i) {
        if (prng.range(1, 100) <= 35) {
            bool isUpper = std::isupper(static_cast<unsigned char>(result[i]));
    
            if (IsVowel(result[i])) {
                result[i] = vowels[prng.range(0, vowels.size() - 1)];
            } else if (std::isalpha(static_cast<unsigned char>(result[i]))) {
                result[i] = consonants[prng.range(0, consonants.size() - 1)];
            }
    
            if (isUpper) {
                result[i] = std::toupper(static_cast<unsigned char>(result[i]));
            }
        }
    }
    
    // Swap adjacent characters (20% chance)
    if (result.length() >= 3 && prng.range(1, 100) <= 20) {
        size_t idx = prng.range(1, result.length() - 2);
        std::swap(result[idx], result[idx + 1]);
    }
    
    // Duplicate a vowel (15% chance)
    if (prng.range(1, 100) <= 15) {
        for (size_t i = 0; i < result.length(); ++i) {
            if (IsVowel(result[i])) {
                result.insert(i, 1, result[i]);
                break; // Double only the first matching vowel
            }
        }
    }
    
    // Suffix Selection
    static const std::vector<std::string> defaultSuffixes = {"", "or", "is", "ath", "en", "ia", "us", "yl", "os"};
    const std::vector<std::string>& activeSuffixes = coreSuffixes.empty() ? defaultSuffixes : coreSuffixes;
    
    // Append procedural core suffix (40% chance)
    if (prng.range(1, 100) <= 40 && !activeSuffixes.empty()) {
        result += activeSuffixes[prng.range(0, activeSuffixes.size() - 1)];
    }
    
    return result;
}

std::string NameGenerator::GenerateIncarnation(const std::vector<std::string>& sourceNames, const std::vector<std::string>& coreSuffixes) {
    if (sourceNames.empty()) return "";
    if (sourceNames.size() == 1) return GenerateIncarnation(sourceNames[0], coreSuffixes);
    
    size_t index1 = prng.range(0, sourceNames.size() - 1);
    size_t index2 = prng.range(0, sourceNames.size() - 1);
    
    while (index2 == index1 && sourceNames.size() > 1) {
        index2 = prng.range(0, sourceNames.size() - 1);
    }
    
    const std::string& nameA = sourceNames[index1];
    const std::string& nameB = sourceNames[index2];
    
    if (nameA.empty()) return GenerateIncarnation(nameB, coreSuffixes);
    if (nameB.empty()) return GenerateIncarnation(nameA, coreSuffixes);
    
    // Pick dynamic slice points for high variations
    size_t cutA = prng.range(1, std::max<size_t>(1, nameA.length()));
    size_t cutB = prng.range(0, nameB.length() > 1 ? nameB.length() - 1 : 0);
    
    std::string hybridName = nameA.substr(0, cutA) + nameB.substr(cutB);
    
    return GenerateIncarnation(hybridName, coreSuffixes);
}

std::string NameGenerator::GenerateIncarnation(
    const std::vector<std::string>& prefixes,
    const std::vector<std::string>& names,
    const std::vector<std::string>& suffixes,
    const std::vector<std::string>& coreSuffixes) {
    std::string prefix = "";
    std::string coreName = "";
    std::string suffix = "";
    
    // 80% chance to include a prefix (if available)
    if (!prefixes.empty() && prng.range(1, 100) <= 80) {
        prefix = prefixes[prng.range(0, prefixes.size() - 1)];
    }
    
    // Always pick and mutate a core name (if available)
    if (!names.empty()) {
        std::string rawCore = names[prng.range(0, names.size() - 1)];
        coreName = GenerateIncarnation(rawCore, coreSuffixes);
    }
    
    // 80% chance to include a suffix (if available)
    if (!suffixes.empty() && prng.range(1, 100) <= 80) {
        suffix = suffixes[prng.range(0, suffixes.size() - 1)];
    }
    
    std::string result = prefix + coreName + suffix;
    if (result.empty()) return "";
    
    // Normalize casing (First letter capitalized, rest lowercase)
    result[0] = std::toupper(static_cast<unsigned char>(result[0]));
    for (size_t i = 1; i < result.length(); ++i) {
        result[i] = std::tolower(static_cast<unsigned char>(result[i]));
    }
    
    return result;
}
