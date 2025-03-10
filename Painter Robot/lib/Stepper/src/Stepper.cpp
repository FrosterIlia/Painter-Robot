#include "Stepper.h"


Stepper::Stepper(uint8_t step_pin, uint8_t dir_pin, uint8_t timer_number) {

    _step_pin = step_pin;
    _dir_pin = dir_pin;
    pinMode(step_pin, OUTPUT);
    pinMode(dir_pin, OUTPUT);
    
    timer = timerBegin(timer_number, 80, true);
    if (timer == NULL) Serial.println("Stepper timer not initialized");
    else{
        timerAlarmWrite(timer, 10, true); // Set initial frequency (interrupt every 100us)
        timerAlarmEnable(timer); // Enable the timer
        Serial.println("Stepper initialized");
    }
    
}

void Stepper::move_steps(int steps){
    // _dir = _sign(_vel);
    // // Serial.println(_dir);
    // digitalWrite(_dir_pin, _dir);
    // _is_moving = true;
    // _steps_counter = abs(steps) * 2;
    // _steps_counter_set = abs(steps);
}


void Stepper::interruptHandler(){
    if (_is_moving && abs(_steps_counter) > 0){
        digitalWrite(_dir_pin, _steps_counter > 0 ? 1 : 0);
        digitalWrite(_step_pin, _step_flag ? 0 : 1);
        _step_flag = !_step_flag;
        if (_steps_counter > 0){
            _steps_counter--;
            _pos_counter--;
        } 
        else{
            _steps_counter++;
            _pos_counter++;
        } 
        
        if (abs(_pos_counter) >= 2){
            _pos += _pos_counter > 0 ? -1 : 1;
            _pos_counter = 0;
        }
    }
   
}
int Stepper::get_step_interval(){
    return (int)(1/_vel);
}

void Stepper::step(bool dir){
    if (dir) _steps_counter += 2;
    else _steps_counter -= 2;
}

void Stepper::attach_timer_handler(void (*timer_handler)()){
    _timer_handler = timer_handler;
    timerAttachInterrupt(timer, _timer_handler, 1);
    Serial.println("Successfully attached stepper interrupt ");
}

void Stepper::set_velocity(float velocity){

}

float Stepper::get_velocity(){
    return _vel;
}

int Stepper::get_pos(){
    return _pos;
}

void Stepper::start(){
    _is_moving = true;
}

void Stepper::stop(){
    _is_moving = false;
}