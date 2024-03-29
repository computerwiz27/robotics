#ifndef _MOTORS_H
#define _MOTORS_H

#define L_PWM_PIN   10
#define L_DIR_PIN   16
#define R_PWM_PIN   9
#define R_DIR_PIN   15

#define FWD LOW
#define REV HIGH

#define LEFT    0
#define RIGHT   1

class Motors {
private:

    int8_t l_pow;
    int8_t r_pow;
    
public:
    Motors() {
        pinMode(L_PWM_PIN, OUTPUT);
        pinMode(L_DIR_PIN, OUTPUT);
        pinMode(R_PWM_PIN, OUTPUT);
        pinMode(R_DIR_PIN, OUTPUT);

        l_pow = 0;
        r_pow = 0;
    }

    int8_t getPower(int8_t dir) {
        if (dir == LEFT) return l_pow;
        return r_pow;
    }

    void setPower(int left, int right) {
        if (left > 100) left = 100;
        if (left < -100) left = -100;
        if (right > 100) right = 100;
        if (right < -100) right = -100;

        l_pow = left;
        r_pow = right;

        int8_t l_dir = FWD;
        if (left < 0) {
            l_dir = REV;
            left *= -1;
        }
        int8_t r_dir = FWD;
        if (right < 0) {
            r_dir = REV;
            right *= -1;
        }

        float l_pow_float = (float)left / 100 * 0xff;
        uint8_t l_pow = l_pow_float;

        float r_pow_float = (float)right / 100 * 0xff;
        uint8_t r_pow = r_pow_float;

        digitalWrite(L_DIR_PIN, l_dir);
        digitalWrite(R_DIR_PIN, r_dir);
        analogWrite(L_PWM_PIN, l_pow);
        analogWrite(R_PWM_PIN, r_pow);
    }

    void stop() {
        this->setPower(0, 0);
    }
};

#endif