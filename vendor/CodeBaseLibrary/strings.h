#ifndef STRING_FUNCTIONS
#define STRING_FUNCTIONS

//#define _WIN32_WINNT  0x500
#include <SDKDDKVer.h>
#include <windows.h>

// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>

#include <sstream>

#include <string>
#include <vector>


std::vector<std::string> StringExplode(std::string String, const char Char);
bool  StringFind(std::string SubString, std::string String);

std::string StringGetNameFromFilename(std::string FilePath);
std::string StringGetNameFromFilenameNoExt(std::string FilePath);
std::string StringGetExtFromFilename(std::string FilePath);

#endif
