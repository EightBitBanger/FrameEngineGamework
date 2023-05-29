//
// Application unit tests
#ifndef APPLICATION_UNIT_TESTS
#define APPLICATION_UNIT_TESTS


class ApplicationTest {
    
public:
    
    bool hasTestFailed;
    
    ApplicationTest();
    
    /// Initiate the unit testing framework
    void Initiate(void);
    /// Signal to complete after all tests have passed.
    void Complete(void);
    
    /// Start the suite of unit tests.
    void RunTestSuite(void);
    
    /// Finalize a unit test after it has finished before the next has started.
    void Finalize(void);
    
    //
    // Test suite
    //
    
    // Test engine API
    void TestEngineFunctionality(void);
    void TestGameObject(void);
    void TestComponentObject(void);
    
    // Test lower level systems
    void TestRenderSystem(void);
    void TestScriptSystem(void);
    
    
private:
    
    const std::string msgPassed = "PASS";
    const std::string msgFailed = "FAIL\n";
    
    const std::string msgFailedNullptr            = "object pointer is null\n";
    const std::string msgFailedObjectCreate       = "failed to create an object\n";
    const std::string msgFailedObjectDestroy      = "failed to destroy an object\n";
    const std::string msgFailedToAttachComponent  = "component not attaching to game object\n";
    const std::string msgFailedToDetachComponent  = "component not detaching from game object\n";
    const std::string msgFailedAllocatorNotZero   = "object allocator still contains objects\n";
    
    std::string mLogString;
    
};

#endif
