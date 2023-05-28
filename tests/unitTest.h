//
// Application unit tests
#ifndef APPLICATION_UNIT_TESTS
#define APPLICATION_UNIT_TESTS


class ApplicationTest {
    
public:
    
    bool hasTestFailed;
    
    ApplicationTest();
    
    void Initiate(void);
    void Complete(void);
    
    void RunTestSuite(void);
    
    // Test suite
    void TestGameObject(void);
    void TestComponentObject(void);
    void TestEngineFunctionality(void);
    
    void TestRenderer(void);
    
    
private:
    
    const std::string msgPassed = "PASS";
    const std::string msgFailed = "FAIL\n";
    
    std::string mLogString;
    
};

#endif
