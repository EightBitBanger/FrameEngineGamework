#include <iostream>
#include <string>


#include "../source/Engine/Engine.h"
#include "../source/Application/ApplicationLayer.h"
#include "../source/Renderer/RenderSystem.h"
extern EngineSystemManager  Engine;
extern RenderSystem         Renderer;
extern ApplicationLayer     Application;

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
    TestGameObject();
    TestComponentObject();
    TestEngineFunctionality();
}

