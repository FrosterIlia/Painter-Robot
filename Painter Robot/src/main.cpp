#include <Arduino.h>


#define _TIMERINTERRUPT_LOGLEVEL_     4
#include <ESP32_New_TimerInterrupt.h>
#include "Stepper.h"
#include "GyverPortal.h"
#include "Timer.h"

GyverPortal portal;

Stepper motor_x(X_STEP_PIN, X_DIR_PIN, 0);
Stepper motor_y(Y_STEP_PIN, Y_DIR_PIN, 2);

bool IRAM_ATTR TimerHandler1(void *timerNo){
  motor_x.interruptHandler();
  return true;
}

bool IRAM_ATTR TimerHandler2(void *timerNo){
  motor_y.interruptHandler();
  return true;
}

void setupPortal();

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP.SLIDER("slider_vel", 500, 0, 1000, 0.1, 2);

  GP.BUTTON("start", "Start");
  GP.BUTTON("stop", "Stop");

  GP.SLIDER("slider_x", 0, 0, 7000, 1);
  GP.SLIDER("slider_y", 0, 0, 7000, 1);

  GP.BUILD_END();
}

void action() {
  if (portal.click()) {
      if (portal.click("slider_vel")){
          motor_x.set_velocity(portal.getFloat("slider_vel"));
          motor_y.set_velocity(portal.getFloat("slider_vel"));
          Serial.println(motor_x.get_velocity());
      }

      if (portal.click("slider_x")){
        motor_x.move_steps(portal.getInt("slider_x") - motor_x.get_pos());
      }

      if (portal.click("slider_y")){
        motor_y.move_steps(portal.getInt("slider_y") - motor_y.get_pos());
      }


      if (portal.click("start")) {
        motor_x.start();
        motor_y.start();
      }

      if (portal.click("stop")) {
        motor_x.stop();
        motor_y.stop();
      }
  }
}

Timer plotter_timer(100);

void setup() {
  Serial.begin(9600);

  motor_x.attach_timer_handler(TimerHandler1);
  motor_y.attach_timer_handler(TimerHandler2);
  setupPortal();
  Serial.println(F("CNC Shield Initialized"));
}

void loop() {
  portal.tick();
  static uint16_t steps_number;
  static char key;
  if (Serial.available() > 1){
    key = Serial.read();
    Serial.println(key);
    switch (key){
      case 'f':
        steps_number = Serial.parseInt();
        motor_x.move_steps(steps_number);
        break;

      case 'b':
        steps_number = Serial.parseInt();
        motor_x.move_steps(-steps_number);
        break;

      case 'q':
        steps_number = Serial.parseInt();
        motor_y.move_steps(steps_number);
        break;

      case 'w':
        steps_number = Serial.parseInt();
        motor_y.move_steps(-steps_number);
        break;

      case 'v':
        motor_x.set_velocity(Serial.parseInt());
        motor_y.set_velocity(motor_x.get_velocity());
        break;
    }
  }

  if (plotter_timer.isReady()){
    Serial.print("{P(pos_x:");
    Serial.print(motor_x.get_pos());
    Serial.print(",pos_y:");
    Serial.print(motor_y.get_pos());
    Serial.print(")}");
  }
}

void setupPortal(){
  WiFi.mode(WIFI_STA);
  WiFi.begin("BCIT Robotics Club", "IWillBuildARobot");
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  portal.attachBuild(build);
  portal.attach(action);
  portal.start();
}