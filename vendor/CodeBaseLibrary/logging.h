#ifndef EVENT_LOGGING
#define EVENT_LOGGING

#include <string>
#include <fstream>

#define  EVENT_LOG_FILENAME   "events.txt"

class __declspec(dllexport) Logger {
    
public:
    
    /// Write a string to the log file.
    void Write(std::string LogString);
    
    /// Write a blank line to the log file.
    void WriteLn(void);
    
    /// Delete the log file.
    void Clear(void);
    
};

#endif
