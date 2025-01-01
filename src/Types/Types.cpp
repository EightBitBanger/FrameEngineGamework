#include <GameEngineFramework/Types/types.h>
#include <GameEngineFramework/Math/Math.h>

#include <algorithm>
#include <sstream>
#include <filesystem>

float StringType::ToFloat(const std::string& value) {
    return std::stof(value);
}

double StringType::ToDouble(const std::string& value) {
    return std::stod(value);
}

int StringType::ToInt(const std::string& value) {
    return std::stoi(value);
}

long int StringType::ToLongInt(const std::string& value) {
    return std::stol(value);
}

unsigned int StringType::ToUint(const std::string& value) {
    return std::stoul(value);
}

unsigned long int StringType::ToLongUint(const std::string& value) {
    return std::stoull(value);
}

std::vector<std::string> StringType::Explode(const std::string& value, char character) {
    std::vector<std::string> result;
    std::istringstream iss(value);
    for (std::string token; std::getline(iss, token, character); ) {
        if (!token.empty()) {
            result.push_back(token);
        }
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
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

void StringType::Lowercase(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

void StringType::Uppercase(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

std::string FloatType::ToString(float value) {
    return std::to_string(value);
}

std::string DoubleType::ToString(double value) {
    return std::to_string(value);
}

std::string IntType::ToString(int value) {
    return std::to_string(value);
}

std::string IntLongType::ToString(long int value) {
    return std::to_string(value);
}

std::string UintType::ToString(unsigned int value) {
    return std::to_string(value);
}

float FloatType::Lerp(float min, float max, float bias) {
    return glm::lerp(min, max, bias);
}

double DoubleType::Lerp(double min, double max, float bias) {
    return glm::lerp(min, max, static_cast<double>(bias));
}

int IntType::Lerp(int min, int max, float bias) {
    return static_cast<int>(glm::lerp(static_cast<float>(min), static_cast<float>(max), bias));
}

long int IntLongType::Lerp(long int min, long int max, float bias) {
    return static_cast<long int>(glm::lerp(static_cast<float>(min), static_cast<float>(max), bias));
}

unsigned int UintType::Lerp(unsigned int min, unsigned int max, float bias) {
    return static_cast<unsigned int>(glm::lerp(static_cast<float>(min), static_cast<float>(max), bias));
}

