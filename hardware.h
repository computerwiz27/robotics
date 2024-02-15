#ifndef _HARDWARE_H
#define _HARDWARE_H

#include "hardware/encoders.h"
#include "hardware/motors.h"
#include "hardware/linesensors.h"
#include "hardware/input.h"
#include "hardware/output.h"
#include "hardware/dimensions.h"

Motors motors;
LineSensors sensors;
Input in;
Output out;

void setupHardware() {
    setupEncoders();

    motors = Motors();
    sensors = LineSensors();
    in = Input();
    out = Output();
    
    sensors.calibrate();
}

#endif