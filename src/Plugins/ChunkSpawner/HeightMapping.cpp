#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>
void HeightMapping::ClearColorAdditive(glm::vec3* colorAdditive, unsigned int width, unsigned int height, const glm::vec3& clearColor) {
    if (colorAdditive == nullptr) return;
    unsigned int size = width * height;
    for (unsigned int i = 0; i < size; i++) {
        colorAdditive[i] = clearColor;
    }
}

void HeightMapping::BlendColorFields(glm::vec3* outFinalColors, const glm::vec3* baseColors, const glm::vec3* additiveColors, unsigned int size, bool multiply) {
    for (unsigned int i = 0; i < size; i++) {
        if (additiveColors == nullptr) {
            outFinalColors[i] = baseColors[i];
        } else if (multiply) {
            outFinalColors[i] = glm::clamp(baseColors[i] * additiveColors[i], 0.0f, 1.0f);
        } else {
            outFinalColors[i] = glm::clamp(baseColors[i] + additiveColors[i], 0.0f, 1.0f);
        }
    }
}

void HeightMapping::AddHeightFieldToMesh(Mesh* mesh, float* heightField, glm::vec3* colorField, glm::vec3* colorAdditive,
                                        unsigned int width, unsigned int height, float offsetX, float offsetZ, 
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
            if (colorAdditive != nullptr) {
                cA = glm::clamp(cA + colorAdditive[za * width + xa], 0.0f, 1.0f);
            }
            glm::vec3 cB = cA;
            glm::vec3 cC = cA;
            glm::vec3 cD = cA;
            
            float xx = (((float)x + offsetX - (float)width / 2.0f) / 2.0f) + 0.25f;
            float zz = (((float)z + offsetZ - (float)height / 2.0f) / 2.0f) + 0.25f;
            
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
}

void HeightMapping::ApplyColorAdditiveBrush(glm::vec3* colorAdditive, unsigned int width, unsigned int height, 
                                           float localX, float localZ, float radius, const glm::vec3& color, float intensity, bool additive) {
    if (colorAdditive == nullptr || radius <= 0.0f) return;
    
    int minX = glm::clamp((int)std::floor(localX - radius), 0, (int)width - 1);
    int maxX = glm::clamp((int)std::ceil(localX + radius), 0, (int)width - 1);
    int minZ = glm::clamp((int)std::floor(localZ - radius), 0, (int)height - 1);
    int maxZ = glm::clamp((int)std::ceil(localZ + radius), 0, (int)height - 1);
    
    for (int z = minZ; z <= maxZ; z++) {
        for (int x = minX; x <= maxX; x++) {
            float dist = glm::distance(glm::vec2((float)x, (float)z), glm::vec2(localX, localZ));
            if (dist <= radius) {
                float falloff = (1.0f - (dist / radius)) * intensity;
                unsigned int index = z * width + x;
                if (additive) {
                    colorAdditive[index] = glm::clamp(colorAdditive[index] + (color * falloff), -1.0f, 1.0f);
                } else {
                    colorAdditive[index] = glm::mix(colorAdditive[index], color, falloff);
                }
            }
        }
    }
}

void HeightMapping::FadeColorAdditive(glm::vec3* colorAdditive, unsigned int width, unsigned int height, float decayRate) {
    if (colorAdditive == nullptr) return;
    unsigned int size = width * height;
    for (unsigned int i = 0; i < size; i++) {
        colorAdditive[i] = glm::mix(colorAdditive[i], glm::vec3(0.0f), decayRate);
    }
}

void HeightMapping::SetColorFieldValues(glm::vec3* colorField, unsigned int width, unsigned int height, Color color, float noise) {
    glm::vec3 colorVec(color.r, color.g, color.b);
    unsigned int size = width * height;
    for (unsigned int i = 0; i < size; i++) {
        glm::vec3 additive = (Colors.MakeRandomGrayScale().ToVec3() * 0.5f + 1.0f) * 0.7f;
        colorField[i] = Math.Lerp(colorVec, additive, noise);
    }
}

void HeightMapping::GenerateColorFieldFromHeightField(glm::vec3* colorField, float* heightField, 
                                                     unsigned int width, unsigned int height, 
                                                     Color low, Color high, float bias, float beginHeight) {
    unsigned int size = width * height;
    
    for (unsigned int i = 0; i < size; i++) {
        float heightBias = (heightField[i] - beginHeight) * bias;
        heightBias = glm::clamp(heightBias, 0.0f, 1.0f);
        
        Color color = Colors.Lerp(low, high, heightBias);
        
        float uniformVariant = (Random.Range(0, 100) * 0.00001f) - (Random.Range(0, 10) * 0.00001f);
        
        color.r += uniformVariant;
        color.g += uniformVariant;
        color.b += uniformVariant;
        
        colorField[i] = glm::vec3(color.r, color.g, color.b);
    }
}

void HeightMapping::AddColorFieldSnowCap(glm::vec3* colorField, float* heightField, 
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
}

void HeightMapping::AddColorFieldWaterTable(glm::vec3* colorField, float* heightField, 
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
}

Mesh* HeightMapping::CreateMeshFromHeightField(float* heightField, glm::vec3* colorField, 
                                               unsigned int width, unsigned int height, 
                                               float offsetX, float offsetZ) {
    Mesh* mesh = Engine.Create<Mesh>();
    
    AddHeightFieldToMesh(mesh, heightField, colorField, width, height, offsetX, offsetZ);
    
    return mesh;
}

void HeightMapping::AddHeightStepToMesh(float* heightField, unsigned int width, unsigned int height) {
    unsigned int size = width * height;
    
    for (unsigned int i = 0; i < size; i++) 
        heightField[i] = Math.Round((heightField[i] * 10)) / 10;
}

void HeightMapping::AddHeightFieldToMesh(Mesh* mesh, float* heightField, glm::vec3* colorField, 
                                        unsigned int width, unsigned int height, float offsetX, float offsetZ, 
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
}

void HeightMapping::AddHeightFieldToMeshSimplified(Mesh* mesh, float* heightField, glm::vec3* colorField, 
                                                  unsigned int width, unsigned int height, float offsetX, float offsetZ, 
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
}

void HeightMapping::AddHeightFieldToMeshReduced(Mesh* mesh, float* heightField, glm::vec3* colorField, 
                                                unsigned int width, unsigned int height, float offsetX, float offsetZ, 
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
}
