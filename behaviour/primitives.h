#ifndef _PRIMITIVES_H
#define _PRIMITIVES_H

#include "../hardware.h"

void motor_power(int left, int right) {
    motor_l_pid->setDemand(left);
    motor_r_pid->setDemand(right);
}

void stop() {
    motor_l_pid->setDemand(0);
    motor_r_pid->setDemand(0);
}

void drive_straight(int8_t power, int distance) {
    float start_x = kinematics.get_coordonates().x;
    float d_t = 0;
    motor_power(power, power);
    while(d_t < (float)distance) {
        d_t = kinematics.get_coordonates().x - start_x;
    }
    stop();
}

void drive_straight(int8_t power) {
    drive_straight(power, INTMAX_MAX);
}


void follow_line() {
    while(sensors.isOnLine(3) && !(sensors.isOnLine(1) || sensors.isOnLine(5))) {
        float w = sensors.getLineWeight();
        int8_t left_pow = LINE_FOLLOW_POW - w * TURN_POW;
        int8_t right_pow = LINE_FOLLOW_POW + w * TURN_POW;
        motor_power(left_pow, right_pow);
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