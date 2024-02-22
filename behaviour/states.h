#ifndef _STATES_H
#define _STATES_H

#include "primitives.h"

#define STATE_INITIAL           0
#define STATE_FOLLOW_LINE       1
#define STATE_TURN_90           2
#define STATE_LINE_END          3
#define STATE_RETURN_TO_START   4
#define STATE_SEARCH_LINE       5

uint8_t getState() {
    if (sensors.isOnLine(3)) {
        return STATE_FOLLOW_LINE;
    }
    if (sensors.isOnLine(1) != sensors.isOnLine(5)) {
        return STATE_TURN_90;
    }

    coords_t position = kinematics.getCoordonates();
    float distance_from_start = sqrt(position.x * position.x + position.y * position.y);

    if (distance_from_start > 0) {
        return STATE_RETURN_TO_START;
    }

    return STATE_SEARCH_LINE;
}

void state_initial() {
    out.toggle_on_led(RED);
    out.toggle_on_led(YELLOW);
    out.toggle_on_led(GREEN);
    delay(500);
    out.toggle_off_led(RED);
    out.toggle_off_led(YELLOW);
    out.toggle_off_led(GREEN);

    in.waitForPress(B_BUTTON);

    delay(500);
    sensors.calibrate();
    out.blink(RED, 3);

    drive_straight(10);
    while(!sensors.isOnLine(3)) {}
    while(sensors.isOnLine(3)) {}
    while(!sensors.isOnLine(3)) {}
    while(sensors.isOnLine(3)) {}
    stop();
    while(!sensors.isOnLine(3)) {
        turn_in_place(RIGHT, TURN_POW * 0.7);
    }
    stop();
}

void state_follow_line() {
    out.toggle_on_led(GREEN);
    while (
        sensors.isOnLine(3) ||
        sensors.isOnLine(2) != sensors.isOnLine(4)
    )
    {
        follow_line();
    }
    stop();
    out.toggle_off_led(GREEN);
}

void state_turn_90() {
    int8_t dir;
    if (sensors.isOnLine(1)) {
        dir = LEFT;
        while(!sensors.isOnLine(3)) {
        turn_in_place(dir, TURN_POW * 0.7);
    }
    } 
    else {
        dir = RIGHT;
        while(!sensors.isOnLine(3)) {
            turn_in_place(dir, TURN_POW * 0.7);
        }
    }
}

void state_return_to_start() {
    out.buzz(100);

    coords position = kinematics.getCoordonates();
    float distance = sqrt(position.x * position.x + position.y * position.y);

    heading_pid->setMode(HEADING_GOTO);
    heading_pid->setDemand(0, 0);
    heading_pid->setActive(true);

    while (distance > 0){
        coords position_new = kinematics.getCoordonates();
        float x_comp = position_new.x - position.x;
        x_comp *= x_comp;
        float y_comp = position_new.y - position.y;
        y_comp *= y_comp;
        float diff = sqrt(x_comp + y_comp);
        distance -= diff;
        position = position_new;
        delay(20);
    }
    heading_pid->setActive(false);
    stop();
    while (true){
    }
    
}

void state_search_line() {
    stop();
}

void actState(uint8_t state) {
    if (state == STATE_INITIAL) {
        state_initial();
    } else if (state == STATE_FOLLOW_LINE) {
        state_follow_line();
    } else if (state == STATE_TURN_90) {
        state_turn_90();
    } else if (state = STATE_RETURN_TO_START) {
        state_return_to_start();
    } else if (state == STATE_SEARCH_LINE) {
        state_search_line();
    }
}

#endif