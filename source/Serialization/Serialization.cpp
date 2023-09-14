#include <fstream>

#include "Serialization.h"



Serialization::Serialization(void) {
    
}


/// Serialize data out to a file.
bool Serialization::Serialize(std::string filename, void* buffer, unsigned int size) {
    std::ofstream fStream(filename, std::fstream::out | std::fstream::binary);
    
    if (!fStream.is_open()) 
        return false;
    
    fStream.write( (char*)buffer, size);
    
    fStream.close();
    return true;
}


/// Deserialize data in from a file.
bool Serialization::Deserialize(std::string filename, void* buffer, unsigned int size) {
    std::ifstream fStream(filename, std::fstream::in | std::fstream::binary);
    
    if (!fStream.is_open()) 
        return false;
    
    fStream.read((char*)buffer, size);
    
    fStream.close();
    return true;
}

