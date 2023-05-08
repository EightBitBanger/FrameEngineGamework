#ifndef TYPE_CONVERSION
#define TYPE_CONVERSION

//#define _WIN32_WINNT  0x500
#include <SDKDDKVer.h>
#include <windows.h>

// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>

#include <sstream>

#include <string>

std::string FloatToString(float Float);

float StringToFloat(std::string String);

std::string DoubleToString(double Double);

double StringToDouble(std::string String);

std::string IntToString(int Integer);

int StringToInt(std::string String);

#endif
