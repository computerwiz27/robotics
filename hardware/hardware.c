#include "../hardware.h"

void setupHardware() {
    setupEncoders();

    motors = Motors();
    sensors = LineSensors();
    in = Input();
    out = Output();
}