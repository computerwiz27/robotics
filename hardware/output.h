#ifndef _OUTPUT_H
#define _OUTPUT_H

#define BUZ_PWM_PIN 6
#define RED_LED_PIN LED_BUILTIN_RX
#define YEL_LED_PIN LED_BUILTIN
#define GRN_LED_PIN LED_BUILTIN_TX

#define RED     0x0
#define YELLOW  0x1
#define GREEN   0x2

class Output {
private:
    uint8_t led_pins[3] = {
    RED_LED_PIN,
    YEL_LED_PIN,
    GRN_LED_PIN
};

public:
    Output() {

    }

    void buzz(uint32_t ms, uint8_t vol) {
        pinMode(BUZ_PWM_PIN, OUTPUT);
        analogWrite(BUZ_PWM_PIN, vol);
        delay(ms);
        analogWrite(BUZ_PWM_PIN, 0);
    }
    
    void buzz(uint32_t ms) {
        buzz(ms, 30);
    }

    void toggle_on_led(uint8_t colour) {
        pinMode(led_pins[colour], OUTPUT);

        if (colour == RED) {
            digitalWrite(RED_LED_PIN, LOW);
        } else if (colour == YELLOW) {
            digitalWrite(YEL_LED_PIN, HIGH);
        } else if (colour == GREEN) {
            digitalWrite(GRN_LED_PIN, LOW);
        }
    }

    void toggle_off_led(uint8_t colour) {
        pinMode(led_pins[colour], OUTPUT);

        if (colour == RED) {
            digitalWrite(RED_LED_PIN, HIGH);
        } else if (colour == YELLOW) {
            digitalWrite(YEL_LED_PIN, LOW);
        } else if (colour == GREEN) {
            digitalWrite(GRN_LED_PIN, HIGH);
        }
    }

    void blink(uint8_t colour, uint16_t times) {
        for (uint16_t i = 0; i < times; i++) {
            this->toggle_on_led(colour);
            delay(150);
            this->toggle_off_led(colour);
            if (i + 1 < times)
                delay(150);
        }
    }
};


#endif