#include <GameEngineFramework/Types/types.h>
#include <GameEngineFramework/Math/Math.h>

#include <algorithm>
#include <sstream>

std::string StringType::Copy(std::string source, unsigned int begin, unsigned int count) {
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

std::string StringType::Trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos) 
        return ""; // string is all whitespace
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, (last - first + 1));
}

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


bool StringType::IsFloat(const std::string& str) {
    std::string trimmed = Trim(str);
    if (trimmed.empty()) 
        return false;
    char* endPtr = nullptr;
    std::strtof(trimmed.c_str(), &endPtr);
    return (*endPtr == '\0' && endPtr != trimmed.c_str());
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

bool StringType::IsNumeric(const std::string& str) {
    return std::all_of(str.begin(), str.end(), [](char c) { return std::isdigit(c); });
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

std::string StringType::RemoveWhiteSpace(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    return str;
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
