#include "Planner.h"

Planner::Planner() : stepper_x(X_STEP_PIN, X_DIR_PIN, 0), stepper_y(Y_STEP_PIN, Y_DIR_PIN, 1){

}

void Planner::tick(){
    if (_target_x == stepper_x.get_pos() && _target_y == stepper_y.get_pos()) _is_moving = false;
    if (_is_moving){
        // _is_moving = true;
        int steps_moved_x = stepper_x.get_steps_count_set() - stepper_x.get_steps_count() / 2;
        int direction = _x_kinematics.target_vel >= 0 ? 1 : -1;
        // Serial.println(_x_kinematics.min_vel);
        if (steps_moved_x <= _x_kinematics.steps_acc){ // acceleration
            Serial.println((_x_kinematics.max_vel - _x_kinematics.min_vel) / _x_kinematics.steps_acc * steps_moved_x + _x_kinematics.min_vel);
            stepper_x.set_velocity(((_x_kinematics.max_vel - _x_kinematics.min_vel) / _x_kinematics.steps_acc * steps_moved_x + _x_kinematics.min_vel) * direction);
        }
        else if (steps_moved_x > _x_kinematics.steps_acc && steps_moved_x < _x_kinematics.total_steps - _x_kinematics.steps_acc){ // max speed
            Serial.println("MAX SPEED");
            if (stepper_x.get_velocity() != _x_kinematics.max_vel) stepper_x.set_velocity(_x_kinematics.max_vel * direction);
        }
        else if (steps_moved_x >= _x_kinematics.steps_acc){ // deceleration
            Serial.println("decelerating");
            stepper_x.set_velocity((_x_kinematics.max_vel - (_x_kinematics.steps_acc - stepper_x.get_steps_count()) * (_x_kinematics.max_vel - _x_kinematics.min_vel) / _x_kinematics.steps_acc) * direction);
        }
    }
}

void Planner::move(){
    
    calculate_kinematics();
    stepper_x.move_steps(abs(_target_x - stepper_x.get_pos()));
    stepper_y.move_steps(abs(_target_y - stepper_y.get_pos()));
    _is_moving = true;
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
    _target_vel = velocity;
    // calculate_stepper_velocity();
}

void Planner::calculate_stepper_velocity(){
    float angle = atan2(_target_y - stepper_y.get_pos(), _target_x - stepper_x.get_pos());
    stepper_x.set_velocity(_target_vel * cos(angle));
    stepper_y.set_velocity(_target_vel * sin(angle));
}

void Planner::calculate_kinematics(){


    _x_kinematics.total_steps = abs(_target_x - stepper_x.get_pos());
    _y_kinematics.total_steps = abs(_target_y - stepper_y.get_pos());

    float angle = atan2(_target_y - stepper_y.get_pos(), _target_x - stepper_x.get_pos());
    _x_kinematics.target_vel = _target_vel * cos(angle);
    _y_kinematics.target_vel = _target_vel * sin(angle);

    _x_kinematics.t_total = (float)_x_kinematics.total_steps / _target_vel;
    _y_kinematics.t_total = (float)_y_kinematics.total_steps / _target_vel;

    _x_kinematics.min_vel = 1000;
    _y_kinematics.min_vel = 1000;

    _x_kinematics.t_acc = _x_kinematics.t_total * ACCELERATION_REGION;
    _y_kinematics.t_acc = _y_kinematics.t_total * ACCELERATION_REGION;

    // _x_kinematics.max_vel = ((_x_kinematics.total_steps * ACCELERATION_REGION - _x_kinematics.min_vel * _x_kinematics.t_acc) * 2) / _x_kinematics.t_acc + _x_kinematics.min_vel;
    // _y_kinematics.max_vel = ((_y_kinematics.total_steps * ACCELERATION_REGION - _y_kinematics.min_vel * _y_kinematics.t_acc) * 2) / _y_kinematics.t_acc + _y_kinematics.min_vel;

    _x_kinematics.max_vel = 2000;
    _y_kinematics.max_vel = 2000;

}