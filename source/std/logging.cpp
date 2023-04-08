//#define _WIN32_WINNT  0x500
#include <SDKDDKVer.h>
#include <windows.h>

// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <vector>
#include <thread>
#include <map>

#include "strings.h"
#include "types.h"
#include "logging.h"


void Logger::Write(std::string LogString) {
    std::ofstream  FileID;
    
    FileID.open(EVENT_LOG_FILENAME, std::ifstream::app);
    FileID << LogString << "\n";
    
    FileID.close();
}

void Logger::WriteLn(void) {
    std::ofstream  FileID;
    
    FileID.open(EVENT_LOG_FILENAME, std::ifstream::app);
    FileID << "\n";
    
    FileID.close();
}

void Logger::Clear(void) {
    
    std::ifstream  FileID;
    bool FileExists=false;
    
    FileID.open(EVENT_LOG_FILENAME, std::ifstream::in);
    if (FileID.good()) {FileExists=true;}
    FileID.close();
    
    if (FileExists) {remove(EVENT_LOG_FILENAME.c_str());}
    
}

