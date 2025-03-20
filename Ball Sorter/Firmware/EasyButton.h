#pragma once
#include "pigpio.h"
#include <opencv2/opencv.hpp>

#define BUTTON_1 9
#define BUTTON_2 11

class EasyButton {
  public:
    EasyButton(int pin) {
      _mainTimer = cv::getTickCount() / cv::getTickFrequency() * 1000;
      _clickFlag = false;
      _holdFlag = false;
      _holdTimer = cv::getTickCount() / cv::getTickFrequency() * 1000;
      _pin = pin;
      _clickDebounce = 10;
      _holdTimeout = 1000;
    }

    void init(){
      gpioSetMode(_pin, PI_INPUT);
      gpioSetPullUpDown(_pin, PI_PUD_UP);
    }

    void tick() {
      if (!gpioRead(_pin) && _clickFlag == false) {
        _clickFlag = true;
        _mainTimer = cv::getTickCount() / cv::getTickFrequency() * 1000;
      }
      if (gpioRead(_pin) && _clickFlag == true && cv::getTickCount() / cv::getTickFrequency() * 1000 - _mainTimer >= _clickDebounce) {
        _clickFlag = false;
      }
      if (!gpioRead(_pin) && _holdFlag == false){
        _holdTimer = cv::getTickCount() / cv::getTickFrequency() * 1000;
        _holdFlag = true;
      }
      if (gpioRead(_pin) && _holdFlag == true) {
        _holdFlag = false;
      }
    }

    bool isClick() {
      if (gpioRead(_pin) && _clickFlag == true && cv::getTickCount() / cv::getTickFrequency() * 1000 - _mainTimer >= _clickDebounce && cv::getTickCount() / cv::getTickFrequency() * 1000 - _mainTimer <= _holdTimeout) {
        _clickFlag = false;
        return true;
      }
      return false;
    }
    bool get_state() {
      return !gpioRead(_pin);
    }

    bool isHolded() {

      if (!gpioRead(_pin) && _holdFlag == true && cv::getTickCount() / cv::getTickFrequency() * 1000 - _holdTimer >= _holdTimeout) {
        _holdTimer = cv::getTickCount() / cv::getTickFrequency();
        _holdFlag = false;
        return true;
      }
      return false;
    }
    bool isHold(){
      if (_clickFlag == true && cv::getTickCount() / cv::getTickFrequency() * 1000 - _holdTimer >= _holdTimeout){
        return true;
      }
      return false;
    }
    void set_pin(int pin){
      _pin = pin;
    }
    int get_pin(){
      return _pin;
    }
  private:
    double _mainTimer;
    double _holdTimer;
    bool _clickFlag;
    bool _holdFlag;
    int _pin;
    double _clickDebounce;
    double _holdTimeout;
};
