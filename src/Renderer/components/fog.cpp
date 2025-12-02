#include <GameEngineFramework/configuration.h>
#include <GameEngineFramework/Renderer/components/fog.h>
#include <GameEngineFramework/Engine/types/color.h>

Fog::Fog() : 
    fogActive(true),
    fogDensity(3),
    fogHeightCutoff(1000000),
    fogBegin(30),
    fogEnd(100),
    fogColorBegin{0, 0, 0, 0},
    fogColorEnd{0, 0, 0, 0} {}
