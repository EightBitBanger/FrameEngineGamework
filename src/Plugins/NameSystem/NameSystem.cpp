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
    std::lock_guard<std::mutex> lock(mux);
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

std::string NameGenerator::GenerateFirstName(
    const std::vector<std::string>& prefixes,
    const std::vector<std::string>& names,
    const std::vector<std::string>& suffixes,
    const std::vector<std::string>& coreSuffixes) {
    std::lock_guard<std::mutex> lock(mux);
    
    // Collect all valid roots (prefixes + names) to pull authentic thematic roots
    std::vector<std::string> roots;
    roots.insert(roots.end(), prefixes.begin(), prefixes.end());
    roots.insert(roots.end(), names.begin(), names.end());
    
    if (roots.empty()) return "";
    
    // Pick a single base root
    std::string root = roots[prng.range(0, roots.size() - 1)];
    
    // If the selected root is already quite long (> 4 chars), trim it down to a single punchy syllable
    if (root.length() > 4 && prng.range(1, 100) <= 60) {
        size_t cutLen = prng.range(3, 4);
        root = root.substr(0, cutLen);
    }
    
    // Light Mutation (20% chance per character to avoid over-distorting the ethnic root)
    for (size_t i = 0; i < root.length(); ++i) {
        if (prng.range(1, 100) <= 20) {
            if (IsVowel(root[i])) {
                root[i] = vowels[prng.range(0, vowels.size() - 1)];
            } else if (std::isalpha(static_cast<unsigned char>(root[i]))) {
                root[i] = consonants[prng.range(0, consonants.size() - 1)];
            }
        }
    }
    
    // Optional short ending (prefer coreSuffixes or single-syllable suffixes)
    // 55% chance to attach an ending; 45% chance to leave as a strong monosyllabic root (e.g., "Grim", "Stark", "Wolf")
    std::string ending = "";
    if (prng.range(1, 100) <= 55) {
        const std::vector<std::string>& endingPool = !coreSuffixes.empty() ? coreSuffixes : suffixes;
        
        if (!endingPool.empty()) {
            std::string candidate = endingPool[prng.range(0, endingPool.size() - 1)];
            // Reject compound suffixes (e.g., "dottir", "stadt", "dorf") to keep names short
            if (candidate.length() <= 3) {
                ending = candidate;
            }
        }
    }
    
    // Combine root and ending
    std::string result = root + ending;
    
    // Prevent awkward double vowels/consonants at the joint (e.g., "Thor" + "rok" -> "Thorok")
    if (result.length() >= 2 && !ending.empty()) {
        char lastRoot = std::tolower(static_cast<unsigned char>(root.back()));
        char firstEnd = std::tolower(static_cast<unsigned char>(ending.front()));
        
        if (lastRoot == firstEnd) {
            result = root + ending.substr(1);
        }
    }
    
    // 4. Normalize Casing (e.g., "thoralf" -> "Thoralf")
    result[0] = std::toupper(static_cast<unsigned char>(result[0]));
    for (size_t i = 1; i < result.length(); ++i) {
        result[i] = std::tolower(static_cast<unsigned char>(result[i]));
    }
    
    return result;
}
