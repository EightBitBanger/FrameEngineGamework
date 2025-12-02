#include <GameEngineFramework/Resources/assets/textureTag.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Types/Types.h>

#define  STB_IMAGE_IMPLEMENTATION
//#define  STBI_ONLY_JPEG
#define  STBI_ONLY_PNG
#define  STBI_ONLY_BMP
//#define  STBI_ONLY_PSD
#define  STBI_ONLY_TGA
#define  STBI_ONLY_GIF
//#define  STBI_ONLY_HDR
#define  STBI_ONLY_PIC
//#define  STBI_ONLY_PNM // (.ppm and .pgm)

#include "../../../vendor/stb/stb_image.h"

extern Logger Log;
extern IntType Int;


TextureTag::TextureTag() : 
    path(""),
    name(""),
    isLoaded(false),
    filtration(MATERIAL_FILTER_LINEAR),
    width(0),
    height(0),
    channels(0),
    buffer(nullptr)
{
    stbi_set_flip_vertically_on_load(true);
}

bool TextureTag::Load(void) {
    if (isLoaded) 
        Unload();
    
    buffer = stbi_load(path.c_str(), &width, &height, &channels, 0);
    
    if (buffer == nullptr) 
        return false;
    
#ifdef EVENT_LOG_DETAILED
    std::string logstr = "  + " + name + " " + Int.ToString(width) + " X " + Int.ToString(height);
    Log.Write(logstr);
#endif
    
    isLoaded = true;
    return true;
}

bool TextureTag::Unload(void) {
    if (!isLoaded) 
        return false;
    stbi_image_free( buffer );
    isLoaded = false;
    return true;
}
