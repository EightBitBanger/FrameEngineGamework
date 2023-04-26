#include "logging.h"


void Logger::Write(std::string LogString) {
    std::ofstream  FileID;
    
    FileID.open(EVENT_LOG_FILENAME, std::ifstream::app);
    FileID << LogString << "\n";
    
    FileID.close();
    return;
}

void Logger::WriteLn(void) {
    std::ofstream  FileID;
    
    FileID.open(EVENT_LOG_FILENAME, std::ifstream::app);
    FileID << "\n";
    
    FileID.close();
    return;
}

void Logger::Clear(void) {
    
    std::ifstream  FileID;
    bool FileExists=false;
    
    FileID.open(EVENT_LOG_FILENAME, std::ifstream::in);
    if (FileID.good()) {FileExists=true;}
    FileID.close();
    
    if (FileExists) {remove(EVENT_LOG_FILENAME);}
    return;
}
