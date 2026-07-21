#ifndef _FILESYSTEM_FUNCTIONS__
#define _FILESYSTEM_FUNCTIONS__

#include <GameEngineFramework/configuration.h>

#include <vector>
#include <string>


class ENGINE_API FileSystem {
    
public:
    
    /// Return a vector array of filename strings.
    std::vector<std::string> DirectoryGetList(std::string directoryName);
    
    /// Create a directory.
    bool DirectoryCreate(std::string directoryName);
    
    /// Delete a directory.
    bool DirectoryDelete(std::string directoryName);
    
    /// Check if a directory exists.
    bool DirectoryExists(std::string directoryName);
    
    
    /// Create a new file with a given size.
    bool FileCreate(std::string filename, unsigned int size);
    
    /// Check if a file exists.
    bool FileExists(std::string filename);
    
    /// Delete a file.
    bool FileDelete(std::string filename);
    
};

#endif
