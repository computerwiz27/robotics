#ifndef _HARDWARE_H
#define _HARDWARE_H

#include "hardware/encoders.h"
#include "hardware/motors.h"
#include "hardware/linesensors.h"
#include "hardware/input.h"
#include "hardware/output.h"
#include "hardware/kinematics.h"

Motors motors;
LineSensors sensors;
Input in;
Output out;
Kinematics kinematics;

void setupHardware() {
    setupEncoders();

    motors = Motors();
    sensors = LineSensors();
    sensors.calibrate();
    in = Input();
    out = Output();
    kinematics = Kinematics();
    
    setupTimer3();
}

#endif