#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

// Snap to a grid
float Snap1D(float v, float grid, float origin = 0.0f);

// Smoothly damp values outside [minH, maxH]
float DampenHeightToRange(float v, float minH, float maxH, float softness);

void ChunkManager::GenerateBiome(glm::vec3* colorField, float* heightField, Chunk* chunk, Biome* biome, float* weightMask, float* totalWeights) {
    unsigned int chunkSZ = chunkSize + 1;
    unsigned int fieldSize = chunkSZ * chunkSZ;
    
    // Apply height contribution from each Perlin layer
    for (unsigned int i = 0; i < biome->perlin.size(); i++) {
        Perlin& layer = biome->perlin[i];
        
        for (unsigned int j = 0; j < fieldSize; j++) {
            unsigned int x = j % chunkSZ;
            unsigned int z = j / chunkSZ;
            
            float xCoord = ((float)x + chunk->x + layer.offsetX) * layer.noiseWidth;
            float zCoord = ((float)z + chunk->y + layer.offsetY) * layer.noiseHeight;
            
            float noise = Random.Perlin(xCoord, 0, zCoord, worldSeed) * layer.heightMultuplier;
            float normalizedWeight = weightMask[j] / (totalWeights[j] + 0.3f);
            float pointHeight = heightField[j] + (noise * normalizedWeight);
            
            if (pointHeight > layer.heightBlowoutHeight) 
                pointHeight *= layer.heightBlowoutMul;
            
            // Smoothly damp toward
            const float heightSoftness = 0.04f;
            pointHeight = DampenHeightToRange(pointHeight, layer.heightMin, layer.heightMax, heightSoftness);
            
            //pointHeight = Snap1D(pointHeight, 1.0f, 0.5f);
            heightField[j] = pointHeight;
        }
        
    }
    
    // Apply biome blending weights
    for (unsigned int i = 0; i < fieldSize; i++) {
        float normalizedWeight = weightMask[i] / (totalWeights[i] + 0.3f);
        normalizedWeight = glm::clamp(normalizedWeight, 0.0f, 1.0f);
        if (colorField[i] == Colors.black.ToVec3()) 
            colorField[i] = biome->colorLow.ToVec3();
        
        Color colorBase(colorField[i].x, colorField[i].y, colorField[i].z);
        Color colorLow = biome->colorLow;
        Color colorHigh = biome->colorHigh;
        
        float heightBias = (chunk->heightField[i] - biome->colorHeight) * biome->colorBias;
        heightBias = glm::clamp(heightBias, 0.0f, 1.0f);
        Color colorTarget = Colors.Lerp(biome->colorLow, biome->colorHigh, heightBias);
        
        Color result = Colors.Lerp(colorBase, colorTarget, normalizedWeight);
        
        colorField[i] = glm::vec3(result.r, result.g, result.b);
    }
}

void HeightMapping::AddColorFieldFromPerlinNoise(glm::vec3* colorField, unsigned int width, unsigned int height, 
                                                float noiseWidth, float noiseHeight, int seed, Color color, int offsetX, int offsetZ) {
    unsigned int size = width * height;
    for (unsigned int i = 0; i < size; i++) {
        unsigned int x = i % width;
        unsigned int z = i / width;
        
        float xCoord = ((float)x + offsetX) * noiseWidth;
        float zCoord = ((float)z + offsetZ) * noiseHeight;
        
        float noise = Random.Perlin(xCoord, 0, zCoord, seed);
        noise = glm::clamp(noise, 0.0f, 1.0f);
        
        Color original;
        original = Color(colorField[i].x, colorField[i].y, colorField[i].z);
        
        Color result = Colors.Lerp(original, color, noise);
        
        colorField[i] = glm::vec3(result.r, result.g, result.b);
    }
}




void HeightMapping::SetHeightFieldValues(float* heightField, unsigned int width, unsigned int height, float value) {
    unsigned int size = width * height;
    for (unsigned int i = 0; i < size; i++) {
        heightField[i] = value;
    }
}

float HeightMapping::AddHeightFieldFromPerlinNoise(float* heightField, unsigned int width, unsigned int height, 
                                                  float noiseWidth, float noiseHeight, 
                                                  float noiseMul, int offsetX, int offsetZ, float heightThreshold, int seed) {
    float minimumHeight = 1000.0f;
    unsigned int size = width * height;
    
    for (unsigned int i = 0; i < size; i++) {
        unsigned int x = i % width;
        unsigned int z = i / width;
        
        float xCoord = ((float)x + offsetX) * noiseWidth;
        float zCoord = ((float)z + offsetZ) * noiseHeight;
        
        float noise = Random.Perlin(xCoord, 0, zCoord, seed) * noiseMul;
        if (heightThreshold != 0.0f) {
            if (noise < heightThreshold) 
                continue;
            noise -= heightThreshold;
        }
        
        heightField[i] += noise;
        
        if (heightField[i] < minimumHeight)
            minimumHeight = heightField[i];
    }
    
    return minimumHeight;
}

void GenerateWaterTableFromHeightField(float* heightField, 
                                                     unsigned int width, unsigned int height, 
                                                     float tableHeight) {
    unsigned int size = width * height;
    for (unsigned int i = 0; i < size; i++) 
        if (heightField[i] < tableHeight) 
            heightField[i] *= 0.9;
    
}

float DampenHeightToRange(float v, float minH, float maxH, float softness) {
    //if (softness <= 0.0f || minH >= maxH) 
    //    return glm::clamp(v, minH, maxH);
    
    // Below min: asymptotically approach minH - (1/softness)
    if (v < minH) {
        float d = minH - v;
        float factor = 1.0f / (1.0f + d * softness);
        return minH - d * factor;
    }
    
    // Above max: asymptotically approach maxH + (1/softness)
    if (v > maxH) {
        float d = v - maxH;
        float factor = 1.0f / (1.0f + d * softness);
        return maxH + d * factor;
    }
    
    // Inside the band, leave it alone.
    return v;
}

