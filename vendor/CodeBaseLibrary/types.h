#ifndef TYPE_CONVERSION
#define TYPE_CONVERSION

// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>

#include <sstream>

#include <string>

#include <GameEngineFramework/configuration.h>


/// Return a string containing the number in the float.
ENGINE_API std::string FloatToString(float value);

/// Return a float containing the number in the string.
ENGINE_API float StringToFloat(std::string str);

/// Return a string containing the number in the double.
ENGINE_API std::string DoubleToString(double value);

/// Return a double containing the number in the string.
ENGINE_API double StringToDouble(std::string str);

/// Return a string containing the number in the integer.
ENGINE_API std::string IntToString(int value);

/// Return an integer containing the number in the string.
ENGINE_API int StringToInt(std::string str);

#endif
