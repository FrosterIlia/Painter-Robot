#include "Planner.h"

Planner::Planner() : stepper_x(X_STEP_PIN, X_DIR_PIN, 0), stepper_y(Y_STEP_PIN, Y_DIR_PIN, 1){
}

void Planner::tick(){
    if (_target_x == stepper_x.get_pos() && _target_y == stepper_y.get_pos()) _is_moving = false;
    else{
        if (_is_moving){
            int steps_moved_x = stepper_x.get_steps_count_set() - stepper_x.get_steps_count() / 2;
            int direction_x = _x_kinematics.target_vel >= 0 ? 1 : -1;
            if (steps_moved_x <= _x_kinematics.steps_acc){ // acceleration
                stepper_x.set_velocity((_x_kinematics.min_vel + steps_moved_x * (_x_kinematics.max_vel - _x_kinematics.min_vel) / _x_kinematics.steps_acc) * direction_x);
            }
            else if (steps_moved_x > _x_kinematics.steps_acc && steps_moved_x < _x_kinematics.total_steps - _x_kinematics.steps_acc){ // max speed
                if (stepper_x.get_velocity() != _x_kinematics.max_vel) stepper_x.set_velocity(_x_kinematics.max_vel * direction_x);
            }
            else if (stepper_x.get_steps_count() <= _x_kinematics.steps_acc){ // deceleration
                stepper_x.set_velocity((_x_kinematics.max_vel - (_x_kinematics.steps_acc - stepper_x.get_steps_count()) * (_x_kinematics.max_vel - _x_kinematics.min_vel) / _x_kinematics.steps_acc) * direction_x);
            }

            int steps_moved_y = stepper_y.get_steps_count_set() - stepper_y.get_steps_count() / 2;
            int direction_y = _y_kinematics.target_vel >= 0 ? 1 : -1;
            if (steps_moved_y <= _y_kinematics.steps_acc){ // acceleration
                stepper_y.set_velocity((_y_kinematics.min_vel + steps_moved_y * (_y_kinematics.max_vel - _y_kinematics.min_vel) / _y_kinematics.steps_acc) * direction_y);
            }
            else if (steps_moved_y > _y_kinematics.steps_acc && steps_moved_y < _y_kinematics.total_steps - _y_kinematics.steps_acc){ // max speed
                if (stepper_y.get_velocity() != _y_kinematics.max_vel) stepper_y.set_velocity(_y_kinematics.max_vel * direction_y);
            }
            else if (stepper_y.get_steps_count() <= _y_kinematics.steps_acc){ // deceleration
                stepper_y.set_velocity((_y_kinematics.max_vel - (_y_kinematics.steps_acc - stepper_y.get_steps_count()) * (_y_kinematics.max_vel - _y_kinematics.min_vel) / _y_kinematics.steps_acc) * direction_y);
            }
        }
    }
    
}

void Planner::move(){
    
    compute_kinematics();
    stepper_x.move_steps(abs(_target_x - stepper_x.get_pos()));
    stepper_y.move_steps(abs(_target_y - stepper_y.get_pos()));
    _is_moving = true;
}

void Planner::init_steppers(void (*_timer_handler1)(), void (*_timer_handler2)()){
    stepper_x.attach_timer_handler(_timer_handler1);
    stepper_y.attach_timer_handler(_timer_handler2);
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
}


void Planner::compute_kinematics(){

    _x_kinematics.steps_acc = 1000;
    _y_kinematics.steps_acc = 1000;

    _x_kinematics.total_steps = abs(_target_x - stepper_x.get_pos());
    _y_kinematics.total_steps = abs(_target_y - stepper_y.get_pos());

    float angle = atan2(_target_y - stepper_y.get_pos(), _target_x - stepper_x.get_pos());
    _x_kinematics.target_vel = _target_vel * cos(angle);
    _y_kinematics.target_vel = _target_vel * sin(angle);

    //TODO calculate min_vel and max_vel in terms of target_vel
    _x_kinematics.min_vel = 700;
    _y_kinematics.min_vel = 700;

    _x_kinematics.max_vel = 3000;
    _y_kinematics.max_vel = 3000;

}