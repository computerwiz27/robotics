#include "hardware.h"
#include "behaviour.h"
#include "debug.h"

volatile uint8_t state;

void setup() {
    Serial.begin(9600);
    delay(100);

    setupHardware();

#if !DEBUG_STATE
    state_initial();
#endif

#if DEBUG_STATE
    sensors.calibrate();
    out.blink(RED, 3);
#endif
}


void loop() {
#if !DEBUG_STATE
    actState();
#endif

#if DEBUG_STATE
    bool *onLine = sensors.areOnLine();

    String state;

    if (onLine[2]) {
        if (!(onLine[0] || onLine[4])) {
            state = String("follow line");
            goto end;
        }

        if (onLine[0] || onLine[4]) {
            state = String("intersection");
            goto end;
        }
    } 
    
    else if (!onLine[2]) {
        // edge case on 90 turns
        if ( onLine[0] == true || onLine[4] == true ) {
            state = String("intersection");
            goto end;
        }

        if (onLine[1] || onLine[3])  {
            state = String("intersection");
            goto end;
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
                state = String("go to start");
                goto end;

            } else {
                state = String("line end");
                goto end;
            }
        }
    }

end:;
    Serial.println(state.c_str());
    for (int i = 0; i < 5; i++ ) {
        Serial.print("[");Serial.print(i);Serial.print("]: ");
        Serial.print(onLine[i]); Serial.print(", ");
    }
    Serial.println();
    Serial.println();

    free(onLine);
    delay(50);
#endif
}
