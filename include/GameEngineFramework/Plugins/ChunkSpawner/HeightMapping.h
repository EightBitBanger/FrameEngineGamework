#ifndef __HEIGHT_MAPPING_
#define __HEIGHT_MAPPING_

class ENGINE_API HeightMapping {
public:
    
    // Color mapping
    
    /// Initiates a color field grid array of colors and applies a perlin noise layer to the color grid.
    void SetColorFieldValues(glm::vec3* colorField, unsigned int width, unsigned int height, Color color, float noise);
    
    /// Generate a color field containing a color range from from low to high. The bias will determine the fade 
    /// from the low color to the high color based on the height field values.
    void GenerateColorFieldFromHeightField(glm::vec3* colorField, float* heightField, unsigned int width, unsigned int height, Color low, Color high, float bias, float beginHeight);
    
    /// Generate a snow cap effect of a given color capColor and starting at the height beginHeight.
    /// The bias will determine how much snow will be added.
    void AddColorFieldSnowCap(glm::vec3* colorField, float* heightField, unsigned int width, unsigned int height, Color capColor, float beginHeight, float bias);
    
    /// Generate a water level effect.
    void AddColorFieldWaterTable(glm::vec3* colorField, float* heightField, unsigned int width, unsigned int height, Color waterColor, float beginHeight, float bias, float waterTableHeight);
    
    // Color effects
    
    /// Clears or resets a color additive field to zero.
    void ClearColorAdditive(glm::vec3* colorAdditive, unsigned int width, unsigned int height, const glm::vec3& clearColor = glm::vec3(0.0f));
    
    /// Blends base color with additive/multiplicative color offsets and stores in outFinalColors.
    void BlendColorFields(glm::vec3* outFinalColors, const glm::vec3* baseColors, const glm::vec3* additiveColors, unsigned int size, bool multiply = false);
    
    /// Applies height field and combined color fields to mesh quads.
    void AddHeightFieldToMesh(Mesh* mesh, float* heightField, glm::vec3* colorField, glm::vec3* colorAdditive, unsigned int width, unsigned int height, float offsetX, float offsetZ, unsigned int subTessX = 1, unsigned int subTessZ = 1);
    
    /// Modifies additive colors in a radius around a local grid point.
    void ApplyColorAdditiveBrush(glm::vec3* colorAdditive, unsigned int width, unsigned int height, float localX, float localZ, float radius, const glm::vec3& color, float intensity, bool additive = true);
    
    /// Fades all additive color values toward zero by a given decay rate.
    void FadeColorAdditive(glm::vec3* colorAdditive, unsigned int width, unsigned int height, float decayRate);
    
    // Mesh mapping
    
    /// Apply the height field values to a mesh.
    void AddHeightFieldToMesh(Mesh* mesh, float* heightField, glm::vec3* colorField, unsigned int width, unsigned int height, float offsetX, float offsetZ, unsigned int subTessX=1.0f, unsigned int subTessZ=1.0f);
    
    /// Apply a reduced quality version of the height field values to a mesh. This function will
    /// reduce the mesh by one half of the original size.
    void AddHeightFieldToMeshSimplified(Mesh* mesh, float* heightField, glm::vec3* colorField, unsigned int width, unsigned int height, float offsetX, float offsetZ, unsigned int simplifyFactor);
    
    void AddHeightFieldToMeshLOD(Mesh* mesh, float* heightField, glm::vec3* colorField, unsigned int width, unsigned int height, float offsetX, float offsetZ, unsigned int lodFactor);
    
    /// Apply the height field values to the mesh using a quality resolution value.
    void AddHeightFieldToMeshReduced(Mesh* mesh, float* heightField, glm::vec3* colorField, unsigned int width, unsigned int height, float offsetX, float offsetZ, unsigned int resolution);
    
    /// Generate a height field mesh from perlin noise.
    Mesh* CreateMeshFromHeightField(float* heightField, glm::vec3* colorField, unsigned int width, unsigned int height, float offsetX, float offsetZ);
    
    /// Apply a height stepping effect to the mesh.
    void AddHeightStepToMesh(float* heightField, unsigned int width, unsigned int height);
    
    /// Set a layer of perlin noise into a color field.
    void AddColorFieldFromPerlinNoise(glm::vec3* colorField, unsigned int width, unsigned int height, float noiseWidth, float noiseHeight, int seed, Color color, int offsetX, int offsetZ);
    
    // World generation
    
    /// Initiates a height field grid array of points and set them to zero.
    void SetHeightFieldValues(float* heightField, unsigned int width, unsigned int height, float value);
    
    /// Add a layer of perlin noise into a height field. The minimum 
    /// height value will be returned.
    float AddHeightFieldFromPerlinNoise(float* heightField, unsigned int width, unsigned int height, float noiseWidth, float noiseHeight, float noiseMul, int offsetX, int offsetZ, float heightThreshold, int seed);
    
    /// Smooth the terrain height starting at a given height level and moving downward.
    void GenerateWaterTableFromHeightField(float* heightField, unsigned int width, unsigned int height, float tableHeight);
    
};

#endif
