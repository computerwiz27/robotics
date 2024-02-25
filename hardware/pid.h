#ifndef _PID_H
#define _PID_H

#include <math.h>

#include "kinematics.h"
#include "motors.h"

#ifndef _KINEMATICS_H 
#define MEASURE_TS      20
#define LINE_FOLLOW_POW 13
#define TURN_POW        13
#endif


class PID_controller {
private:
    float p_gain;
    float i_gain;
    float d_gain;


protected:
    volatile float *measurment;
    
    float last_error;
    float integral;
public:
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
        float i_term = integral * i_gain;

        float d_term = (error - last_error) / (float)(MEASURE_TS/1000.0) * d_gain;

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
    int8_t pos;

public:
    MotorController(Motors *motors, uint8_t position, volatile float *measurement, float p_gain, float i_gain, float d_gain) 
        : PID_controller(measurement, p_gain, i_gain, d_gain) {
        this->motors = motors;
        this->pos = position;    
    }

    void setDemand(float demand) override {
        this->last_error = 0;
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

#define HEADING_TURN    0
#define HEADING_GOTO    1

class HeadingController : public PID_controller {
private:
    // bool active;
    uint8_t mode;

    MotorController *motor_l_pid;
    MotorController *motor_r_pid;

public:
    bool active;

public:
    HeadingController(MotorController *motor_l_pid, MotorController *motor_r_pid, float p_gain, float i_gain, float d_gain) :
        PID_controller(&global_coords.th, p_gain, i_gain, d_gain) {
        active = false;
        mode = HEADING_TURN;

        this->motor_l_pid = motor_l_pid;
        this->motor_r_pid = motor_r_pid;
    }

    void setActive(bool state) {
        this->active = state;
    }

    void setMode(uint8_t mode) {
        this->mode = mode;
    }

    void setDemand(float demand) override {
        this->integral = 0;
        this->demand = demand;
    }

    void setDemand(float x, float y) {
        float new_demand = atan2(y - global_coords.y, x - global_coords.x);

        if (new_demand > PI) {
            new_demand -= 2*PI;
        }
        if (new_demand < PI) {
            new_demand += 2*PI;
        }


        this->demand = new_demand;
    }

    void update() {
        if (!active) return;

        float err = *measurment - demand;
        if (err > PI)   err -= 2 * PI;
        if (err < -1 * PI) err += 2 * PI;

        float feedback = -1 * getFeedback(err);

        float power_l = TURN_POW * feedback + mode * LINE_FOLLOW_POW;
        float power_r = TURN_POW * -1 * feedback + mode * LINE_FOLLOW_POW;

        this->motor_l_pid->setDemand(power_l);
        this->motor_r_pid->setDemand(power_r);
    }

};

#endif