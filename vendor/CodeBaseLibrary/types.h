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

/// Return a string containing the number in the float.
std::string FloatToString(float value);

/// Return a float containing the number in the string.
float StringToFloat(std::string str);

/// Return a string containing the number in the double.
std::string DoubleToString(double value);

/// Return a double containing the number in the string.
double StringToDouble(std::string str);

/// Return a string containing the number in the integer.
std::string IntToString(int value);

/// Return an integer containing the number in the string.
int StringToInt(std::string str);

#endif
