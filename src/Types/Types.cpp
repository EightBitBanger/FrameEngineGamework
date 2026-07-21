#include <GameEngineFramework/Types/Types.h>
#include <GameEngineFramework/Math/Math.h>

#include <algorithm>
#include <sstream>

std::string StringType::Copy(const std::string& source, unsigned int begin, unsigned int count) {
    if (begin >= source.size()) {
        return "";
    }
    
    // Adjust count to avoid overflow
    if (begin + count > source.size()) 
        count = source.size() - begin;
    
    char buffer[count + 1];  // +1 for null terminator
    source.copy(buffer, count, begin);
    buffer[count] = '\0';
    
    std::string result(buffer);
    //delete[] buffer;
    return result;
}

std::string StringType::Trim(const std::string& source) {
    size_t first = source.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos) 
        return ""; // string is all whitespace
    size_t last = source.find_last_not_of(" \t\n\r\f\v");
    return source.substr(first, (last - first + 1));
}

float StringType::ToFloat(const std::string& source) {
    float output;
    std::stringstream(source) >> output;
    return output;
}

double StringType::ToDouble(const std::string& source) {
    double output;
    std::stringstream(source) >> output;
    return output;
}

int StringType::ToInt(const std::string& source) {
    int output;
    std::stringstream(source) >> output;
    return output;
}

long int StringType::ToLongInt(const std::string& source) {
    long int output;
    std::stringstream(source) >> output;
    return output;
}

unsigned int StringType::ToUint(const std::string& source) {
    unsigned int output;
    std::stringstream(source) >> output;
    return output;
}

unsigned long int StringType::ToLongUint(const std::string& source) {
    unsigned long int output;
    std::stringstream(source) >> output;
    return output;
}


bool StringType::IsFloat(const std::string& source) {
    std::string trimmed = Trim(source);
    if (trimmed.empty()) 
        return false;
    char* endPtr = nullptr;
    std::strtof(trimmed.c_str(), &endPtr);
    return (*endPtr == '\0' && endPtr != trimmed.c_str());
}


std::vector<std::string> StringType::Explode(const std::string& source, const char delimiter) {
	std::vector<std::string> result;
    std::istringstream iss(source);
    for (std::string token; std::getline(iss, token, delimiter); ) {
        if (std::move(token) == "") 
            continue;
        
        result.push_back(std::move(token));
    }
    return result;
}

std::string StringType::GetNameFromFilename(const std::string& filename) {
    std::vector<std::string> pathParts = Explode(filename, '/');
    if (pathParts.size() == 1) 
        pathParts = Explode(filename, '\\');
    return pathParts[pathParts.size()-1];
}

std::string StringType::GetNameFromFilenameNoExt(const std::string& filename) {
    std::vector<std::string> pathParts = Explode(filename, '/');
    if (pathParts.size() == 1) 
        pathParts = Explode(filename, '\\');
    std::vector<std::string> name = Explode(pathParts[pathParts.size()-1], '.');
    return name[0];
}

std::string StringType::GetExtFromFilename(const std::string& filename) {
    std::vector<std::string> pathParts = Explode(filename, '/');
    if (pathParts.size() == 1) 
        pathParts = Explode(filename, '\\');
    std::vector<std::string> name = Explode(pathParts[pathParts.size()-1], '.');
    return name[1];
}

std::string StringType::GetPathFromFilename(const std::string& filename) {
    std::vector<std::string> pathParts = Explode(filename, '/');
    if (pathParts.size() == 1) 
        pathParts = Explode(filename, '\\');
    std::string path;
    for (unsigned int i=0; i < pathParts.size()-1; i++) 
        path += pathParts[i];
    return path;
}

bool StringType::IsNumeric(const std::string& source) {
    return std::all_of(source.begin(), source.end(), [](char c) { return std::isdigit(c); });
}

void StringType::Lowercase(std::string& str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = tolower(str[i]);
    }
}

void StringType::Uppercase(std::string& str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper(str[i]);
    }
}

void StringType::RemoveWhiteSpace(std::string& str) {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
}

std::string FloatType::ToString(float source) {
    std::stringstream sstream;
    sstream << source;
    return sstream.str();
}

std::string DoubleType::ToString(double source) {
    std::stringstream sstream;
    sstream << source;
    return sstream.str();
}

std::string IntType::ToString(int source) {
    std::stringstream sstream;
    sstream << source;
    return sstream.str();
}

std::string IntLongType::ToString(long int source) {
    std::stringstream sstream;
    sstream << source;
    return sstream.str();
}

std::string UIntType::ToString(unsigned int source) {
    std::stringstream sstream;
    sstream << source;
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
