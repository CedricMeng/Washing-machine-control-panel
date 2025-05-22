#include "pwm_buzzer_music.h"

int WashingStartMusic[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, NOTE_D5, NOTE_F5, NOTE_A4, NOTE_B4};
int WashingFinishMusic[] = {NOTE_G4, NOTE_A4, NOTE_C5, NOTE_E4, NOTE_B4, NOTE_F5, NOTE_D5, NOTE_C4};

// play single note
void play_note(int frequency) {
    buzzer.period_us((float) 1000000.0f / (float) frequency);    // Set PWM signal period (in microseconds)
    buzzer.pulsewidth_us(buzzer.read_period_us() / 2);           // Set pulse width to half the period (50% duty cycle)
    buzzer.write(0.5f);                                         // Set 50% duty cycle
    ThisThread::sleep_for(250ms);                               // Play note for 250ms
    buzzer.write(0.0f);                                         // keep quiet
}

// play the array of musical notes
void play_scale(int* scale, int length) {
    for (int i = 0; i < length; i++) {
        play_note(scale[i]);
    }
    buzzer.write(0.0f); // keep quiet
}

// Buzzer Warning
void play_warning(){
    play_note(NOTE_C6);
    play_note(NOTE_C6);
    play_note(NOTE_C6);
}



