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
    using the following syntax:
    
    [begin] YourDataBlockName
    
    Data...
    
    [end]
    
    */
    
    // State of the file
    bool isFileLoaded;
    
public:
    
    // Raw file data
    std::vector<std::string> rawData;
    
    // Pair of names and values
    std::map<std::string, std::vector<std::string>> assetData;
    
    // Pair of names and data blocks
    std::map<std::string, std::string> dataBlocks;
    
    FileLoader(std::string FileName);
    
    bool CheckIsFileLoaded(void) {return this ->isFileLoaded;}
    
    std::string GetValueByName(std::string Name, unsigned int Index);
    std::string GetDataBlockByName(std::string Name);
    
};

#endif
