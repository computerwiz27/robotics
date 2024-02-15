#include "hardware.h"
#include "behaviour.h"

#include <math.h>

uint8_t state;

typedef struct coords
{
    float x;
    float y;
    double theta;
} coords_t;

coords_t global;
coords_t local;

float r = 16;
float l = 42.6;

long last_e_l;
long last_e_r;

int ts = 100;

#define STEPS_TO_RAD 0.017536102
#define STEPS_TO_DEG 1.004744627
#define DEG_TO_STPES 0.995277778

double phi_l;

void setup()
{
    setupHardware();

    global = {0, 0, 0};
    local = {0, 0, 0};

    Serial.begin(9600);
    Serial.println("***RESET**");
    delay(1000);

    last_e_l = count_e_l;
    last_e_r = count_e_r;

    turn_in_place(RIGHT, 10);

    while (global.theta < PI)
    {
        double phi_l = (double)(count_e_l - last_e_l) * STEPS_TO_RAD;
        double phi_r = (double)(count_e_r - last_e_r) * STEPS_TO_RAD;
        local.x = (r * (phi_l + phi_r)) / 2.0;
        local.theta = (r * phi_l / (2 * l)) - (r * phi_r / (2 * l));

        global.x = global.x + (local.x * cos(global.theta));
        global.y = global.y + (local.x * sin(global.theta));
        global.theta = global.theta + local.theta;

        last_e_l = count_e_l;
        last_e_r = count_e_r;

        delay(ts);
    }

    stop();

    // phi_l = 0;
    // last_e_l = count_e_l;

    // motors.setPower(5, 0);

    // while(phi_l < 2 * PI){
    //     phi_l +=  (count_e_l - last_e_l) * STEPS_TO_RAD;
    //     last_e_l = count_e_l;
    //     delay(1);
    // }

    // Serial.println(phi_l * DEG_TO_STPES);

    // stop();
}

void loop()
{
}
