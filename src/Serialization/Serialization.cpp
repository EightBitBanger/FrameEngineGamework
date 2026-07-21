#include <GameEngineFramework/Serialization/Serialization.h>

#include <fstream>


bool Serialization::Serialize(std::string filename, void* buffer, unsigned int size) {
    std::ofstream fStream(filename, std::fstream::out | std::fstream::binary);
    
    if (!fStream.is_open()) {
        
        fStream.close();
        
        return false;
    }
    
    fStream.write( (char*)buffer, size);
    
    fStream.close();
    return true;
}


bool Serialization::Deserialize(std::string filename, void* buffer, unsigned int size) {
    std::ifstream fStream(filename, std::fstream::in | std::fstream::binary);
    if (!fStream.is_open()) {
        fStream.close();
        return false;
    }
    
    fStream.read((char*)buffer, size);
    fStream.close();
    return true;
}


unsigned int Serialization::GetFileSize(std::string filename) {
    std::ifstream fStream(filename, std::ios::binary);
    if (!fStream.is_open()) {
        fStream.close();
        return 0;
    }
    
    fStream.seekg( 0, std::ios::end );
    std::streamsize size = fStream.tellg();
    
    fStream.close();
    return size;
}

bool Serialization::CheckExists(std::string filename) {
    std::ifstream fStream(filename);
    if (!fStream.is_open()) {
        fStream.close();
        return 0;
    }
    fStream.close();
    return true;
}

