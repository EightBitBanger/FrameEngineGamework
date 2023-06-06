#include "types.h"


std::string FloatToString(float value) {
    std::stringstream sStream;
    sStream << value;
    std::string output = sStream.str();
    if (output.find(".") != 0) {output += ".0";}
    return output;
}

float StringToFloat(std::string str) {
    float value;
    std::stringstream(str) >> value;
    return value;
}

std::string DoubleToString(double value) {
    std::stringstream sStream;
    sStream << value;
    std::string output = sStream.str();
    if (output.find(".") != 0) {output += ".0";}
    return output;
}

double StringToDouble(std::string str) {
    double value;
    std::stringstream(str) >> value;
    return value;
}

std::string IntToString(int value) {
    std::stringstream sStream;
    sStream << value;
    return sStream.str();
}

int StringToInt(std::string str) {
    int value;
    std::stringstream(str) >> value;
    return value;
}

