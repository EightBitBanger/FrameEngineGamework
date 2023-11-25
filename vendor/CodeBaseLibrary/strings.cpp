#include "strings.h"


std::vector<std::string> StringExplode(std::string String, const char Char) {
	
	std::vector<std::string> result;
    std::istringstream iss(String);
    
    for (std::string token; std::getline(iss, token, Char); ) {
        
        if (std::move(token) != "") {
            result.push_back(std::move(token));
        }
        
    }
    
    return result;
}

std::string StringGetNameFromFilename(std::string FilePath) {
    std::vector<std::string> pathParts = StringExplode(FilePath, '/');
    return pathParts[pathParts.size()-1];
}

std::string StringGetNameFromFilenameNoExt(std::string FilePath) {
    std::vector<std::string> pathParts = StringExplode(FilePath, '/');
    std::vector<std::string> name = StringExplode(pathParts[pathParts.size()-1], '.');
    return name[0];
}

std::string StringGetExtFromFilename(std::string FilePath) {
    std::vector<std::string> pathParts = StringExplode(FilePath, '/');
    std::vector<std::string> name = StringExplode(pathParts[pathParts.size()-1], '.');
    return name[1];
}

std::string StringGetPathFromFilename(std::string FilePath) {
    std::vector<std::string> pathParts = StringExplode(FilePath, '/');
    std::string path;
    for (unsigned int i=0; i < pathParts.size()-1; i++) 
        path += pathParts[i];
    return path;
}
