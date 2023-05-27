//
// Application test framework
#ifndef APPLICATION_TEST_FRAMEWORK
#define APPLICATION_TEST_FRAMEWORK


class ApplicationTest {
    
public:
    
    ApplicationTest();
    
    void Initiate(void);
    void Complete(void);
    
    void TestGameObject(void);
    
    int GetFailureCount(void);
    
    void LogFail(std::string message);
    
    
private:
    
    int mFailureCount;
    
};

#endif
