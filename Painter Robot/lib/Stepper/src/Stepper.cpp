#include "Stepper.h"


Stepper::Stepper(uint8_t step_pin, uint8_t dir_pin, uint8_t timer_number) : timer(timer_number) {

    _step_pin = step_pin;
    _dir_pin = dir_pin;
    pinMode(step_pin, OUTPUT);
    pinMode(dir_pin, OUTPUT);
    Serial.println("Initializing stepper");

}

void Stepper::move_steps(int steps){
    _dir = _sign(_vel);
    // Serial.println(_dir);
    digitalWrite(_dir_pin, _dir);
    _is_moving = true;
    _steps_counter = abs(steps) * 2;
    _steps_counter_set = abs(steps);
}


void Stepper::interruptHandler(){
    if (_is_moving && _steps_counter > 0){
        digitalWrite(_step_pin, _step_flag ? 0 : 1);
        _step_flag = !_step_flag;
        _steps_counter--;
        _pos_counter += _dir ? 1 : -1;
        if (abs(_pos_counter) >= 2){
            _pos += _dir ? 1 : -1;
            _pos_counter = 0;
        }
    }
   
}
int Stepper::get_step_interval(){
    return (int)(1/_vel);
}

void Stepper::attach_timer_handler(bool (*timer_handler)(void *timerNo)){
    _timer_handler = timer_handler;
    timer.attachInterruptInterval(DRIVER_STEP_TIME, _timer_handler);
    Serial.println("successfully attached interrupt ");
}

void Stepper::set_velocity(float velocity){
    _vel = velocity;
    _dir = _sign(_vel);
    digitalWrite(_dir_pin, _dir);
    if (abs(_vel) < 30) {
        timer.disableTimer();
        return;
    } 
    else {
        timer.enableTimer();
    }
    if (_timer_handler == nullptr) {
        Serial.println("Error: Timer handler not attached!");
        return;
    }
    timer.disableTimer();
    timer.setInterval((int)abs(1000000 / (abs(_vel)*2)), _timer_handler);
    timer.enableTimer();
    
}

float Stepper::get_velocity(){
    return _vel;
}

int Stepper::get_pos(){
    return _pos;
}

void Stepper::start(){
    Serial.println(_steps_counter);
    _is_moving = true;
}

void Stepper::stop(){
    _is_moving = false;
}