#include "Arduino.h"
#include "Stepper.h"


#define X_STEP_PIN GPIO_NUM_2
#define X_DIR_PIN GPIO_NUM_15
#define Y_STEP_PIN GPIO_NUM_4
#define Y_DIR_PIN GPIO_NUM_5

class Planner{
    public:

        Planner();

        void init_steppers(bool (*_timer_handler1)(void *timerNo), bool (*_timer_handler2)(void *timerNo));

        void tick();

        void move(float x, float y);

        Stepper get_motorX() {return stepper_x;}
        Stepper get_motorY() {return stepper_y;}

        void stop();
        void start();

        int get_velocity() {return _vel;}
        void set_velocity(int velocity);

        int get_pos_x() {return stepper_x.get_pos();}
        int get_pos_y() {return stepper_y.get_pos();}

        Stepper stepper_x;
        Stepper stepper_y;

    private:
    

    float _vel = 100;

    int _vel_x;
    int _vel_y;

    int _target_x;
    int _target_y;
    
};