#ifndef _TYPE_CLASSES__
#define _TYPE_CLASSES__

#include <GameEngineFramework/configuration.h>

#include <string>
#include <vector>
#include <ctype.h>

class ENGINE_API StringType {
    
public:
    
    /// Return a float containing the numbers from text.
    float ToFloat(const std::string& value);
    
    /// Return a double containing the numbers from text.
    double ToDouble(const std::string& value);
    
    /// Return an integer containing the numbers from text.
    int ToInt(const std::string& value);
    
    /// Return a long integer containing the numbers from text.
    long int ToLongInt(const std::string& value);
    
    /// Return an unsigned integer containing the numbers from text.
    unsigned int ToUint(const std::string& value);
    
    /// Return a long unsigned integer containing the numbers from text.
    unsigned long int ToLongUint(const std::string& value);
    
    /// Explode the string by the given delimiter into an array of strings.
    std::vector<std::string> Explode(const std::string& value, const char character);
    
    /// Return the filename from a file name.
    std::string GetNameFromFilename(const std::string& filename);
    
    /// Return the filename without an extension from a file name.
    std::string GetNameFromFilenameNoExt(const std::string& filename);
    
    /// Return the extension of the file name.
    std::string GetExtFromFilename(const std::string& filename);
    
    /// Return the path part of a file name.
    std::string GetPathFromFilename(const std::string& filename);
    
    /// Check if a string contains only numbers.
    bool IsNumeric(const std::string& str);
    
    /// Convert a string to lower case letters
    void Lowercase(std::string& str);
    
    /// Convert a string to upper case letters
    void Uppercase(std::string& str);
    
};


class ENGINE_API FloatType {
    
public:
    
    /// Convert a float to a string.
    std::string ToString(float value);
    
    /// Linearly interpolate between min and max via the bias factor.
    float Lerp(float min, float max, float bias);
    
};

class ENGINE_API DoubleType {
    
public:
    
    /// Convert a double to a string.
    std::string ToString(double value);
    
    /// Linearly interpolate between min and max via the bias factor.
    double Lerp(double min, double max, float bias);
    
};

class ENGINE_API IntType {
    
public:
    
    /// Convert an integer to a string.
    std::string ToString(int value);
    
    /// Linearly interpolate between min and max via the bias factor.
    int Lerp(int min, int max, float bias);
    
};

class ENGINE_API IntLongType {
    
public:
    
    /// Convert a long integer to a string.
    std::string ToString(long int value);
    
    /// Linearly interpolate between min and max via the bias factor.
    long int Lerp(long int min, long int max, float bias);
    
};

class ENGINE_API UIntType {
    
public:
    
    /// Convert an unsigned integer to a string.
    std::string ToString(unsigned int value);
    
    /// Linearly interpolate between min and max via the bias factor.
    unsigned int Lerp(unsigned int min, unsigned int max, float bias);
    
};

#endif
