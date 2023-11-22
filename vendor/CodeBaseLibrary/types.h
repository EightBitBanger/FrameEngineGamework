#ifndef TYPE_CONVERSION
#define TYPE_CONVERSION

// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>

#include <sstream>

#include <string>

/// Return a string containing the number in the float.
__declspec(dllexport) std::string FloatToString(float value);

/// Return a float containing the number in the string.
__declspec(dllexport) float StringToFloat(std::string str);

/// Return a string containing the number in the double.
__declspec(dllexport) std::string DoubleToString(double value);

/// Return a double containing the number in the string.
__declspec(dllexport) double StringToDouble(std::string str);

/// Return a string containing the number in the integer.
__declspec(dllexport) std::string IntToString(int value);

/// Return an integer containing the number in the string.
__declspec(dllexport) int StringToInt(std::string str);

#endif
