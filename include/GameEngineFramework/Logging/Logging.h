#ifndef EVENT_LOGGING
#define EVENT_LOGGING

#include <GameEngineFramework/configuration.h>

#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#define EVENT_LOG_FILENAME "events.txt"

class ENGINE_API Logger {
public:
    Logger();
    
    void Shutdown(void);
    
    /// Write a string to the log queue (non-blocking file I/O).
    void Write(std::string LogString);
    
    /// Write a blank line to the log queue.
    void WriteLn(void);
    
    /// Delete the log file and clear pending log entries.
    void Clear(void);
    
private:
    /// Worker function running on the background thread.
    void ProcessQueue();
    
    std::queue<std::string> m_logQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_cv;
    std::thread m_workerThread;
    std::atomic<bool> m_running{true};
};

#endif
