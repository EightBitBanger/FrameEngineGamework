//
// Application unit tests
#ifndef APPLICATION_UNIT_TESTS
#define APPLICATION_UNIT_TESTS

#include <iostream>
#include <string>
#include <vector>

#include "../vendor/CodeBaseLibrary/types.h"
#include "../vendor/CodeBaseLibrary/strings.h"


class __declspec(dllexport) TestFramework {
    
public:
    
    bool hasTestFailed;
    
    TestFramework();
    
    /// Initiate the unit testing framework
    void Initiate(void);
    /// Signal to complete after all tests have passed.
    void Complete(void);
    
    /// Start the suite of unit tests.
    void RunTestSuite(void);
    
    /// Finalize a unit test after it has finished before the next has started.
    void Finalize(void);
    
    /// Log a message string to the output console.
    void Throw(std::string message, std::string sourceFile, int line);
    
    
    //
    // Test suite
    //
    
    void AddTest(void(TestFramework::*testFunction)());
    
    void TestEngineFunctionality(void);
    void TestGameObject(void);
    void TestComponentObject(void);
    void TestSerializerSystem(void);
    void TestRenderSystem(void);
    void TestScriptSystem(void);
    void TestPhysicsSystem(void);
    void TestTransform(void);
    
private:
    
    const std::string msgPassed = "PASS";
    const std::string msgFailed = "FAIL\n";
    
    const std::string msgFailedNullptr             = "object pointer is null";
    const std::string msgFailedConstructor         = "failed to construct an object instance";
    const std::string msgFailedObjectCreate        = "failed to create an object";
    const std::string msgFailedObjectDestroy       = "failed to destroy an object";
    const std::string msgFailedToAttachComponent   = "component not attaching to game object";
    const std::string msgFailedToDetachComponent   = "component not detaching from game object";
    const std::string msgFailedToDestroyAttachment = "attachment not destroying when parent is destroyed";
    const std::string msgFailedAllocatorNotZero    = "object allocator still contains objects";
    const std::string msgFailedSetGet              = "set/get not returning correct value";
    const std::string msgFailedOperator            = "operator failed to operate";
    const std::string msgFailedSerialization       = "serialization failed";
    
    std::string mLogString;
    
    std::vector<void(TestFramework::*)()> mTestList;
    
};

#endif
