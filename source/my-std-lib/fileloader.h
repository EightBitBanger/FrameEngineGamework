#ifndef _FILE_LOADER_SUPPORT__
#define _FILE_LOADER_SUPPORT__

#include <fstream>
#include <vector>
#include <map>

/*
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


class FileLoader {
    
public:
    
    /// Array of raw data strings from a loaded file.
    std::vector<std::string> rawData;
    
    /// Array of name / value pairs from a loaded file.
    std::map<std::string, std::vector<std::string>> assetData;
    
    /// Array of name / data_block pairs from a loaded file.
    std::map<std::string, std::string> dataBlocks;
    
    FileLoader(std::string FileName);
    
    /// Returns whether a file was loaded successfully.
    bool CheckIsFileLoaded(void) {return this ->isFileLoaded;}
    
    /// Returns a value of the index location from the name.
    std::string GetValueByName(std::string Name, unsigned int Index);
    
    /// Returns a data block of the index location from the name.
    std::string GetDataBlockByName(std::string Name);
    
    
private:
    
    // State of the loaded file
    bool isFileLoaded;
    
};

#endif
