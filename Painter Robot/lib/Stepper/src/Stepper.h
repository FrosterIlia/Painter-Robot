#pragma once
#include "Arduino.h"
#include <ESP32_New_TimerInterrupt.h>
#include "Timer.h"


#define DRIVER_STEP_TIME 500

#define _sign(x) ((x) >= 0 ? 1 : 0) 

class Stepper {
public:

    Stepper(uint8_t step_pin, uint8_t dir_pin, uint8_t timer_number);

    void step();

    void move_steps(int steps);

    void stop();

    void start();

    void interruptHandler();

    void set_velocity(float velocity);
    float get_velocity();
    int get_pos();

    int get_steps_count() {return _steps_counter;}
    int get_steps_count_set() {return _steps_counter_set;}

    void attach_timer_handler(bool (*timer_handler)(void *timerNo));
    
    ESP32Timer timer;

private:
    uint8_t _step_pin;
    uint8_t _dir_pin;
    volatile bool _step_flag = false;
    bool _dir;
    bool _is_moving;
    volatile uint16_t _steps_counter;
    uint16_t _steps_counter_set;
    int8_t _pos_counter = 0; // need to count pos every 2 interrupts

    int _vel = DRIVER_STEP_TIME;

    volatile int _pos = 0;

    bool (*_timer_handler)(void *timerNo);
    int get_step_interval();

    // kinematics _kinematics;
    
};

struct kinematics{
    float t_total;
    float t_acc;
    int total_steps;
    int steps_acc = 300;
    float target_vel;
    float min_vel;
    float max_vel;

};