#ifndef _KINEMATICS_H
#define _KINEMATICS_H

#include "encoders.h"
#include "pid.h"

#include <math.h>

// dimensions measured in mm
#define WHEEL_RADIUS        16
#define WHEEL_SEPARATION    85.2

// useful constants
#define STEPS_TO_RAD    0.017536102
#define STEPS_TO_DEG    1.004744627
#define STEPS_PER_REV   358.3

#define MAX_ANGULAR_V 90 // radians per second

// must be between  1 and 262
#define MEASURE_TS  20 //ms

#define LINE_FOLLOW_POW 13
#define TURN_POW        13

typedef struct coords {
    volatile float x;
    volatile float y;
    volatile float th;
} coords_t;

coords_t global_coords;

volatile long last_count_l;
volatile long last_count_r;

volatile float angular_v_l;
volatile float angular_v_r;
float alpha = 0.25;

class Kinematics {
public:
    Kinematics() {
        global_coords =  {0, 0, 0};

        last_count_l = count_e_l;
        last_count_r = count_e_r;

        angular_v_l = 0;
        angular_v_r = 0;
    }

    coords getCoordonates() {
        coords_t ret = {
            global_coords.x,
            global_coords.y,
            global_coords.th
        };

        return ret;
    }

    void resetCoordonates() {
        global_coords = {0,0,0};
    }

};

void updatePosition(float phi_l, float phi_r) {
    float x_cont = (phi_l + phi_r) * WHEEL_RADIUS / 2;
    float th_cont = (phi_l - phi_r) *  WHEEL_RADIUS / WHEEL_SEPARATION;

    global_coords.x += x_cont * cos(global_coords.th);
    global_coords.y += x_cont * sin(global_coords.th);
    global_coords.th += th_cont;

    if (global_coords.th > PI ) {
        global_coords.th = -PI + (global_coords.th - PI);
    }
    else if (global_coords.th < -PI) {
        global_coords.th = PI - (global_coords.th - PI);
    }
}

void updateAngularV(float phi_l, float phi_r) {
    float angular_v_l_comp = phi_l / (MEASURE_TS / 1000.0);
    float angular_v_r_comp = phi_r / (MEASURE_TS / 1000.0);

    angular_v_l = (angular_v_l * (1-alpha)) + (angular_v_l_comp * alpha);
    angular_v_r = (angular_v_r * (1-alpha)) + (angular_v_r_comp * alpha);
}

void updateKinematics() {
    float phi_l = (float)(count_e_l - last_count_l) * STEPS_TO_RAD;
    float phi_r = (float)(count_e_r - last_count_r) * STEPS_TO_RAD;

    updatePosition(phi_l, phi_r);
    updateAngularV(phi_l, phi_r);

    last_count_l = count_e_l;
    last_count_r = count_e_r;
}

#endif