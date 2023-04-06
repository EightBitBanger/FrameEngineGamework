#ifndef _FILE_LOADER_SUPPORT__
#define _FILE_LOADER_SUPPORT__

#include <fstream>
#include <vector>
#include <map>

/// General purpose file loading class.
class FileLoader {
    
    /** 
    The asset file`s header should be defined
    in the following way:
    
    define_asset YourAssetName
    
    Data/value pairs can be specified by
    the following layout:
    
    Name Value
    
    Data blocks are specified
    by using the following syntax:
    
    [begin] YourDataBlockName
    
    Data...
    
    [end]
    
    */
    
    // State of the file
    bool isFileLoaded;
    
    public:
    
    // Pair of names and values
    std::map<std::string, std::vector<std::string>> assetData;
    
    // Pair of names and data blocks
    std::map<std::string, std::string> dataBlocks;
    
    FileLoader(std::string FileName);
    
    bool CheckIsFileLoaded(void) {return this ->isFileLoaded;}
    
    std::string GetValueByName(std::string Name, unsigned int Index);
    std::string GetDataBlockByName(std::string Name);
    
};

FileLoader::FileLoader(std::string FileName) 
: isFileLoaded(false)
{
    
    std::fstream FileStream;
    
    this ->assetData.clear();
    this ->dataBlocks.clear();
    
    // Load the file data
    FileStream.open( FileName );
    if (FileStream.is_open()) {
        
        std::string String;
        
        // Load the data from file
        while ( getline(FileStream, String) ) {
            
            if (String == "") continue;
            if (String.find("//") == 0) continue;
            
            // Split string by spaces
            std::vector<std::string> ArrayData = StringExplode(String, ' ');
            int MaxSz = ArrayData.size();
            if (MaxSz < 1) continue;
            
            // Check data block
            if (ArrayData[0] == "[begin]") {
                
                // Get block name
                std::string BlockName = ArrayData[1];
                std::string BlockString = "";
                
                for (int i=0; i < 512; i++) {
                    
                    getline(FileStream, String);
                    if (String == "[end]") break;
                    
                    BlockString += String + "\n";
                    
                }
                
                this ->dataBlocks.emplace(BlockName, BlockString);
            }
            
            std::vector<std::string> ValueList;
            
            for (int i=1; i < MaxSz; i++) {
                
                ValueList.push_back(ArrayData[i]);
                
            }
            
            this ->assetData.insert( std::pair<std::string, std::vector<std::string>>(ArrayData[0], ValueList) );
            
        }
        
        FileStream.close();
        isFileLoaded = true;
    }
    
}

std::string FileLoader::GetValueByName(std::string Name, unsigned int Index) {
    
    for (std::map<std::string, std::vector<std::string>>::iterator it = assetData.begin(); it != assetData.end(); it++) {
        
        if (it ->first == Name) {
            
            std::vector<std::string> ValueList = it ->second;
            if (ValueList.size() == 0) return "";
            
            if (ValueList.size() < Index) return "";
            
            return ValueList[Index];
            
        }
        
    }
    
    return "";
}
std::string FileLoader::GetDataBlockByName(std::string Name) {
    
    for (std::map<std::string, std::string>::iterator it = dataBlocks.begin(); it != dataBlocks.end(); it++) {
        
        if (it ->first == Name) {
            
            return it ->second;
        }
        
    }
    
    return "";
}

#endif
