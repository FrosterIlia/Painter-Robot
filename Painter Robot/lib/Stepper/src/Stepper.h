/**
 * @file Stepper.h
 * @author Ilia Moroz (iliamorozim@gmail.com)
 * @brief Class for convenient stepper motor control
 * @version 1.0
 * @date 2025-07-05
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
#include "Arduino.h"
#include "Timer.h"

#define DRIVER_STEP_TIME 500

#define _sign(x) ((x) >= 0 ? 1 : 0)

class Stepper
{
public:
    Stepper(uint8_t step_pin, uint8_t dir_pin, uint8_t timer_number);

    void step(bool dir);
    void move_steps(int steps);

    void stop();
    void start();

    void interruptHandler();

    void set_velocity(float velocity);
    float get_velocity();
    int get_pos();

    int get_steps_count() { return _steps_counter; }
    int get_steps_count_set() { return _steps_counter_set; }

    void attach_timer_handler(void (*timer_handler)());

    hw_timer_t *timer = NULL;

private:
    uint8_t _step_pin;
    uint8_t _dir_pin;
    volatile bool _step_flag = false;
    bool _dir;
    bool _is_moving;
    volatile int _steps_counter;
    int _steps_counter_set;
    int8_t _pos_counter = 0; // need to count pos every 2 interrupts

    int _vel = DRIVER_STEP_TIME;

    volatile int _pos = 0;

    void (*_timer_handler)();
    int get_step_interval();
};
