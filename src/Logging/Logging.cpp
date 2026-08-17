#include <GameEngineFramework/Logging/Logging.h>

#include <fstream>
#include <cstdio>

Logger::Logger() {
    m_workerThread = std::thread(&Logger::ProcessQueue, this);
}

void Logger::Shutdown(void) {
    m_running = false;
    m_cv.notify_one();
    
    if (m_workerThread.joinable()) {
        m_workerThread.join();
    }
}

void Logger::Write(std::string LogString) {
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_logQueue.push(std::move(LogString));
    }
    m_cv.notify_one();
}

void Logger::WriteLn(void) {
    Write("");
}

void Logger::Clear(void) {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    
    // Clear pending queued logs
    std::queue<std::string> emptyQueue;
    std::swap(m_logQueue, emptyQueue);
    
    std::ifstream FileID(EVENT_LOG_FILENAME, std::ifstream::in);
    bool FileExists = FileID.good();
    FileID.close();
    
    if (FileExists) {
        std::remove(EVENT_LOG_FILENAME);
    }
}

void Logger::ProcessQueue() {
    while (true) {
        std::queue<std::string> localQueue;
        
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            
            // Sleep until logs arrive or shutdown is requested
            m_cv.wait(lock, [this] { 
                return !m_running || !m_logQueue.empty(); 
            });
            
            if (!m_running && m_logQueue.empty()) {
                break;
            }
            std::swap(m_logQueue, localQueue);
        }
        
        if (!localQueue.empty()) {
            std::ofstream FileID(EVENT_LOG_FILENAME, std::ios::app);
            if (FileID.is_open()) {
                while (!localQueue.empty()) {
                    FileID << localQueue.front() << "\n";
                    localQueue.pop();
                }
                FileID.close();
            }
        }
    }
}
