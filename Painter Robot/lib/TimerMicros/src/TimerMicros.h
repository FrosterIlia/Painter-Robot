#pragma once
#include <Arduino.h>

class TimerMicros{
  public:

    TimerMicros(uint32_t period = 1000);
    bool tick();
    void set_period(uint32_t new_period) {_period = new_period;}
    void attach(void (*timer_handler)());
    void disable_timer() {_is_running = 0;}
    void enable_timer() {_is_running = 1;}

    uint32_t get_period() {return _period;}


  private:
    uint32_t _period;
    uint32_t _timer;
    bool _is_running = 1;

    void (*_timer_handler)();

};
