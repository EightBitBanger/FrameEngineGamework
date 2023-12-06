#include <GameEngineFramework/Types/types.h>

#include <sstream>

float StringType::ToFloat(std::string value) {
    float output;
    std::stringstream(value) >> output;
    return output;
}

double StringType::ToDouble(std::string value) {
    double output;
    std::stringstream(value) >> output;
    return output;
}

int StringType::ToInt(std::string value) {
    int output;
    std::stringstream(value) >> output;
    return output;
}

unsigned int StringType::ToUint(std::string value) {
    unsigned int output;
    std::stringstream(value) >> output;
    return output;
}

std::vector<std::string> StringType::Explode(std::string value, const char character) {
	std::vector<std::string> result;
    std::istringstream iss(value);
    
    for (std::string token; std::getline(iss, token, character); ) {
        
        if (std::move(token) == "") 
            continue;
        
        result.push_back(std::move(token));
    }
    return result;
}

std::string StringType::GetNameFromFilename(std::string filename) {
    std::vector<std::string> pathParts = Explode(filename, '/');
    return pathParts[pathParts.size()-1];
}

std::string StringType::GetNameFromFilenameNoExt(std::string filename) {
    std::vector<std::string> pathParts = Explode(filename, '/');
    std::vector<std::string> name = Explode(pathParts[pathParts.size()-1], '.');
    return name[0];
}

std::string StringType::GetExtFromFilename(std::string filename) {
    std::vector<std::string> pathParts = Explode(filename, '/');
    std::vector<std::string> name = Explode(pathParts[pathParts.size()-1], '.');
    return name[1];
}

std::string StringType::GetPathFromFilename(std::string filename) {
    std::vector<std::string> pathParts = Explode(filename, '/');
    std::string path;
    for (unsigned int i=0; i < pathParts.size()-1; i++) 
        path += pathParts[i];
    return path;
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

std::string UintType::ToString(unsigned int value) {
    std::stringstream sstream;
    sstream << value;
    return sstream.str();
}

