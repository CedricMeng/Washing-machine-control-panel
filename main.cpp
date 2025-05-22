/*
    Washing machine control panel

    Author: Zehao Meng 2025
*/

#include "mbed.h"
#include "sensor_voltage_converter.h"
#include "seven_segment_display.h"
#include "pwm_buzzer_music.h"
#include <cstdio>

// Serial communication
BufferedSerial pc(USBTX, USBRX, 115200);

// Sensors input
AnalogIn temperature_sensor(PC_3);  // Temperature sensor
AnalogIn light_sensor(PC_2);        // Light sensor
AnalogIn pressure_sensor(PA_1);     // Pressure sensor

// Indicators and buttons
DigitalOut power_led(PC_0);         // Power indicator LED
DigitalOut mode1_led(PC_1);         // Mode1 LED
DigitalOut mode2_led(PB_0);         // Mode2 LED
DigitalOut running_led(PA_4);       // Running LED

PwmOut warning_led(PB_3);           // Warning LED

DigitalIn power_button(PC_10);      // Power button
DigitalIn mode_button(PC_11);       // Mode selection button
DigitalIn start_button(PD_2);       // Start button

// Buzzer
PwmOut buzzer(PA_15);               // Buzzer

// System status enumeration
enum SystemState {
    OFF,
    IDLE,
    MODE_SELECT,
    PARAMETER_CHECK,
    RUNNING,
    COMPLETE,
    PAUSED // System paused due to error
};

// Washing mode enumeration
enum WashMode {
    NORMAL,    // 8s washing cycle
    DELICATE,  // 5s washing cycle
    NO_MODE    // Waiting for selection
};

// Mode names
const char* mode_names[] = {"Normal", "Delicate", "No Mode"};

SystemState current_state = OFF;
WashMode selected_mode = NO_MODE;
bool system_initialized = false;
Timer debounce_timer;
int count = 0;

bool lid_opened = false;      // Lid status flag

// Button debounce function pro plus max extreeeeeeeeeem 
bool Button(DigitalIn &button) {
    static bool last_state = 0;
    bool current_state = button.read();
    
    if(current_state && !last_state) {
        debounce_timer.start();
        while(debounce_timer.elapsed_time().count() < 50) {
            if(!button.read()) return false;
        }
        return true;
    }
    last_state = current_state;
    return false;
}

// Initialize system function
void iniSystem() {
    power_led = 0;
    mode1_led = 0;
    mode2_led = 0;
    warning_led = 0;
    running_led = 0;
    buzzer = 0.0f;
    SegDis_init();
    system_initialized = true;
    selected_mode = NO_MODE;
    printf("System Initialized\n");
}

// Display current state
void displayStatus() {
    switch(current_state) {
        case OFF: break;
        case IDLE: displayChar('I'); break;
        case MODE_SELECT: 
            if(selected_mode == NORMAL) displayChar('N');
            else displayChar('D');
            break;
        case PARAMETER_CHECK: displayChar('C'); break;
        case RUNNING: break;
        case COMPLETE: displayChar('E'); break;
        case PAUSED: displayChar('P');break;
    }
}

// Display system information in terminal
void displaySystemInfo() {
    printf("\n=== Washing Machine Status ===\n");
    printf("System State: ");
    switch(current_state) {
        case OFF: printf("OFF\n"); break;
        case IDLE: printf("IDLE\n"); break;
        case MODE_SELECT: printf("MODE SELECT\n"); break;
        case PARAMETER_CHECK: printf("PARAMETER CHECK\n"); break;
        case RUNNING: printf("RUNNING\n"); break;
        case COMPLETE: printf("COMPLETE\n"); break;
        case PAUSED: printf("PAUSED\n");break;
    }
    
    if(current_state != OFF) {
        printf("Current Mode: %s\n", mode_names[selected_mode]);
        printf("Temperature: %.1f°C\n", TMPconverter(temperature_sensor.read()));
        printf("Light Level: %.1f%%\n", LDRconverter(light_sensor.read()));
        printf("Pressure: %.1f%%\n", FSRconverter(pressure_sensor.read()));
    }
    printf("=============================\n");
}

// Check if parameters are within acceptable range
bool checkParameters() {
    float temp = TMPconverter(temperature_sensor.read());
    float light = LDRconverter(light_sensor.read());
    float pressure = FSRconverter(pressure_sensor.read());
    
    printf("Current Parameters - Temp: %.1fC, Light: %.1f%%, Pressure: %.1f%%\n", 
          temp, light, pressure);
    
    if(temp < 20.0f) {
        printf("Warning: Temperature too low!\n");
        play_warning();
        displayChar('L');// The water temperature is too low will make machine working
        warning_led = 1;
        return false;
    }
    
    if(pressure > 60.0f) {
        printf("Warning: Overweight!\n");
        play_warning();
        displayChar('H'); // High pressure means too much clothes in the machine
        warning_led = 1;
        return false;
    }
    if(light > 80.0f){
        printf("Warning: Please close the lid!\n");
        play_warning();
        displayChar('O'); // Open lid will make clothes get out the machine
        warning_led = 1;
        return false;
    }
    if(selected_mode == NO_MODE){
        printf("Warning: Please choose the mode!\n");
        play_warning();
        displayChar('C'); //choose a mode
        warning_led = 1;
        return false;
    }
    warning_led = 0;
    return true;
}

// Run washing cycle
void WashCycle() {
    printf("Starting %s wash cycle...\n", mode_names[selected_mode]);
    play_scale(WashingStartMusic, 8);
    running_led = 1;

    // Set time based on mode (seconds)
    int total_time;
    if (selected_mode == NORMAL) {
        total_time = 5;
    } else {
        total_time = 8;
    }
    
    for (int i = total_time; i >= 0; i--) {
        // Check lid status
        while (LDRconverter(light_sensor.read()) > 80.0f) { // Wait while lid is open
            if (current_state != PAUSED) {
                current_state = PAUSED;
                running_led = 0;
                warning_led = 1;
                printf("PAUSED: Lid opened!\n");
                displayChar('P');
                play_warning();
            }
            ThisThread::sleep_for(200ms); // Shorter detection interval
        }

        // Resume operation
        if (current_state == PAUSED) {
            current_state = RUNNING;
            running_led = 1;
            warning_led = 0;
            printf("RESUMED: Lid closed. Continuing...\n");
            displayChar('0' + i); // Show remaining time
        }

        // Normal countdown
        printf("Washing... remaining time: %ds\n", i);
        displayChar('0' + i);
        ThisThread::sleep_for(1000ms);
    }

    current_state = COMPLETE;
    printf("%s wash cycle complete!\n", mode_names[selected_mode]);
    play_scale(WashingFinishMusic, 8);
    ThisThread::sleep_for(50ms);

    iniSystem();// after the completion, reset the machine
    current_state = IDLE;
    power_led = 1;
}

int main() {
    iniSystem();
    int count = 0;
    
    while(true) {
        // Power button handling
        if(Button(power_button)) {
            if(current_state == OFF) {
                current_state = IDLE;
                power_led = 1;
                printf("System Powered On\n");
                play_note(NOTE_C5);
            } else {
                current_state = OFF;
                power_led = 0;
                mode1_led = 0;
                mode2_led = 0;
                warning_led = 0;
                lid_opened = false;
                printf("System Powered Off\n");
                SegDis_init();
            }
        }
        
        // System operation handling
        if(current_state != OFF) {
            // Mode button handling - can switch mode in IDLE or MODE_SELECT state
            if(Button(mode_button) && (current_state == IDLE || current_state == MODE_SELECT)) {
                current_state = MODE_SELECT;
                if(selected_mode == NO_MODE || selected_mode == DELICATE) {
                    selected_mode = NORMAL;
                    mode1_led = 1;
                    mode2_led = 0;
                } else {
                    selected_mode = DELICATE;
                    mode1_led = 0;
                    mode2_led = 1;
                }
                printf("Selected Mode: %s\n", mode_names[selected_mode]);
                play_note(NOTE_E5);
                
                // Display mode abbreviation on 7-segment display
                if(selected_mode == NORMAL) {
                    displayChar('N');
                } else {
                    displayChar('D');
                }
                
                // Briefly display then return to IDLE state while keeping mode selection
                current_state = IDLE;
            }
            
            // Start button handling - can only start from IDLE state
            if(Button(start_button) && current_state == IDLE) {
                current_state = PARAMETER_CHECK;
                printf("Checking parameters for %s mode...\n", mode_names[selected_mode]);
                if(checkParameters()) {
                    current_state = RUNNING;
                    WashCycle();
                } else {
                    current_state = IDLE;
                    printf("Cannot start in %s mode due to parameter issues\n", 
                         mode_names[selected_mode]);
                }
            }
            
            // Status display
            displayStatus();
            
            // Periodic system info display
            if(count % 20 == 0) {  // Display every 1 second (50ms*20)
                displaySystemInfo();
            }
            count++;
        }
        
        ThisThread::sleep_for(50ms);
    }
}