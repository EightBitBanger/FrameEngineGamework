//
// Application test framework
#ifndef APPLICATION_TEST_FRAMEWORK
#define APPLICATION_TEST_FRAMEWORK


class ApplicationTest {
    
public:
    
    ApplicationTest();
    
    void Initiate(void);
    void Complete(void);
    
    // Test suite
    
    void TestGameObject(void);
    void TestComponentObject(void);
    void TestEngineFunctionality(void);
    
    void TestRenderer(void);
    
    
private:
    
    std::string mLogString;
    
    const std::string msgPassed = "PASS";
    const std::string msgFailed = "FAIL";
    
};

#endif
