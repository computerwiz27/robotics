#include "hardware.h"

uint8_t state;

void setup()
{
    setupHardware();

    Serial.begin(9600);
    Serial.println("***RESET**");
    delay(1000);

    //motors.setPower(10, 10);
}

void loop()
{

}
