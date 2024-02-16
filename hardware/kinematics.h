#ifndef _KINEMATICS_H
#define _KINEMATICS_H

#include "encoders.h"

#include <math.h>

// dimensions measured in mm
#define WHEEL_RADIUS        16
#define WHEEL_SEPARATION    85.2

// useful constants
#define STEPS_TO_RAD    0.017536102
#define STEPS_TO_DEG    1.004744627
#define STEPS_PER_REV   358.3

// must be between  1 and 262
#define MEASURE_TS  20 //ms

typedef struct coords {
    volatile float x;
    volatile float y;
    volatile double th;
} coords_t;

coords_t global_coords;

volatile long last_count_l;
volatile long last_count_r;

class Kinematics {
public:
    Kinematics() {
        global_coords =  {0, 0, 0};

        last_count_l = count_e_l;
        last_count_r = count_e_r;
    }

    coords get_coordonates() {
        coords_t ret = {
            global_coords.x,
            global_coords.y,
            global_coords.th
        };

        return ret;
    }

};

void update_position() {
    double phi_l = (double)(count_e_l - last_count_l) * STEPS_TO_RAD;
    double phi_r = (double)(count_e_r - last_count_r) * STEPS_TO_RAD;

    float x_cont = (phi_l + phi_r) * WHEEL_RADIUS / 2;
    double th_cont = (phi_l - phi_r) *  WHEEL_RADIUS / WHEEL_SEPARATION;

    global_coords.x += x_cont * cos(global_coords.th);
    global_coords.y += x_cont * sin(global_coords.th);
    global_coords.th += th_cont;

    if (abs(global_coords.th) > 2 * PI) {
        
    }

    last_count_l = count_e_l;
    last_count_r = count_e_r;
}

// Using datasheet for ATmega32U4 
// https://www.microchip.com/en-us/product/ATmega32u4

// #include <avr/iocanxx.h>
int led_state;

void setupTimer3() {
    led_state = LOW;
    pinMode(LED_BUILTIN, OUTPUT);

    // disable interrupts on output compare match A
    // page 138, 14.10.18
    TIMSK3 &= ~(1 << OCIE3A);

    // reset timer3 controll registers
    // pages 131 - 135, 14.10.2 - 14.10.6
    TCCR3A = 0;
    TCCR3B = 0;
    TCCR3C = 0;

    // set prescaler to 64
    // page 134, 14.10.4
    TCCR3B |= (1 << CS30);
    TCCR3B |= (1 << CS31);
    TCCR3B &= ~(1 << CS32);

    // clear timer3 on compare
    // page 133, 14.10.3
    TCCR3B |= (1 << WGM32);

    // load value to match against in reg A
    // page 136, 14.10.12
    int count_to = (int)(MEASURE_TS / (62.5 * pow(10, -6) * 64));
    OCR3A = count_to;

    // reset timer3
    TCNT3 = 0;

    // enable interrupts on output compare match A
    // page 138, 14.10.18
    TIMSK3 |= (1 << OCIE3A);
}

ISR( TIMER3_COMPA_vect ) {
    update_position();
}

#endif