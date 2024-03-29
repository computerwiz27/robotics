#ifndef _LINESENSOR_H
#define _LINESENSOR_H

#define MAX_LINE_SENSORS   5

#define EMIT_PIN    11
#define LS_DN1_PIN  12
#define LS_DN2_PIN  18
#define LS_DN3_PIN  20
#define LS_DN4_PIN  21
#define LS_DN5_PIN  22

class LineSensors {
private:
    uint8_t sensor_pins[MAX_LINE_SENSORS] = {
        LS_DN1_PIN,
        LS_DN2_PIN,
        LS_DN3_PIN,
        LS_DN4_PIN,
        LS_DN5_PIN
    };

    float *treshold_vs;

    float calibration_factor;

public:
    LineSensors() {
        pinMode(EMIT_PIN, INPUT);
        for (uint8_t i = 0; i < MAX_LINE_SENSORS; i++) {
            pinMode(sensor_pins[i], INPUT);
        }

        float *_treshold_vs = (float*)malloc(sizeof(float) * MAX_LINE_SENSORS);
        for (uint8_t i = 0; i < MAX_LINE_SENSORS; i++) {
            _treshold_vs[i] = 900;
        }
        treshold_vs = _treshold_vs;
        calibration_factor = 0;
    }

    float *getReadings() {
        float *readings = (float*)malloc(sizeof(float) * MAX_LINE_SENSORS);
        for (uint8_t i = 0; i < MAX_LINE_SENSORS; i++) {
            readings[i] = -1;
        }

        pinMode(EMIT_PIN, OUTPUT);
        digitalWrite(EMIT_PIN, HIGH);

        for (uint8_t i = 0; i < MAX_LINE_SENSORS; i++) {
            pinMode(sensor_pins[i], OUTPUT);
            digitalWrite(sensor_pins[i], HIGH);
        }
        delay(10);

        for (uint8_t i = 0; i < MAX_LINE_SENSORS; i++) {
            pinMode(sensor_pins[i], INPUT);
        }

        uint64_t start_t = micros();
        uint8_t sensors_still_reading = MAX_LINE_SENSORS;
        while(sensors_still_reading > 0) {
            for (uint8_t i = 0; i < MAX_LINE_SENSORS; i++) {
                if (readings[i] < 0) {
                    if (digitalRead(sensor_pins[i]) == LOW) {
                        uint64_t end_t = micros();
                        uint64_t delta_t = end_t -start_t;
                        readings[i] = (float)delta_t;
                        sensors_still_reading --;  
                    }
                }
            }
        }

        pinMode(EMIT_PIN, INPUT);

        return readings;
    }

    float *getReadings(uint8_t from, uint8_t to) {
        if (from > MAX_LINE_SENSORS) {
            return getReadings();
        }
        if (to < from) {
            from = to + from;
            to = from - to;
            from = from - to;
        }
        if (to > MAX_LINE_SENSORS) {
            to = MAX_LINE_SENSORS;
        }

        float *readings = getReadings();

        uint8_t slice_len = to - from + 1;
        float *readings_slice = (float*)malloc(sizeof(float) * slice_len);
        for (int i = 0; i < slice_len; i++) {
            readings_slice[i] = readings[i + from]; 
        }

        free(readings);

        return readings_slice;
    }

    float getReading(uint8_t sensor_no) {
        pinMode(EMIT_PIN, OUTPUT);
        digitalWrite(EMIT_PIN, HIGH);

        pinMode(sensor_pins[sensor_no], OUTPUT);
        digitalWrite(sensor_pins[sensor_no], HIGH);
        delay(10);

        pinMode(sensor_pins[sensor_no], INPUT);

        float reading = 0;
        uint64_t start_t = micros();
        while (digitalRead(sensor_pins[sensor_no]) == HIGH) {

        }
        reading = (float)(micros() - start_t);

        pinMode(EMIT_PIN, INPUT);

        return reading;
    }
    
    float getLineWeight() {
        float *readings = getReadings(1, 3);
        float sum = readings[0] + readings[2];
        float norm_readings[3];
        
        for (uint8_t i = 0; i < 3; i++) {
            norm_readings[i] = readings[i] / sum;
        } 

        free(readings);

        return norm_readings[0] - norm_readings[2] - calibration_factor;
    }

    bool isOnLine(uint8_t sensor_no) {
        uint8_t record_len = 3;

        uint8_t line_record[record_len];

        for (uint8_t i = 0; i < record_len; i++) {
            float reading = this->getReading(sensor_no);
            if (reading > treshold_vs[sensor_no]) {
                line_record[i] = 1;
            }
            else line_record[i] = 0;
        }

        uint8_t record_sum = 0;
        for (uint8_t i = 0; i < record_len; i++) {
            record_sum += line_record[i];
        }

        if (record_sum == record_len) {
            return true;
        }
        return false;
    }

    bool *areOnLine() {
        uint8_t record_len = 5;

        float reading_avg[MAX_LINE_SENSORS];
        for (uint8_t i = 0; i < MAX_LINE_SENSORS; i ++) {
            reading_avg[i] = 0;
        }

        for (uint8_t i = 0; i < record_len; i ++) {
            float *readings = getReadings();
            for (uint8_t j = 0; j < MAX_LINE_SENSORS; j++ ) {
                reading_avg[j] += readings[j] / record_len;
            }
            free(readings);
        }

        bool *onLine = (bool*)malloc(MAX_LINE_SENSORS * sizeof(bool));
        for (uint8_t i = 0; i < MAX_LINE_SENSORS; i++) {
            if(reading_avg[i] > treshold_vs[i]){
                onLine[i] = true;
            } 
            else onLine[i] = false;
        }

        return onLine;
    }
    
    void calibrate() {
        uint8_t readings_no = 100;


        float *light_avgs = this->getReadings();
        for (uint8_t i = 0; i < MAX_LINE_SENSORS; i++) {
            light_avgs[i] = light_avgs[i] / (float)readings_no;
        }

        for (uint8_t i = 1; i < readings_no; i++) {
            float *readings = getReadings();
            for (int i = 0; i < MAX_LINE_SENSORS; i++) {
                light_avgs[i] += readings[i] / (float)readings_no;
            }
            free(readings);
        }

        for (uint8_t i = 0; i < MAX_LINE_SENSORS; i++) {
            treshold_vs[i] = light_avgs[i] * 1.5;
        }

        free(light_avgs);
            
        float line_weight_avg = 0.0;
        for (uint8_t i = 0; i < readings_no; i++) {
            line_weight_avg += this->getLineWeight() / (float)readings_no;
        }

        calibration_factor = line_weight_avg;

    }
};

#endif