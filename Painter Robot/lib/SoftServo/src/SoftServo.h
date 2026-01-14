/**
 * @file SoftServo.h
 * @author Ilia Moroz (iliamorozim@gmail.com)
 * @brief Class for generating impulses for servo control in software
 * @version 1.0
 * @date 2025-07-05
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once
#include <Arduino.h>

class SoftServo
{
public:
    void attach(int pin, int min = 500, int max = 2400)
    {
        _pin = pin;
        pinMode(_pin, OUTPUT);
        _attached = true;
        _min = min;
        _max = max;
        _tmr50 = millis();
        _tmrUs = micros();
        _flag = 0;
    }

    void detach()
    {
        _attached = false;
    }

    void asyncMode()
    {
        _mode = true;
    }

    void delayMode()
    {
        _mode = false;
    }

    bool tick()
    {
        if (_attached && millis() - _tmr50 >= 20)
        {
            if (_mode)
            {
                if (!_flag)
                {
                    _tmrUs = micros();
                    _flag = 1;
                    fastWrite(_pin, 1);
                }
                else
                {
                    if (micros() - _tmrUs >= _us)
                    {
                        fastWrite(_pin, 0);
                        _flag = 0;
                        _tmr50 = millis();
                    }
                    else
                        return true;
                }
            }
            else
            {
                _tmr50 = millis();
                fastWrite(_pin, 1);
                delayMicroseconds(_us);
                fastWrite(_pin, 0);
            }
        }
        return false;
    }

    void write(int value)
    {
        if (value < 200)
            value = map(value, 0, 180, _min, _max);
        writeMicroseconds(value);
    }

    void writeMicroseconds(int us)
    {
        _us = us;
    }

    int read()
    {
        return map(_us, _min, _max, 0, 180);
    }

    int readMicroseconds()
    {
        return _us;
    }

    bool attached()
    {
        return _attached;
    }

private:
    void fastWrite(const uint8_t pin, bool val)
    {

        digitalWrite(pin, val);
    }

    uint8_t _pin = 255;
    int _us = 700, _min, _max;
    bool _attached = 0, _mode = 0, _flag = 0;
    uint32_t _tmr50, _tmrUs;
};
