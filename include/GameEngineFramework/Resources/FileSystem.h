#ifndef _DIRECTORY_SEARCH__
#define _DIRECTORY_SEARCH__

#include <GameEngineFramework/configuration.h>

#include <vector>
#include <string>


class ENGINE_API FileSystemDir {
    
public:
    
    std::vector<std::string> List(std::string path);
    
    bool Create(std::string directoryName);
    
    bool Delete(std::string directoryName);
    
    bool CheckExists(std::string directoryName);
    
};

#endif
