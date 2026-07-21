#include <GameEngineFramework/Resources/FileSystem.h>

#include <fstream>

#include <sys/stat.h>
#include <dirent.h>


std::vector<std::string> FileSystem::DirectoryGetList(std::string directoryName) {
    DIR* dir;
    struct dirent* ent;
    std::vector<std::string> directoryList;
    
    if ((dir = opendir( directoryName.c_str() )) == NULL) 
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

bool FileSystem::DirectoryCreate(std::string directoryName) {
    if (mkdir(directoryName.c_str()) == 0) 
        return true;
    return false;
}

bool FileSystem::DirectoryDelete(std::string directoryName) {
    std::vector<std::string> fileList = DirectoryGetList(directoryName);
    for (unsigned int i=0; i < fileList.size(); i++) {
        std::string filename = directoryName + "/" + fileList[i];
        FileDelete( filename );
        continue;
    }
    if (rmdir(directoryName.c_str()) == 0) 
        return true;
    return false;
}

bool FileSystem::DirectoryExists(std::string directoryName) {
    DIR* dir = opendir(directoryName.c_str());
    if (dir) {
        closedir(dir);
        return true;
    } else if (ENOENT == errno) {
        return false;
    }
    return false;
}

bool FileSystem::FileCreate(std::string filename, unsigned int size) {
    std::ofstream fStream(filename, std::fstream::out | std::fstream::binary);
    if (!fStream.is_open()) {
        fStream.close();
        return false;
    }
    
    char buffer[ size ];
    for (unsigned int i=0; i < size; i++) 
        buffer[i] = '\n';
    
    fStream.write( (char*)buffer, size);
    fStream.close();
    return true;
}

bool FileSystem::FileExists(std::string filename) {
    std::ofstream fStream(filename, std::fstream::in | std::fstream::binary);
    if (!fStream.is_open()) {
        fStream.close();
        return false;
    }
    fStream.close();
    return true;
}

bool FileSystem::FileDelete(std::string filename) {
    if (remove( filename.c_str() ) == 0) 
        return true;
    return false;
}

