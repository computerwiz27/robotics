#ifndef _STATES_H
#define _STATES_H

#define STATE_INITIAL       0
#define STATE_FOLLOW_LINE   1
#define STATE_TURN_90       2
#define STATE_LINE_END      3
#define STATE_SEARCH_LINE   4

uint8_t getState() {
    if (sensors.isOnLine(3)) {
        return STATE_FOLLOW_LINE;
    }
    if (sensors.isOnLine(1) || sensors.isOnLine(5)) {
        return STATE_TURN_90;
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
    while(!sensors.isOnLine(3)){}
    while(sensors.isOnLine(3)){}
    while(!sensors.isOnLine(3)){
        turn_in_place(RIGHT, 7);
    }
}

void state_follow_line() {
    follow_line();
}

void state_turn_90() {
    int8_t dir;
    if (sensors.isOnLine(1)) {
        dir = LEFT;
    } else {
        dir = RIGHT;
    }
    while(!sensors.isOnLine(3)) {
        turn_in_place(dir, 7);
    }
}

void state_search_line() {
    while(!sensors.isOnLine(3)) {
        drive_straight(20);
    }
    stop();
}

void actState(uint8_t state) {
    if (state == STATE_INITIAL) {
        state_initial();
    } else if (state == STATE_FOLLOW_LINE) {
        state_follow_line();
    } else if (state == STATE_TURN_90) {
        state_turn_90();
    }
}

#endif