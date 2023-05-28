#include <iostream>
#include <string>

#include "../unitTest.h"
#include "../../source/Renderer/RenderSystem.h"
extern RenderSystem Renderer;


void ApplicationTest::TestRenderer(void) {
    if (hasTestFailed) return;
    
    std::string msgFailedObjectCreation    = "Render object failing to create an object\n";
    std::string msgFailedObjectDestruction = "Render object failing to destroy an object\n";
    std::string msgFailedObjectListNotZero = "Render object list does not contain ZERO objects\n";
    
    std::cout << "Render system........... ";
    
    Entity* entityPtr = Renderer.CreateEntity();
    // Check entity was created
    if (entityPtr == nullptr) mLogString += msgFailedObjectCreation;
    // Check entity was destroyed
    if (!Renderer.DestroyEntity(entityPtr)) mLogString += msgFailedObjectDestruction;
    // Check entity was not left over
    if (Renderer.GetEntityCount() > 0) mLogString += msgFailedObjectListNotZero;
    
    Mesh* meshPtr = Renderer.CreateMesh();
    // Check mesh was created
    if (meshPtr == nullptr) mLogString += msgFailedObjectCreation;
    // Check mesh was destroyed
    if (!Renderer.DestroyMesh(meshPtr)) mLogString += msgFailedObjectDestruction;
    
    Material* materialPtr = Renderer.CreateMaterial();
    // Check material was created
    if (materialPtr == nullptr) mLogString += msgFailedObjectCreation;
    // Check material was destroyed
    if (!Renderer.DestroyMaterial(materialPtr)) mLogString += msgFailedObjectDestruction;
    
    
    // Finalize the test
    if (mLogString != "") {
        std::cout  << msgFailed << std::endl;
        std::cout << mLogString << std::endl;
        mLogString="";
    } else {
        std::cout << msgPassed << std::endl;
    }
    
    return;
}

