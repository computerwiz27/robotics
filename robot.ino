#include "hardware.h"
#include "behaviour.h"

uint8_t state;

void setup()
{
    Serial.begin(9600);
    delay(1000);

    setupHardware();
}

void loop()
{
    // Serial.println(global_coords.th * RAD_TO_DEG);
    // delay(1000);
}
