#include <iostream>
#include <string>

#include "unitTest.h"


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

void ApplicationTest::RunTestSuite(void) {
    TestEngineFunctionality();
    TestRenderer();
    TestGameObject();
    TestComponentObject();
}

