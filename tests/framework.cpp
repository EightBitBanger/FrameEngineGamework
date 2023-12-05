#include "framework.h"


TestFramework::TestFramework() : 
    hasTestFailed(false),
    mLogString("")
{}

void TestFramework::Initiate(void) {
    std::cout << "Running unit tests\n\n";
}

void TestFramework::Complete(void) {
    std::cout << "\nComplete\n\n";
}

void TestFramework::Finalize(void) {
    if (mLogString != "") {
        std::cout  << msgFailed << "\n";
        std::cout << mLogString << "\n";
        mLogString="";
    } else {
        std::cout << msgPassed << "\n";
    }
}

void TestFramework::Throw(std::string message, std::string sourceFile, int line) {
    
    std::string sourceFileName = StringGetNameFromFilename(sourceFile);
    
    mLogString += sourceFileName + "\nLine " + IntToString((int)line) + " - " + message + "\n\n";
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

