#include "Planner.h"

Planner::Planner() : stepper_x(X_STEP_PIN, X_DIR_PIN, 0), stepper_y(Y_STEP_PIN, Y_DIR_PIN, 1){

}


void Planner::move(float x, float y){
    // _target_x = x;
    // _target_y = y;

    float angle = atan2((float)y, (float)x);
    Serial.println(angle);
    
    // stepper_x.set_velocity(_vel * sin(angle));
    // stepper_y.set_velocity(_vel * cos(angle));
    stepper_x.move_steps(x - stepper_x.get_pos());
    stepper_y.move_steps(y - stepper_y.get_pos());
}

void Planner::init_steppers(bool (*_timer_handler1)(void *timerNo), bool (*_timer_handler2)(void *timerNo)){
    stepper_x = Stepper(X_STEP_PIN, X_DIR_PIN, 0);
    stepper_y = Stepper(Y_STEP_PIN, Y_DIR_PIN, 2);
    stepper_x.attach_timer_handler(*_timer_handler1);
    stepper_y.attach_timer_handler(*_timer_handler2);
}

void Planner::stop(){
    stepper_x.stop();
    stepper_y.stop();
}

void Planner::start(){
    stepper_x.start();
    stepper_y.start();
}

void Planner::set_velocity(int velocity){
    stepper_x.set_velocity(velocity);
    stepper_y.set_velocity(velocity);
}