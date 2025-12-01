#ifndef __DECORATION_SPECIFIER_
#define __DECORATION_SPECIFIER_

class ENGINE_API DecorationSpecifier {
public:
    
    /// Decoration name.
    std::string name;
    
    /// Spawn rate factor.
    unsigned int density;
    
    /// Base generation color.
    Color colorLow;
    
    /// Fade to top generation color.
    Color colorHigh;
    
    /// Minimum generation height.
    float spawnHeightMinimum;
    
    /// Maximum generation height.
    float spawnHeightMaximum;
    
    /// Threshold to generate decoration from perlin noise.
    float threshold;
    
    /// Perlin noise multiplier.
    float noise;
    
    DecorationSpecifier() : 
        name(""),
        density(10),
        
        spawnHeightMinimum(0),
        spawnHeightMaximum(100),
        
        threshold(0.1f),
        noise(0.1f)
    {
    }
    
};

#endif
