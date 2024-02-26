#ifndef _HARDWARE_H
#define _HARDWARE_H

#include "hardware/encoders.h"
#include "hardware/linesensors.h"
#include "hardware/input.h"
#include "hardware/output.h"
#include "hardware/pid.h"
#include "hardware/timer3.h"
#include "debug.h"

#if DEBUG_MEM
#include<MemoryFree.h>
#endif

Motors motors;
LineSensors sensors;
Input in;
Output out;
Kinematics kinematics;

MotorController *motor_l_pid;
MotorController *motor_r_pid;

HeadingController *heading_pid;

void setupHardware() {
    setupEncoders();

    motors = Motors();
    sensors = LineSensors();
    in = Input();
    out = Output();
    kinematics = Kinematics();

    motor_l_pid = new MotorController(&motors, LEFT, &angular_v_l, 1, 0.3, 0.01);
    motor_r_pid = new MotorController(&motors, RIGHT, &angular_v_r, 1, 0.3, 0.01);

    heading_pid = new HeadingController(motor_l_pid, motor_r_pid, 1, 0.05, 0.005);
    heading_pid->setActive(false);
    
    setupTimer3();
}

ISR( TIMER3_COMPA_vect ) {
    updateKinematics();
    motor_l_pid->update();
    motor_r_pid->update();
    heading_pid->update();

#if DEGBUG_MEM
    Serial.println(freeMemory());
#endif
}

#endif
