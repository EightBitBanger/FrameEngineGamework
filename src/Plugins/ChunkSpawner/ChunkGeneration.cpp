#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>


void ChunkManager::GenerateBiomeColorField(glm::vec3* colorField, unsigned int width, unsigned int height,
                                           float noiseScaleX, float noiseScaleZ,
                                           int offsetX, int offsetZ, int seed) {
    // Define your biome base colors
    Color grassland = Colors.green;
    Color desert    = Colors.yellow;
    Color tundra    = Colors.white;
    Color forest    = Colors.dkgreen; // assuming a darker green exists
    Color mountain  = Colors.gray;
    
    for (unsigned int i = 0; i < width * height; i++) {
        unsigned int x = i % width;
        unsigned int z = i / width;
        
        float worldX = (float)(x + offsetX) * noiseScaleX;
        float worldZ = (float)(z + offsetZ) * noiseScaleZ;
        
        // Use Perlin noise to drive biome selection (0.0 - 1.0 range)
        float biomeNoise = glm::clamp(Random.Perlin(worldX, 0, worldZ, seed), 0.0f, 1.0f);
        
        Color baseColor;
        
        // You can divide biomeNoise range into sections for different biomes
        if (biomeNoise < 0.2f)
            baseColor = tundra;
        else if (biomeNoise < 0.4f)
            baseColor = mountain;
        else if (biomeNoise < 0.6f)
            baseColor = forest;
        else if (biomeNoise < 0.8f)
            baseColor = grassland;
        else
            baseColor = desert;
        
        // Add slight color variation for realism
        float variant = (Random.Range(0, 100) * 0.0001f) - (Random.Range(0, 100) * 0.0001f);
        baseColor.r += variant;
        baseColor.g += variant;
        baseColor.b += variant;
        
        colorField[i] = glm::vec3(baseColor.r, baseColor.g, baseColor.b);
    }
}










void ChunkManager::SetHeightFieldValues(float* heightField, unsigned int width, unsigned int height, float value) {
    unsigned int size = width * height;
    for (unsigned int i = 0; i < size; i++) {
        heightField[i] = value;
    }
    return;
}

void ChunkManager::SetColorFieldValues(glm::vec3* colorField, unsigned int width, unsigned int height, Color color) {
    glm::vec3 colorVec(color.r, color.g, color.b);
    unsigned int size = width * height;
    for (unsigned int i = 0; i < size; i++) {
        colorField[i] = colorVec;
    }
    return;
}

float ChunkManager::AddHeightFieldFromPerlinNoise(float* heightField, unsigned int width, unsigned int height, 
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
        
        heightField[i] += noise;//Math.Round((noise * 2.0)) * 0.5;
        
        if (heightField[i] < minimumHeight)
            minimumHeight = heightField[i];
    }
    
    return minimumHeight;
}

void ChunkManager::GenerateColorFieldFromHeightField(glm::vec3* colorField, float* heightField, 
                                                     unsigned int width, unsigned int height, 
                                                     Color low, Color high, float bias) {
    unsigned int size = width * height;
    
    for (unsigned int i = 0; i < size; i++) {
        float heightBias = heightField[i] * bias;
        heightBias = glm::clamp(heightBias, 0.0f, 1.0f);
        
        Color color = Colors.Lerp(low, high, heightBias);
        
        float uniformVariant = (Random.Range(0, 100) * 0.00001f) - (Random.Range(0, 10) * 0.00001f);
        
        color.r += uniformVariant;
        color.g += uniformVariant;
        color.b += uniformVariant;
        
        colorField[i] = glm::vec3(color.r, color.g, color.b);
    }
    return;
}

void ChunkManager::AddColorFieldFromPerlinNoise(glm::vec3* colorField, unsigned int width, unsigned int height, 
                                                float noiseWidth, float noiseHeight, Color color, int offsetX, int offsetZ) {
    unsigned int size = width * height;
    
    for (unsigned int i = 0; i < size; i++) {
        unsigned int x = i % width;
        unsigned int z = i / width;
        
        float xCoord = ((float)x + offsetX) * noiseWidth;
        float zCoord = ((float)z + offsetZ) * noiseHeight;
        
        float noise = Random.Perlin(xCoord, 0, zCoord, 100);
        noise = glm::clamp(noise, 0.0f, 1.0f);
        
        Color original;
        original = Color(colorField[i].x, colorField[i].y, colorField[i].z);
        
        Color result = Colors.Lerp(original, color, noise);
        
        colorField[i] = glm::vec3(result.r, result.g, result.b);
    }
    
    return;
}

void ChunkManager::GenerateWaterTableFromHeightField(float* heightField, 
                                                     unsigned int width, unsigned int height, 
                                                     float tableHeight) {
    unsigned int size = width * height;
    for (unsigned int i = 0; i < size; i++) 
        if (heightField[i] < tableHeight) 
            heightField[i] *= 0.9;
    
    return;
}

void ChunkManager::AddColorFieldSnowCap(glm::vec3* colorField, float* heightField, 
                                        unsigned int width, unsigned int height, 
                                        Color capColor, float beginHeight, float bias) {
    unsigned int size = width * height;
    
    for (unsigned int i = 0; i < size; i++) {
        float heightBias = heightField[i] * 0.01;
        heightBias = glm::clamp(heightBias, 0.0f, 1.0f);
        
        Color color(colorField[i].x, colorField[i].y, colorField[i].z);
        int diff = ((beginHeight - (beginHeight - 20)) - (heightField[i] - beginHeight)) * bias;
        
        if (Random.Range(0, 100) > diff) 
            color = Colors.Lerp(color, capColor, heightField[i] * 0.07);
        
        colorField[i] = glm::vec3(color.r, color.g, color.b);
    }
    
    return;
}

void ChunkManager::AddColorFieldWaterTable(glm::vec3* colorField, float* heightField, 
                                           unsigned int width, unsigned int height, 
                                           Color waterColor, float beginHeight, float bias, float waterTableHeight) {
    unsigned int size = width * height;
    
    for (unsigned int i = 0; i < size; i++) {
        
        if (heightField[i] > waterTableHeight) continue;
        
        Color color( colorField[i].x, colorField[i].y, colorField[i].z );
        waterColor = 0.001f;
        
        float height = heightField[i];
        if (height <= (waterTableHeight - 9.0f))   color = Colors.Lerp(color, waterColor, 1.0f);
        else if (height <= (waterTableHeight - 8.0f)) color = Colors.Lerp(color, waterColor, 0.9f);
        else if (height <= (waterTableHeight - 7.0f)) color = Colors.Lerp(color, waterColor, 0.85f);
        else if (height <= (waterTableHeight - 6.0f)) color = Colors.Lerp(color, waterColor, 0.8f);
        else if (height <= (waterTableHeight - 5.0f)) color = Colors.Lerp(color, waterColor, 0.75f);
        else if (height <= (waterTableHeight - 4.0f)) color = Colors.Lerp(color, waterColor, 0.7f);
        else if (height <= (waterTableHeight - 3.0f)) color = Colors.Lerp(color, waterColor, 0.65f);
        else if (height <= (waterTableHeight - 2.0f)) color = Colors.Lerp(color, waterColor, 0.6f);
        else if (height <= (waterTableHeight - 1.0f)) color = Colors.Lerp(color, waterColor, 0.55f);
        else if (height <= waterTableHeight) color = Colors.Lerp(color, waterColor, 0.5f);
        
        if (height < (waterTableHeight - 7.0f) && Random.Range(1, 100) > 70) color = Colors.orange * 0.003f;
        if (height < (waterTableHeight - 10.0f) && Random.Range(1, 100) > 90) color = Colors.green * 0.003f;
        if (height >= (waterTableHeight - 10.0f) && height < (waterTableHeight - 4.0f) && Random.Range(1, 100) > 88) color = Colors.green * 0.002f;
        if (height >= (waterTableHeight - 10.0f) && height < (waterTableHeight - 4.0f) && Random.Range(1, 100) > 80) color = Colors.green * 0.005f;
        if (height >= (waterTableHeight - 7.0f) && height < waterTableHeight && Random.Range(1, 100) > 90) color = Colors.brown * 0.01f;
        
        colorField[i] = glm::vec3(color.r, color.g, color.b);
    }
    
    return;
}

Mesh* ChunkManager::CreateMeshFromHeightField(float* heightField, 
                                              glm::vec3* colorField, 
                                              unsigned int width, unsigned int height, 
                                              float offsetX, float offsetZ) {
    Mesh* mesh = Engine.Create<Mesh>();
    
    AddHeightFieldToMesh(mesh, heightField, colorField, width, height, offsetX, offsetZ);
    
    return mesh;
}

void ChunkManager::AddHeightStepToMesh(float* heightField, unsigned int width, unsigned int height) {
    unsigned int size = width * height;
    
    for (unsigned int i = 0; i < size; i++) 
        heightField[i] = Math.Round((heightField[i] * 10)) / 10;
    
    return;
}

void ChunkManager::AddHeightFieldToMesh(Mesh* mesh, 
                                        float* heightField, glm::vec3* colorField, 
                                        unsigned int width, unsigned int height, 
                                        float offsetX, float offsetZ, 
                                        unsigned int subTessX, unsigned int subTessZ) {
    
    unsigned int fieldWidth = (width / subTessX) - 1;
    unsigned int fieldHeight = (height / subTessZ) - 1;
    
    float sx = (subTessX > 1) ? subTessX * 4.0f : 1.0f;
    float sz = (subTessZ > 1) ? subTessZ * 4.0f : 1.0f;
    
    for (unsigned int x = 0; x < fieldWidth; x++) {
        
        for (unsigned int z = 0; z < fieldHeight; z++) {
            unsigned int xa = x * subTessX;
            unsigned int za = z * subTessZ;
            
            float yyA = heightField[za * width + xa];
            float yyB = heightField[za * width + (xa + 1)];
            float yyC = heightField[(za + 1) * width + (xa + 1)];
            float yyD = heightField[(za + 1) * width + xa];
            
            glm::vec3 cA = colorField[za * width + xa];
            glm::vec3 cB = colorField[za * width + xa];
            glm::vec3 cC = colorField[za * width + xa];
            glm::vec3 cD = colorField[za * width + xa];
            
            float xx = (((float)x + offsetX - (float)width / 2) / 2) + 0.25;
            float zz = (((float)z + offsetZ - (float)height / 2) / 2) + 0.25;
            
            Vertex vertex[4] = {
                Vertex(xx, yyA, zz, cA.x, cA.y, cA.z, 0, 1, 0, 0, 0),
                Vertex(xx + sx, yyB, zz, cB.x, cB.y, cB.z, 0, 1, 0, 1, 0),
                Vertex(xx + sx, yyC, zz + sz, cC.x, cC.y, cC.z, 0, 1, 0, 1, 1),
                Vertex(xx, yyD, zz + sz, cD.x, cD.y, cD.z, 0, 1, 0, 0, 1)
            };
            
            glm::vec3 U = glm::vec3(vertex[2].x, vertex[2].y, vertex[2].z) - glm::vec3(vertex[0].x, vertex[0].y, vertex[0].z);
            glm::vec3 V = glm::vec3(vertex[1].x, vertex[1].y, vertex[1].z) - glm::vec3(vertex[0].x, vertex[0].y, vertex[0].z);
            glm::vec3 normal = glm::cross(U, V);
            
            for (int i = 0; i < 4; i++) {
                vertex[i].nx = normal.x;
                vertex[i].ny = normal.y;
                vertex[i].nz = normal.z;
            }
            
            SubMesh subBuffer;
            subBuffer.vertexBuffer.assign(vertex, vertex + 4);
            subBuffer.indexBuffer = {0, 2, 1, 0, 3, 2};
            
            mesh->AddSubMesh(xx, 0, zz, subBuffer.vertexBuffer, subBuffer.indexBuffer, false);
        }
        
    }
    
    return;
}

void ChunkManager::AddHeightFieldToMeshSimplified(Mesh* mesh, 
                                                  float* heightField, glm::vec3* colorField, 
                                                  unsigned int width, unsigned int height, 
                                                  float offsetX, float offsetZ, 
                                                  unsigned int simplifyFactor) {
    unsigned int sx = simplifyFactor;
    unsigned int sz = simplifyFactor;
    
    unsigned int fieldWidth = (width / 1) - sx;
    unsigned int fieldHeight = (height / 1) - sz;
    
    for (unsigned int x = 0; x < fieldWidth; x+=sx) {
        
        for (unsigned int z = 0; z < fieldHeight; z+=sz) {
            
            unsigned int xa = x * 1;
            unsigned int za = z * 1;
            
            float yyA = heightField[za * width + xa];
            float yyB = heightField[za * width + (xa + sx)];
            float yyC = heightField[(za + sz) * width + (xa + sx)];
            float yyD = heightField[(za + sz) * width + xa];
            
            glm::vec3 cA = colorField[za * width + xa];
            glm::vec3 cB = colorField[za * width + xa];
            glm::vec3 cC = colorField[za * width + xa];
            glm::vec3 cD = colorField[za * width + xa];
            
            float xx = (((float)x + offsetX - (float)width / 2) / 2) + 0.25;
            float zz = (((float)z + offsetZ - (float)height / 2) / 2) + 0.25;
            
            Vertex vertex[4] = {
                Vertex(xx, yyA, zz, cA.x, cA.y, cA.z, 0, 1, 0, 0, 0),
                Vertex(xx + sx, yyB, zz, cB.x, cB.y, cB.z, 0, 1, 0, 1, 0),
                Vertex(xx + sx, yyC, zz + sz, cC.x, cC.y, cC.z, 0, 1, 0, 1, 1),
                Vertex(xx, yyD, zz + sz, cD.x, cD.y, cD.z, 0, 1, 0, 0, 1)
            };
            
            glm::vec3 U = glm::vec3(vertex[2].x, vertex[2].y, vertex[2].z) - glm::vec3(vertex[0].x, vertex[0].y, vertex[0].z);
            glm::vec3 V = glm::vec3(vertex[1].x, vertex[1].y, vertex[1].z) - glm::vec3(vertex[0].x, vertex[0].y, vertex[0].z);
            glm::vec3 normal = glm::cross(U, V);
            
            for (int i = 0; i < 4; i++) {
                vertex[i].nx = normal.x;
                vertex[i].ny = normal.y;
                vertex[i].nz = normal.z;
            }
            
            SubMesh subBuffer;
            subBuffer.vertexBuffer.assign(vertex, vertex + 4);
            subBuffer.indexBuffer = {0, 2, 1, 0, 3, 2};
            
            mesh->AddSubMesh(xx, 0, zz, subBuffer.vertexBuffer, subBuffer.indexBuffer, false);
        }
        
    }
    
    return;
}

void ChunkManager::AddHeightFieldToMeshReduced(Mesh* mesh, 
                                               float* heightField, 
                                               glm::vec3* colorField, 
                                               unsigned int width, unsigned int height, 
                                               float offsetX, float offsetZ, 
                                               unsigned int resolution) {
    
    int fieldWidth = width / resolution;
    int fieldHeight = height / resolution;
    
    for (int x = 0; x < fieldWidth; x++) {
        
        for (int z = 0; z < fieldHeight; z++) {
            int xa = x * resolution;
            int za = z * resolution;
            
            float yyA = heightField[za * width + xa];
            float yyB = heightField[za * width + (xa + resolution)];
            float yyC = heightField[(za + resolution) * width + (xa + resolution)];
            float yyD = heightField[(za + resolution) * width + xa];
            
            glm::vec3 cA = colorField[za * width + xa];
            glm::vec3 cB = colorField[za * width + (xa + resolution)];
            glm::vec3 cC = colorField[(za + resolution) * width + (xa + resolution)];
            glm::vec3 cD = colorField[(za + resolution) * width + xa];
            
            float xx = (((float)x * resolution + offsetX - (float)fieldWidth * resolution / 2) / 2) + 0.25;
            float zz = (((float)z * resolution + offsetZ - (float)fieldHeight * resolution / 2) / 2) + 0.25;
            
            Vertex vertex[4] = {
                Vertex(xx, yyA, zz, cA.x, cA.y, cA.z, 0, 1, 0, 0, 0),
                Vertex(xx + resolution, yyB, zz, cB.x, cB.y, cB.z, 0, 1, 0, 1, 0),
                Vertex(xx + resolution, yyC, zz + resolution, cC.x, cC.y, cC.z, 0, 1, 0, 1, 1),
                Vertex(xx, yyD, zz + resolution, cD.x, cD.y, cD.z, 0, 1, 0, 0, 1)
            };
            
            glm::vec3 U = glm::vec3(vertex[2].x, vertex[2].y, vertex[2].z) - glm::vec3(vertex[0].x, vertex[0].y, vertex[0].z);
            glm::vec3 V = glm::vec3(vertex[1].x, vertex[1].y, vertex[1].z) - glm::vec3(vertex[0].x, vertex[0].y, vertex[0].z);
            glm::vec3 normal = glm::cross(U, V);
            
            for (int i = 0; i < 4; i++) {
                vertex[i].nx = normal.x;
                vertex[i].ny = normal.y;
                vertex[i].nz = normal.z;
            }
            
            float scale = 1.0f;
            float scaleDv = 1.0f / 2.0f;
            
            SubMesh subBuffer;
            for (int i = 0; i < 4; i++) {
                vertex[i].x  = (vertex[i].x * scale) - scaleDv;
                vertex[i].y += (vertex[i].y) - scaleDv;
                vertex[i].z  = (vertex[i].z * scale) - scaleDv;
            }
            
            subBuffer.vertexBuffer.assign(vertex, vertex + 4);
            subBuffer.indexBuffer = {0, 2, 1, 0, 3, 2};
            
            mesh->AddSubMesh(xx, 0, zz, subBuffer.vertexBuffer, subBuffer.indexBuffer, false);
        }
        
    }
    
    return;
}
