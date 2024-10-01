#include <GameEngineFramework/Resources/FileSystem.h>

#include <sys/stat.h>
#include <dirent.h>

std::vector<std::string> FileSystemDir::List(std::string path) {
    
    DIR* dir;
    struct dirent* ent;
    
    std::vector<std::string> directoryList;
    
    if ((dir = opendir( path.c_str() )) == NULL) 
        return directoryList;
    
    int index=0;
    while ((ent = readdir(dir)) != NULL) {
        if (index > 1) 
            directoryList.push_back( ent->d_name );
        index++;
    }
    
    closedir(dir);
    
    return directoryList;
}

bool FileSystemDir::Create(std::string directoryName) {
    
    if (mkdir(directoryName.c_str()) == 0) 
        return true;
    
    return false;
}

bool FileSystemDir::Delete(std::string directoryName) {
    
    if (rmdir(directoryName.c_str()) == 0) 
        return true;
    
    return false;
}

bool FileSystemDir::CheckExists(std::string directoryName) {
    
    DIR* dir = opendir(directoryName.c_str());
    
    if (dir) {
        closedir(dir);
        return true;
    } else if (ENOENT == errno) {
        return false;
    }
    
    return false;
}

