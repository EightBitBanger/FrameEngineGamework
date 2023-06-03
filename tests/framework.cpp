#include "framework.h"


TestFramework::TestFramework() : 
    hasTestFailed(false),
    mLogString("")
{}

void TestFramework::Initiate(void) {
    std::cout << "Running unit tests" << std::endl << std::endl;
}

void TestFramework::Complete(void) {
    std::cout << std::endl << "Complete" << std::endl << std::endl;
}

void TestFramework::Finalize(void) {
    if (mLogString != "") {
        std::cout  << msgFailed << std::endl;
        std::cout << mLogString << std::endl;
        mLogString="";
    } else {
        std::cout << msgPassed << std::endl;
    }
}

void TestFramework::AddTest(void(TestFramework::*testFunction)()) {
    mTestList.push_back(testFunction);
}

void TestFramework::RunTestSuite(void) {
    for (unsigned int i=0; i < mTestList.size(); i++) {
        void(TestFramework::*functionPtr)() = (void(TestFramework::*)())mTestList[i];
        (*this.*functionPtr)(); Finalize();
    }
}

