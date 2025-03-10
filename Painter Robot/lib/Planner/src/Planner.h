#include "Arduino.h"
#include "Stepper.h"


#define X_STEP_PIN GPIO_NUM_19
#define X_DIR_PIN GPIO_NUM_18
#define Y_STEP_PIN GPIO_NUM_4
#define Y_DIR_PIN GPIO_NUM_5

#define ACCELERATION_REGION 0.2

class Planner{
    public:

        Planner(int timer_number);

        void init_steppers(void (*_timer_handler1)(), void (*_timer_handler2)());

        void attach_interrupt_handler(void (*timer_handler)());

        void tick();

        void move();

        void stop();
        void start();

        int get_target_velocity() {return _target_vel;}
        int get_current_velocity() {return _current_vel;}

        void set_target_velocity(int velocity);
        void set_current_velocity(int velocity);

        int get_pos_x() {return stepper_x.get_pos();}
        int get_pos_y() {return stepper_y.get_pos();}

        void set_target_x(int value) {_target_x = value;}
        void set_target_y(int value) {_target_y = value;}

        void interruptHandler();

        Stepper stepper_x;
        Stepper stepper_y;

    private:
    
        hw_timer_t *_timer = NULL;
        void (*_timer_handler)();

        volatile int _target_vel = 1000; // steps/s

        volatile int _min_vel = 500;
        volatile int _max_vel = 1500;
        volatile int _steps_accel;
        volatile int _current_vel = _target_vel;

        int _target_x;
        int _target_y;

        volatile int _dx, _dy;
        volatile int8_t _dir;
        volatile int _x0, _y0, _x1, _y1;

        volatile int _movement_counter = 0;

        void draw_line_h();
        void draw_line_v();

};