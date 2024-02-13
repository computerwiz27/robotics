#ifndef _INPUT_H
#define _INPUT_H

#define A_BUT_PIN   14
#define B_BUT_PIN   30
#define C_BUT_PIN   17

#define A_BUTTON 0x0
#define B_BUTTON 0x1
#define C_BUTTON 0x2


class Input {
private:
    uint8_t button_pins[3] = {
        A_BUT_PIN,
        B_BUT_PIN,
        C_BUT_PIN
    };


public:
    Input() {

    }

    uint8_t getButtonState(uint8_t button) {
        pinMode(button_pins[button], INPUT);
        return digitalRead(button_pins[button]);
    }

    void waitForPress(uint8_t button) {
        uint8_t pin  = button_pins[button];
        pinMode(pin, INPUT);
        while(digitalRead(pin) == HIGH) {

        }
        while(digitalRead(pin) == LOW) {

        }
    }
};

#endif