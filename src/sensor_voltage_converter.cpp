// sensor_voltage_converter.cpp
#include "sensor_voltage_converter.h"

// convert the sensor's value to voltage signal
float TMPconverter(float TMPValue) {

    const float REFERENCE_VOLTAGE = 3.3;
    return TMPValue * REFERENCE_VOLTAGE/0.01;
}

float LDRconverter(float LDRValue) {

    return LDRValue*100;
}

float FSRconverter(float FSRValue) {

    return FSRValue*100*3.3/2.74;
}

