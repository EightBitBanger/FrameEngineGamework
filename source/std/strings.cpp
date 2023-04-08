#include "strings.h"

/** Explodes the string by the given delimiter.*/
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

/** Find a substring within a given string.*/
bool  StringFind(std::string SubString, std::string String) {
    if (strstr ( String.c_str(), SubString.c_str() ) != NULL) {return 1;}
    return 0;
}

/** Returns the file name from a path string.*/
std::string StringGetNameFromFilename(std::string FilePath) {
    std::vector<std::string> pathParts = StringExplode(FilePath, '/');
    return pathParts[pathParts.size()-1];
}

/** Returns the file name without an extension from a path string.*/
std::string StringGetNameFromFilenameNoExt(std::string FilePath) {
    std::vector<std::string> pathParts = StringExplode(FilePath, '/');
    std::vector<std::string> name = StringExplode(pathParts[pathParts.size()-1], '.');
    return name[0];
}

/** Returns the file extension only.*/
std::string StringGetExtFromFilename(std::string FilePath) {
    std::vector<std::string> pathParts = StringExplode(FilePath, '/');
    std::vector<std::string> name = StringExplode(pathParts[pathParts.size()-1], '.');
    return name[1];
}

