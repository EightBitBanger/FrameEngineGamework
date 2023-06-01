//
// Resource loading plug-ins
#ifndef RESOURCE_PLUGINS
#define RESOURCE_PLUGINS


// Image file loader by STB-Image
//
#define  STB_IMAGE_IMPLEMENTATION
#define  STBI_ONLY_JPEG
#define  STBI_ONLY_PNG
#define  STBI_ONLY_BMP
#define  STBI_ONLY_PSD
#define  STBI_ONLY_TGA
#define  STBI_ONLY_GIF
#define  STBI_ONLY_HDR
#define  STBI_ONLY_PIC
#define  STBI_ONLY_PNM // (.ppm and .pgm)
#include "../../vendor/stb/stb_image.h"


// Wavefront file format loader by Bly7
//
#include "../../vendor/Bly7/OBJ_Loader.h"


#endif
