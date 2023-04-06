//
// Logging and message boxes

const std::string  EVENT_LOG_FILENAME = "events.txt";

class Logger {
    
public:
    
    void Write(std::string LogString) {
        std::ofstream  FileID;
        
        FileID.open(EVENT_LOG_FILENAME, std::ifstream::app);
        FileID << LogString << "\n";
        
        FileID.close();
    }
    
    void WriteLn(void) {
        std::ofstream  FileID;
        
        FileID.open(EVENT_LOG_FILENAME, std::ifstream::app);
        FileID << "\n";
        
        FileID.close();
    }
    
    void Clear(void) {
        
        std::ifstream  FileID;
        bool FileExists=false;
        
        FileID.open(EVENT_LOG_FILENAME, std::ifstream::in);
        if (FileID.good()) {FileExists=true;}
        FileID.close();
        
        if (FileExists) {remove(EVENT_LOG_FILENAME.c_str());}
        
    }
    
    
};








//
// Windows message box

void Msg(std::string vText) {MessageBox(NULL, vText.c_str(), "Message box", MB_OK);}
void Msg(const char* vText) {MessageBox(NULL, vText, "Message box", MB_OK);}
void Msg(int vText) {
    std::string String = IntToString(vText);
    const char* ConstString = String.c_str();
    MessageBox(NULL, ConstString, "Message box", MB_OK);
}
void Msg(float vText) {
    std::string String = FloatToString(vText);
    const char* ConstString = String.c_str();
    MessageBox(NULL, ConstString, "Message box", MB_OK);
}
void Msg(double vText) {
    std::string String = DoubleToString(vText);
    const char* ConstString = String.c_str();
    MessageBox(NULL, ConstString, "Message box", MB_OK);
}

