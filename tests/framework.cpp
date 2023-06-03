#include <iostream>
#include <string>

typedef void(*TestFunc)();

#include "framework.h"


ApplicationTest::ApplicationTest() : 
    hasTestFailed(false),
    mLogString("")
{}

void ApplicationTest::Initiate(void) {
    std::cout << "Running unit tests" << std::endl << std::endl;
}

void ApplicationTest::Complete(void) {
    std::cout << std::endl << "Complete" << std::endl << std::endl;
}

void ApplicationTest::Finalize(void) {
    if (mLogString != "") {
        std::cout  << msgFailed << std::endl;
        std::cout << mLogString << std::endl;
        mLogString="";
    } else {
        std::cout << msgPassed << std::endl;
    }
}

void ApplicationTest::RunTestSuite(void) {
    TestEngineFunctionality(); Finalize();
    
    TestGameObject(); Finalize();
    TestComponentObject(); Finalize();
    
    TestRenderSystem(); Finalize();
    TestScriptSystem(); Finalize();
}

