#include <GameEngineFramework/Plugins/WeatherSystem/components/Weather.h>

Weather::Weather() : 
    fogDensity(20.0f),
    fogNear(100.0f),
    fogFar(8000.0f),
    fogColorNear(Colors.ltgray * 0.7f),
    fogColorFar(Colors.Lerp(Colors.blue, Colors.gray, 0.8f))
{}
