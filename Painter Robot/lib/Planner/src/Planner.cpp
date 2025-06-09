#include "Planner.h"

Planner::Planner(int timer_number) : stepper_x(X_STEP_PIN, X_DIR_PIN, 0), stepper_y(Y_STEP_PIN, Y_DIR_PIN, 1)
{

    _timer = timerBegin(timer_number, 80, true);
    if (_timer == NULL)
        Serial.println("Planner timer not initialized");
    else
    {
        timerAlarmWrite(_timer, 1000000 / (2 * _current_vel), true);
        timerAlarmEnable(_timer); // Enable the timer
        Serial.println("Planner initialized");
    }
}

void Planner::tick()
{
    interruptHandler();
}

void Planner::attach_interrupt_handler(void (*timer_handler)())
{
    _timer_handler = timer_handler;
    timerAttachInterrupt(_timer, _timer_handler, 1);
    Serial.println("Successfully attached planner interrupt ");
}

void Planner::move()
{

    _x0 = stepper_x.get_pos();
    _x1 = _target_x;

    _y0 = stepper_y.get_pos();
    _y1 = _target_y;

    _dx = _x1 - _x0;
    _dy = _y1 - _y0;

    if (abs(_x0 - _x1) > abs(_y0 - _y1))
    { // h
        _dir = _dy < 0 ? -1 : 1;
        _movement_counter = abs(_dx);
    }
    else
    { // v
        _dir = _dx < 0 ? -1 : 1;
        _movement_counter = abs(_dy);
    }

    set_current_velocity(_min_vel);

    _steps_accel = _movement_counter * 0.2;
}

void Planner::init_steppers(void (*_timer_handler1)(), void (*_timer_handler2)())
{
    stepper_x.attach_timer_handler(_timer_handler1);
    stepper_y.attach_timer_handler(_timer_handler2);
}

void Planner::stop()
{
    stepper_x.stop();
    stepper_y.stop();
}

void Planner::start()
{
    stepper_x.start();
    stepper_y.start();
}

void Planner::set_current_velocity(int velocity)
{
    _current_vel = velocity;
    timerAlarmWrite(_timer, 1000000 / (2 * _current_vel), true);
}

void Planner::set_target_velocity(int velocity)
{
    _target_vel = velocity;
}

void Planner::interruptHandler()
{

    if (_movement_counter > 0)
    {
        _movement_counter--;
        if (abs(_x0 - _x1) > abs(_y0 - _y1))
        {
            draw_line_h();
            int x = abs(_dx) - _movement_counter;

            // Acceleration
            if (x < _steps_accel)
            {
                set_current_velocity(_min_vel + x * (_max_vel - _min_vel) / _steps_accel);
            }
            else if (x >= _steps_accel && _movement_counter >= _steps_accel)
            {
                set_current_velocity(_max_vel);
            }
            else
            {
                set_current_velocity(_max_vel - (_steps_accel - abs(_dx) + x) * (_max_vel - _min_vel) / _steps_accel);
            }
        }
        else
        {
            draw_line_v();

            int y = abs(_dy) - _movement_counter;

            // Acceleration
            if (y < _steps_accel)
            {
                set_current_velocity(_min_vel + y * (_max_vel - _min_vel) / _steps_accel);
            }
            else if (y >= _steps_accel && _movement_counter >= _steps_accel)
            {
                set_current_velocity(_max_vel);
            }
            else
            {
                set_current_velocity(_max_vel - (_steps_accel - abs(_dy) + y) * (_max_vel - _min_vel) / _steps_accel);
            }
        }
    }
}

void Planner::draw_line_h()
{
    if (_dx != 0)
    {

        if (_dx == _dy)
        {
            stepper_x.step(_sign(_dx));
            stepper_y.step(_sign(_dir));
        }
        else
        {
            // Fixed-point arithmetic for y2 calculation
            int32_t delta_x = stepper_x.get_pos() - _x0 + _dir;
            int32_t y2 = (_dy * delta_x) / _dx + _y0;

            stepper_x.step(_sign(_dx));
            if (abs(stepper_y.get_pos() + _dir - y2) <= abs(stepper_y.get_pos() - y2))
            {
                stepper_y.step(_sign(_dir));
            }
        }
    }
}

void Planner::draw_line_v()
{

    if (_dy != 0)
    {
        if (_dx == _dy)
        {
            stepper_y.step(_sign(_dy));
            stepper_x.step(_sign(_dir));
        }
        else
        {
            // Fixed-point arithmetic for x2 calculation
            int32_t delta_y = stepper_y.get_pos() - _y0 + _dir;
            int32_t x2 = (_dx * delta_y) / _dy + _x0;

            stepper_y.step(_sign(_dy));
            if (abs(stepper_x.get_pos() + _dir - x2) <= abs(stepper_x.get_pos() - x2))
            {
                stepper_x.step(_sign(_dir));
            }
        }
    }
}

bool Planner::done_moving()
{
    return _movement_counter == 0;
}