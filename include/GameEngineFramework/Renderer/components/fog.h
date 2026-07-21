#ifndef _RENDER_FOG_LAYER_TYPE__
#define _RENDER_FOG_LAYER_TYPE__

#include <GameEngineFramework/Engine/types/color.h>

class ENGINE_API Fog {
    
public:
    
    /// Fog active state.
    bool fogActive;
    
    /// Thickness of the fog between begin and end.
    float fogDensity;
    
    /// Height cutoff of the fog.
    float fogHeightCutoff;
    
    /// Distance range to render the fog.
    float fogBegin;
    float fogEnd;
    
    /// Color fade of the fog in the world.
    Color fogColorBegin;
    Color fogColorEnd;
    
    Fog();
    
};

#endif
