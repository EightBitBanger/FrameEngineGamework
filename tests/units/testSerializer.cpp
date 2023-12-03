#include <iostream>
#include <string>

#include "../framework.h"
#include <Serialization/Serialization.h>

extern Serialization  Serializer;

struct SaveTestType {
    int    testA;
    int    testB;
    float  testC;
    float  testD;
};


void TestFramework::TestSerializerSystem(void) {
    if (hasTestFailed) return;
    
    std::cout << "Serialization system.... ";
    
    // Save type data out to file
    SaveTestType* bufferA = (SaveTestType*)malloc(sizeof(SaveTestType));
    bufferA->testA = 241;
    bufferA->testB = 636;
    bufferA->testC = 912.71f;
    bufferA->testD = 531.93f;
    Serializer.Serialize("serialize_test", (void*)bufferA, sizeof(SaveTestType));
    free(bufferA);
    
    // Load file back
    SaveTestType* bufferB = (SaveTestType*)malloc(sizeof(SaveTestType));
    Serializer.Deserialize("serialize_test", (void*)bufferB, sizeof(SaveTestType));
    if (bufferB->testA != 241) {Throw(msgFailedSerialization, __FILE__, __LINE__); return;}
    if (bufferB->testB != 636) {Throw(msgFailedSerialization, __FILE__, __LINE__); return;}
    if (bufferB->testC != 912.71f) {Throw(msgFailedSerialization, __FILE__, __LINE__); return;}
    if (bufferB->testD != 531.93f) {Throw(msgFailedSerialization, __FILE__, __LINE__); return;}
    free(bufferB);
    
    remove("serialize_test");
    
    return;
}

