#include "Arduino.h"
#include "Stepper.h"


#define X_STEP_PIN GPIO_NUM_2
#define X_DIR_PIN GPIO_NUM_15
#define Y_STEP_PIN GPIO_NUM_4
#define Y_DIR_PIN GPIO_NUM_5

#define ACCELERATION_REGION 0.2

class Planner{
    public:

        Planner();

        void init_steppers(bool (*_timer_handler1)(void *timerNo), bool (*_timer_handler2)(void *timerNo));

        void tick();

        void move();

        Stepper get_motorX() {return stepper_x;}
        Stepper get_motorY() {return stepper_y;}

        void stop();
        void start();

        int get_velocity() {return _target_vel;}
        void set_velocity(int velocity);

        int get_pos_x() {return stepper_x.get_pos();}
        int get_pos_y() {return stepper_y.get_pos();}

        void set_target_x(float value) {_target_x = value;}
        void set_target_y(float value) {_target_y = value;}

        Stepper stepper_x;
        Stepper stepper_y;

    private:
    

    float _target_vel = 1000; // steps/s
    float _current_vel;

    float _target_x;
    float _target_y;

    bool _is_moving = 0;

    kinematics _x_kinematics;
    kinematics _y_kinematics;

    void calculate_stepper_velocity();
    void calculate_kinematics();


    
    
    
};