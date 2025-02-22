#include <Arduino.h>


#define _TIMERINTERRUPT_LOGLEVEL_     4
#include <ESP32_New_TimerInterrupt.h>
#include "Stepper.h"
#include "GyverPortal.h"
#include "Timer.h"
#include "Planner.h"

GyverPortal portal;

Planner planner;

bool IRAM_ATTR TimerHandler1(void *timerNo){
  planner.stepper_x.interruptHandler();
  return true;
}

bool IRAM_ATTR TimerHandler2(void *timerNo){
  planner.stepper_y.interruptHandler();
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
          planner.set_velocity(portal.getInt("slider_vel"));
      }

      if (portal.click("slider_x")){
        planner.move(portal.getInt("slider_x"), portal.getInt("slider_y")); 
        Serial.println(portal.getInt("slider_x"));
      }

      if (portal.click("slider_y")){
        planner.move(portal.getInt("slider_x"), portal.getInt("slider_y")); 
      }

      if (portal.click("start")) {
        planner.start();
      }

      if (portal.click("stop")) {
        planner.stop();
      }
  }
}

Timer plotter_timer(100);

void setup() {
  Serial.begin(9600);

  planner.init_steppers(TimerHandler1, TimerHandler2);
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
        // motor_x.move_steps(steps_number);
        break;

      case 'b':
        steps_number = Serial.parseInt();
        // motor_x.move_steps(-steps_number);
        break;

      case 'q':
        steps_number = Serial.parseInt();
        // motor_y.move_steps(steps_number);
        break;

      case 'w':
        steps_number = Serial.parseInt();
        // motor_y.move_steps(-steps_number);
        break;

      case 'v':
        planner.set_velocity(Serial.parseInt());
        break;
    }
  }

  if (plotter_timer.isReady()){
    Serial.print("{P(pos_x:");
    Serial.print(planner.get_pos_x());
    Serial.print(",pos_y:");
    Serial.print(planner.get_pos_y());
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