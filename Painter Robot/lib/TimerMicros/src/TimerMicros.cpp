#include "TimerMicros.h"

TimerMicros::TimerMicros(uint32_t period){
    _period = period;
    _timer = micros();
}

bool TimerMicros::tick(){
    if (micros() - _timer >= _period && _is_running){
        _timer = micros();
        _timer_handler();
        return true;
    }
    return false;
}

void TimerMicros::attach(void (*timer_handler)()){
    _timer_handler = timer_handler;
}