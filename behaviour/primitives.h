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
    coords start_pos = kinematics.getCoordonates();
    float travelled = 0;
    motor_power(power, power);
    while(travelled < (float)distance) {
        coords new_pos = kinematics.getCoordonates();
        float x_comp = start_pos.x - new_pos.x;
        x_comp *= x_comp;
        float y_comp = start_pos.y - new_pos.y;
        y_comp *= y_comp;
        travelled = sqrt(x_comp + y_comp);
    }
    stop();
}

void drive_straight(int8_t power) {
    motor_power(power, power);
}

void advance_to_sens(int8_t power, uint8_t sensor_no) {
    if (sensor_no == 2) {
        drive_straight(power, MID_LINE_SENS_TO_COR);
    } else if (sensor_no == 1 || sensor_no == 3) {
        drive_straight(power, SIDE_LINE_SENS_TO_COR);
    } else if (sensor_no == 0 || sensor_no == 4) {
        drive_straight(power, SIDE_SENS_TO_COR);
    }
}

void follow_line() {
    float w = sensors.getLineWeight();
    int8_t left_pow = LINE_FOLLOW_POW - w * TURN_POW;
    int8_t right_pow = LINE_FOLLOW_POW + w * TURN_POW;
    motor_power(left_pow, right_pow);
}

void turn(double angle) {
    out.buzz(100);
    float start_th = kinematics.getCoordonates().th;
    float target = start_th + angle;
    if (target > PI) target -= 2 * PI;
    if (target < -1 * PI) target += 2 * PI;

    heading_pid->setMode(HEADING_TURN);
    heading_pid->setActive(true);
    heading_pid->setDemand(target);
    while(kinematics.getCoordonates().th >= target * 1.025 ||
        kinematics.getCoordonates().th <= target * 0.975 ) {
    }
    heading_pid->setActive(false);
    out.buzz(100);
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