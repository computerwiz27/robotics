#ifndef _PRIMITIVES_H
#define _PRIMITIVES_H

#include "../hardware.h"

#define LINE_FOLLOW_POW 20
#define TURN_POW        10

void drive_straight(int8_t power) {
    motors.setPower(power, power);
}

void stop() {
    motors.stop();
}

void follow_line() {
    while(sensors.isOnLine(3)) {
        float w = sensors.lineWeight();
        int8_t left_pow = TURN_POW - w * LINE_FOLLOW_POW;
        int8_t right_pow = TURN_POW + w * LINE_FOLLOW_POW;
        motors.setPower(left_pow, right_pow);
    }
    stop();
}

void turn_in_place(int8_t direction, uint8_t power) {
    motors.turn(power, direction);
}

void turn_in_place(int8_t direction) {
    turn_in_place(direction, TURN_POW);
}

#endif