#include <GameEngineFramework/Types/types.h>
#include <GameEngineFramework/Math/Math.h>

#include <algorithm>
#include <sstream>

float StringType::ToFloat(const std::string& value) {
    float output;
    std::stringstream(value) >> output;
    return output;
}

double StringType::ToDouble(const std::string& value) {
    double output;
    std::stringstream(value) >> output;
    return output;
}

int StringType::ToInt(const std::string& value) {
    int output;
    std::stringstream(value) >> output;
    return output;
}

long int StringType::ToLongInt(const std::string& value) {
    long int output;
    std::stringstream(value) >> output;
    return output;
}

unsigned int StringType::ToUint(const std::string& value) {
    unsigned int output;
    std::stringstream(value) >> output;
    return output;
}

unsigned long int StringType::ToLongUint(const std::string& value) {
    unsigned long int output;
    std::stringstream(value) >> output;
    return output;
}

std::vector<std::string> StringType::Explode(const std::string& value, const char character) {
	std::vector<std::string> result;
    std::istringstream iss(value);
    
    for (std::string token; std::getline(iss, token, character); ) {
        
        if (std::move(token) == "") 
            continue;
        
        result.push_back(std::move(token));
    }
    return result;
}

std::string StringType::GetNameFromFilename(const std::string& filename) {
    std::vector<std::string> pathParts = Explode(filename, '/');
    return pathParts[pathParts.size()-1];
}

std::string StringType::GetNameFromFilenameNoExt(const std::string& filename) {
    std::vector<std::string> pathParts = Explode(filename, '/');
    std::vector<std::string> name = Explode(pathParts[pathParts.size()-1], '.');
    return name[0];
}

std::string StringType::GetExtFromFilename(const std::string& filename) {
    std::vector<std::string> pathParts = Explode(filename, '/');
    std::vector<std::string> name = Explode(pathParts[pathParts.size()-1], '.');
    return name[1];
}

std::string StringType::GetPathFromFilename(const std::string& filename) {
    std::vector<std::string> pathParts = Explode(filename, '/');
    std::string path;
    for (unsigned int i=0; i < pathParts.size()-1; i++) 
        path += pathParts[i];
    return path;
}

bool StringType::IsNumeric(const std::string& str) {
    return std::all_of(str.begin(), str.end(), [](char c) { return std::isdigit(c); });
}

void StringType::Lowercase(std::string& str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = tolower(str[i]);
    }
    return;
}

void StringType::Uppercase(std::string& str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper(str[i]);
    }
    return;
}


std::string FloatType::ToString(float value) {
    std::stringstream sstream;
    sstream << value;
    return sstream.str();
}

std::string DoubleType::ToString(double value) {
    std::stringstream sstream;
    sstream << value;
    return sstream.str();
}

std::string IntType::ToString(int value) {
    std::stringstream sstream;
    sstream << value;
    return sstream.str();
}

std::string IntLongType::ToString(long int value) {
    std::stringstream sstream;
    sstream << value;
    return sstream.str();
}

std::string UIntType::ToString(unsigned int value) {
    std::stringstream sstream;
    sstream << value;
    return sstream.str();
}

float FloatType::Lerp(float min, float max, float bias) {
    return glm::lerp(min, max, bias);
}

double DoubleType::Lerp(double min, double max, float bias) {
    return glm::lerp(min, max, (double)bias);
}

int IntType::Lerp(int min, int max, float bias) {
    return glm::lerp((float)min, (float)max, bias);
}

long int IntLongType::Lerp(long int min, long int max, float bias) {
    return glm::lerp((float)min, (float)max, bias);
}

unsigned int UIntType::Lerp(unsigned int min, unsigned int max, float bias) {
    return glm::lerp((float)min, (float)max, bias);
}
