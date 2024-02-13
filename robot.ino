#include "hardware.h"

uint8_t state;

void setup()
{
    // setupHardware();

    Serial.begin(9600);
    Serial.println("***RESET**");
    delay(1000);
}

void loop()
{
    Motors motors = Motors();
}
