#ifndef _DIRECTORY_SEARCH__
#define _DIRECTORY_SEARCH__

#include <GameEngineFramework/configuration.h>

#include <vector>
#include <string>


class ENGINE_API FileSystemDir {
    
public:
    
    std::vector<std::string> GetList(std::string path);
    
};

#endif
