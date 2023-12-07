#ifndef EVENT_LOGGING
#define EVENT_LOGGING

#include <GameEngineFramework/configuration.h>

#include <string>

#define  EVENT_LOG_FILENAME   "events.txt"


class ENGINE_API Logger {
    
public:
    
    /// Write a string to the log file.
    void Write(std::string LogString);
    
    /// Write a blank line to the log file.
    void WriteLn(void);
    
    /// Delete the log file.
    void Clear(void);
    
};

#endif
