#ifndef _STATES_H
#define _STATES_H

#include "primitives.h"
#include "../debug.h"

#define END_DISTANCE_FROM_ORIGIN 1100

volatile int32_t distance_from_start;
float distance_scale = 1.0;

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

    while(!sensors.isOnLine(2)) {}
    while(sensors.isOnLine(2)) {}
    while(!sensors.isOnLine(2)) {}  

    advance_to_sens(10, 2);

    turn_in_place(RIGHT, TURN_POW * 0.7);
    while(!sensors.isOnLine(2)) {
    }
    stop();
}

void state_set_second_run() {
    //adjust!
    distance_scale = 1.075;
    wheel_separation = 82.3;
    run = 2;

    in.waitForPress(B_BUTTON);
    delay(100);
    out.blink(RED, 1);

    state_initial();
}

void state_follow_line() {
    out.toggle_on_led(GREEN);
    bool *onLine = sensors.areOnLine();

    while (onLine[2])
    {
        if ( onLine[0] || onLine[4] ) {
            break;
        }
        follow_line();
        free(onLine);
        onLine = sensors.areOnLine();
    }
    stop();
    free(onLine);
    out.toggle_off_led(GREEN);
}

void state_intersection() {
    out.toggle_on_led(RED);
    bool front_path = false;
    bool left_path = false;
    bool right_path = false;

    coords start_pos = kinematics.getCoordonates();
    float travelled = 0;
    motor_power(9 ,9);
    while(travelled < 22) {
        if(!left_path) {
            if (sensors.isOnLine(0)) {
                left_path = true;
            }
        }
        if(!right_path) {
            if (sensors.isOnLine(4)){
                right_path = true;
            }
        }

        coords new_pos = kinematics.getCoordonates();
        float x_comp = start_pos.x - new_pos.x;
        x_comp *= x_comp;
        float y_comp = start_pos.y - new_pos.y;
        y_comp *= y_comp;
        travelled = sqrt(x_comp + y_comp);
    }

    stop();
    if (sensors.isOnLine(2)) {
        front_path = true;
    } else {
        if (sensors.isOnLine(1)) {
            left_path = true;
        }

        if (sensors.isOnLine(3)) {
            right_path = true;
        }
    }

#if DEBUG_INTER
    out.buzz(100);
    while (in.getButtonState(B_BUTTON) == HIGH)
    {
        Serial.print("left path: ");Serial.print(left_path);
        Serial.print(", front path: "); Serial.print(front_path);
        Serial.print(", right path: "); Serial.println(right_path);
        Serial.println();
    }
    out.buzz(100);
    delay(100);
#endif
    

    if (right_path && front_path && !left_path){
        out.toggle_off_led(RED);
        return;
    }

    bool *onLine = sensors.areOnLine();

    if (right_path && !front_path && !left_path) {
        bool *onLine = sensors.areOnLine();
        while(!( onLine[2] && (onLine[1] || onLine[3]) )) {
            turn_in_place(RIGHT, TURN_POW);
            free(onLine);
            onLine = sensors.areOnLine();
        }
        free(onLine);
        out.toggle_off_led(RED);
        return;
    }

    if (front_path && left_path) {
        while(sensors.isOnLine(2)) {
            turn_in_place(LEFT, TURN_POW);
        }
        while(!( onLine[2] && (onLine[1] || onLine[3]) )) {
            turn_in_place(LEFT, TURN_POW);
            free(onLine);
            onLine = sensors.areOnLine();
        }
        free(onLine);
    }

    onLine = sensors.areOnLine();
    while(!( onLine[2] && (onLine[1] || onLine[3]) )) {
        turn_in_place(LEFT, TURN_POW);
        free(onLine);
        onLine = sensors.areOnLine();
    }
    free(onLine);
    out.toggle_off_led(RED);
}

void state_line_end() {
    advance_to_sens(10, 2);

    bool *onLine = sensors.areOnLine();
    while(!( onLine[2] && (onLine[1] || onLine[3]) )) {
        turn_in_place(LEFT, TURN_POW);
        free(onLine);
        onLine = sensors.areOnLine();
    }
    free(onLine);

    // stop();
    // out.buzz(100);
    // while(in.getButtonState(B_BUTTON) == HIGH) {
    //     Serial.println(distance_from_start);
    // }
    // out.buzz(100);
    // delay(100);
}


void state_return_to_start() {
    out.toggle_on_led(RED);
    coords position = kinematics.getCoordonates();
    float distance = sqrt(position.x * position.x + position.y * position.y);
    distance *= 1.1;
    distance *= 1.05;

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
    
    state_set_second_run();
}

void state_adjust_course() {
    stop();
    bool *onLine = sensors.areOnLine();

    while(true) {
        for (int i = 0; i < 5; i++ ) {
            Serial.print("[");Serial.print(i);Serial.print("]: ");
            Serial.print(onLine[i]); Serial.print(", ");
        }
        Serial.println();
        free(onLine);
         onLine = sensors.areOnLine();
    }

    int8_t dir = -1;
    if (sensors.isOnLine(4)) dir = 1;

    while(!sensors.isOnLine(2)) {
        int8_t left_pow = LINE_FOLLOW_POW / 2 - dir * 0.7 * TURN_POW;
        int8_t right_pow = LINE_FOLLOW_POW / 2 + dir * 0.7 * TURN_POW;
        motor_power(left_pow, right_pow);
    }
    stop();
}

void actState() {
    bool *onLine = sensors.areOnLine();

    if (onLine[2]) {
        if (!(onLine[0] || onLine[4])) {
            state_follow_line();
            goto end_actState;
        }

        if (onLine[0] || onLine[4]) {
            state_intersection();
            goto end_actState;
        }
    } 
    
    else if (!onLine[2]) {
        // edge case on 90 turns
        if ( onLine[0] == true || onLine[4] == true ) {
            state_intersection();
            // state_adjust_course();
            goto end_actState;
        }

        if (onLine[1] == true || onLine[3] == true
        ) {
            state_intersection();
            goto end_actState;
        }
        
        if (
            onLine[0] == false
            && onLine[1] == false
            && onLine[2] == false
            && onLine[3] == false
            && onLine[4] == false
        ) {
            coords_t position = kinematics.getCoordonates();
            distance_from_start = (int32_t)sqrt(position.x * position.x + position.y * position.y); 

            if(distance_from_start > END_DISTANCE_FROM_ORIGIN){
                state_return_to_start();
                goto end_actState;

            } else {
                state_line_end();
                goto end_actState;
            }
        }
    }

    stop();
    out.buzz(100);
    while(true) {
        for (int i = 0; i < 5; i++ ) {
            Serial.print("[");Serial.print(i);Serial.print("]: ");
            Serial.print(onLine[i]); Serial.print(", ");
        }
        Serial.println();
    }

end_actState:;
    free(onLine);
}

#endif