#ifndef _DIRECTORY_SEARCH__
#define _DIRECTORY_SEARCH__

#include <vector>
#include <string>

#include <GameEngineFramework/configuration.h>

class ENGINE_API FileSystemDir {
    
public:
    
    std::vector<std::string> GetList(std::string path);
    
};

#endif
