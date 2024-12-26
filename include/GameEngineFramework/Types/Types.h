#ifndef _TYPE_CLASSES__
#define _TYPE_CLASSES__

#include <GameEngineFramework/configuration.h>

#include <string>
#include <vector>

class ENGINE_API StringType {
    
public:
    
    /// Return a float containing the numbers from text.
    float ToFloat(std::string value);
    
    /// Return a double containing the numbers from text.
    double ToDouble(std::string value);
    
    /// Return an integer containing the numbers from text.
    int ToInt(std::string value);
    
    /// Return a long integer containing the numbers from text.
    long int ToLongInt(std::string value);
    
    /// Return an unsigned integer containing the numbers from text.
    unsigned int ToUint(std::string value);
    
    /// Return a long unsigned integer containing the numbers from text.
    unsigned long int ToLongUint(std::string value);
    
    /// Explode the string by the given delimiter into an array of strings.
    std::vector<std::string> Explode(std::string value, const char character);
    
    /// Return the filename from a file name.
    std::string GetNameFromFilename(std::string filename);
    
    /// Return the filename without an extension from a file name.
    std::string GetNameFromFilenameNoExt(std::string filename);
    
    /// Return the extension of the file name.
    std::string GetExtFromFilename(std::string filename);
    
    /// Return the path part of a file name.
    std::string GetPathFromFilename(std::string filename);
    
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

class ENGINE_API UintType {
    
public:
    
    /// Convert an unsigned integer to a string.
    std::string ToString(unsigned int value);
    
    /// Linearly interpolate between min and max via the bias factor.
    unsigned int Lerp(unsigned int min, unsigned int max, float bias);
    
};

#endif
