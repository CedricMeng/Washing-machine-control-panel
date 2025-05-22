#include "seven_segment_display.h"

// 7-segment display segment pin definitions
DigitalOut seg1(PA_12);
DigitalOut seg2(PA_11);
DigitalOut seg3(PB_12);
DigitalOut seg4(PB_11);
DigitalOut seg5(PB_2);
DigitalOut seg6(PB_1);
DigitalOut seg7(PB_15);
DigitalOut seg8(PB_14);

// Bus output for segments and decimal point pin
BusOut SegDis(PA_11, PA_12, PB_1, PB_15, PB_14, PB_12, PB_11);
DigitalOut decimal_point(PB_2);

// 7-segment display encoding array (hex values)
const int hexDis[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

// Character to 7-segment encoding mapping table
const int charToHex[] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F, // 9
    0x77, // A
    0x7C, // B
    0x39, // C
    0x5E, // D
    0x79, // E
    0x71, // F
    0x38, // G
    0x76, // H
    0x30, // I
    0x1E, // J
    0x75, // K
    0x38, // L
    0x54, // M
    0x5C, // N
    0x3F, // O
    0x73, // P
    0x67, // Q
    0x50, // R
    0x6D, // S
    0x78, // T
    0x3E, // U
    0x3E, // V
    0x6A, // W
    0x76, // X
    0x6E, // Y
    0x5B  // Z
};

// Decimal point status
int dp_status = 0;

// Initialize 7-segment display
void SegDis_init() {
    SegDis.write(0x00); // Turn off display
    ThisThread::sleep_for(200ms);
}

// Display character on 7-segment 
void displayChar(char c) {
    if (c >= '0' && c <= '9') {
        SegDis.write(charToHex[c - '0']);
    } else if (c >= 'A' && c <= 'Z') {
        SegDis.write(charToHex[c - 'A' + 10]);
    }
    ThisThread::sleep_for(500ms);
}