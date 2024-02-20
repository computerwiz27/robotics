#ifndef _PID_H
#define _PID_H

#include <math.h>

#include "kinematics.h"
#include "motors.h"

#ifndef _KINEMATICS_H 
#define MEASURE_TS      20
#define LINE_FOLLOW_POW 15
#define TURN_POW        15
#endif


class PID_controller {
public:
    float p_gain;
    float i_gain;
    float d_gain;

    float last_error;

    float i_term;
    float d_term;

protected:
    volatile float *measurment;
    
    float integral;
    float demand;

public:
    PID_controller(volatile float *measurement, float p_gain, float i_gain, float d_gain) {
        last_error = 0;
        integral = 0;

        demand = 0;

        this->measurment = measurement;

        this->p_gain = p_gain;
        this->i_gain = i_gain;
        this->d_gain = d_gain;
    }

    virtual void setDemand(float demand) {
        this->demand = demand;
    }

    virtual void update(){}

protected:
    float getFeedback(float error) {
        float p_term = p_gain * error;

        integral = integral + error * (float)(MEASURE_TS/100.0);
        i_term = integral * i_gain;
        // if (i_term > demand * 1.1) {
        //     i_term = demand * 1.1;
        // }

        d_term = (error - last_error) / (float)(MEASURE_TS/1000.0) * d_gain;

        float feedback = p_term + i_term + d_term;

        last_error = error;

        return feedback;
    }

    float getFeedback() {
        float error = demand - *measurment;
        return getFeedback(error);
    }
};


class MotorController : public PID_controller {
private:
    Motors *motors;
    uint8_t pos;

public:
    MotorController(Motors *motors, uint8_t position, volatile float *measurement, float p_gain, float i_gain, float d_gain) 
        : PID_controller(measurement, p_gain, i_gain, d_gain) {
        this->motors = motors;
        this->pos = position;    
    }

    void setDemand(float demand) override {
        this->integral = 0;
        this->demand = demand / 100.0 * MAX_ANGULAR_V;
    }

    void update() override {
        float feedback = this->getFeedback();
        int power = feedback / MAX_ANGULAR_V * 100;

        if (this->pos == LEFT) {
            int other_motor_power = motors->getPower()[1];
            motors->setPower(power, other_motor_power);
        } else if (this->pos == RIGHT) {
            int other_motor_power = motors->getPower()[0];
            motors->setPower(other_motor_power, power);
        }
    }
};

class HeadingController : public PID_controller {
private:
    bool active;
    MotorController *motor_l_pid;
    MotorController *motor_r_pid;

public:
    HeadingController(MotorController *motor_l_pid, MotorController *motor_r_pid, float p_gain, float i_gain, float d_gain) :
        PID_controller(&global_coords.th, p_gain, i_gain, d_gain) {
        active = false;

        this->motor_l_pid = motor_l_pid;
        this->motor_r_pid = motor_r_pid;
    }

    void setActive(bool state) {
        this->active = state;
    }

    void setDemand(float demand) override {
        this->demand = demand;
    }

    void setDemand(float x, float y) {
        this->demand = -1 * atan2(x,y);
    }

    void update() {
        float err = *measurment - demand;
        if (err < -1 * PI) {
            err += 2 * PI;
        }
        if (err > PI) {
            err -= 2 * PI;
        }

        float feedback = -1 * getFeedback(err);

        float power_l = TURN_POW * feedback;
        float power_r = TURN_POW * -1 * feedback;

        float m = global_coords.th;

        Serial.print(demand * RAD_TO_DEG); Serial.print(",");
        Serial.print(m* RAD_TO_DEG); Serial.print(",");
        // Serial.print(err * RAD_TO_DEG); Serial.print(",");
        Serial.println(d_term * -RAD_TO_DEG); Serial.print(",");

        this->motor_l_pid->setDemand(power_l);
        this->motor_r_pid->setDemand(power_r);
    }
};

#endif