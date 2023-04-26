#ifndef EVENT_LOGGING
#define EVENT_LOGGING

#include <string>
#include <fstream>

#define  EVENT_LOG_FILENAME   "events.txt"

class Logger {
    
public:
    
    void Write(std::string LogString);
    void WriteLn(void);
    void Clear(void);
    
};

#endif
