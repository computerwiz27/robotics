#include "hardware.h"
#include "behaviour.h"

#include "hardware/pid.h"

uint8_t state;
void setup() {
    Serial.begin(9600);
    delay(10);
    
    setupHardware();

    heading_pid->setActive(true);
    heading_pid->setDemand(PI);
}


void loop() {

}
