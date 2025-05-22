#ifndef SEVEN_SEGMENT_DISPLAY_H
#define SEVEN_SEGMENT_DISPLAY_H

#include "PinNames.h"
#include "mbed.h"


// Define segment control pins for 7-segment display
extern DigitalOut seg1;
extern DigitalOut seg2;
extern DigitalOut seg3;
extern DigitalOut seg4;
extern DigitalOut seg5;
extern DigitalOut seg6;
extern DigitalOut seg7;
extern DigitalOut seg8;

// Define bus output for segments and decimal point pin
extern BusOut SegDis;
extern DigitalOut decimal_point;

// 7-segment display encoding array (hex values)
extern const int hexDis[];

// Character to 7-segment encoding mapping table
extern const int charToHex[];

// Initialize 7-segment display
void SegDis_init();

// Display specified character on 7-segment display
void displayChar(char c);

#endif