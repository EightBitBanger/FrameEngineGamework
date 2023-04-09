#include "strings.h"
#include "types.h"
#include "logging.h"


std::string FloatToString(float Float) {
    std::stringstream sStream;
    sStream << Float;
    std::string String = sStream.str();
    if (!StringFind(".", String)) {String += ".0";}
    return String;
}

float StringToFloat(std::string String) {
    float Float;
    std::stringstream(String) >> Float;
    return Float;
}

std::string DoubleToString(double Double) {
    std::stringstream sStream;
    sStream << Double;
    std::string String = sStream.str();
    if (!StringFind(".", String)) {String += ".0";}
    return String;
}

double StringToDouble(std::string String) {
    double Double;
    std::stringstream(String) >> Double;
    return Double;
}

std::string IntToString(int Integer) {
    std::stringstream sStream;
    sStream << Integer;
    return sStream.str();
}

int StringToInt(std::string String) {
    int Integer;
    std::stringstream(String) >> Integer;
    return Integer;
}
