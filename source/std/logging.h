// Logging and message boxes

const std::string  EVENT_LOG_FILENAME = "events.txt";

struct Logger {
    
    void Write(std::string LogString);
    void WriteLn(void);
    void Clear(void);
    
};

