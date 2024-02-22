#ifndef _PRIMITIVES_H
#define _PRIMITIVES_H

#include "../hardware.h"

void motor_power(int left, int right) {
    motor_l_pid->setDemand((float)left);
    motor_r_pid->setDemand((float)right);
}

void stop() {
    motor_l_pid->setDemand(0);
    motor_r_pid->setDemand(0);
}

void drive_straight(int8_t power, int distance) {
    float start_x = kinematics.getCoordonates().x;
    float d_t = 0;
    motor_power(power, power);
    while(d_t < (float)distance) {
        d_t = kinematics.getCoordonates().x - start_x;
    }
    stop();
}

void drive_straight(int8_t power) {
    motor_power(power, power);
}

void follow_line() {
    float w = sensors.getLineWeight();
    int8_t left_pow = LINE_FOLLOW_POW - w * TURN_POW;
    int8_t right_pow = LINE_FOLLOW_POW + w * TURN_POW;
    motor_power(left_pow, right_pow);
}

void turn(double angle) {
    float start_th = kinematics.getCoordonates().th;
    float target = start_th + angle;
    if (target > PI) target -= 2 * PI;
    if (target < -1 * PI) target += 2 * PI;

    heading_pid->setMode(HEADING_TURN);
    heading_pid->setActive(true);
    heading_pid->setDemand(target);
    while(kinematics.getCoordonates().th >= target * 1.05 ||
        kinematics.getCoordonates().th <= target * 0.95 ) {
            Serial.println(kinematics.getCoordonates().th * RAD_TO_DEG);
    }
    heading_pid->setActive(false);
    stop();
}   

void turn(int angle) {
    turn((float)angle * (float)DEG_TO_RAD);
}

void turn_in_place(int8_t direction, uint8_t power) {
    if (direction == LEFT) {
        motor_l_pid->setDemand(-1 * (float)power);
        motor_r_pid->setDemand(power);
    } 
    if (direction == RIGHT) {
        motor_l_pid->setDemand(power);
        motor_r_pid->setDemand(-1 * (float)power);
    }
}

void turn_in_place(int8_t direction) {
    turn_in_place(direction, TURN_POW);
}

#endif