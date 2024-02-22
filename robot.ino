#include "hardware.h"
#include "behaviour.h"

#include "hardware/pid.h"

uint8_t state;

void setup() {
    Serial.begin(9600);
    delay(10);
    
    setupHardware();

    state = STATE_INITIAL;

    // heading_pid->setDemand(100, 100);
    // heading_pid->setMode(HEADING_GOTO);
    // heading_pid->setActive(true);
    // Serial.print("x: "), Serial.print(global_coords.x); Serial.print(", ");
    // Serial.print("y: "); Serial.println(global_coords.y);
    // while(global_coords.x < 100 || global_coords.y < 100) {
    //     Serial.print("x: "), Serial.print(global_coords.x); Serial.print(", ");
    //     Serial.print("y: "); Serial.println(global_coords.y);
    //     delay(10);
    // }
    // heading_pid->setActive(false);
    // stop();

}


void loop() {
    actState(state);
    state = getState();
}
