#include <Arduino.h>

#include "Stepper.h"
#include "GyverPortal.h"
#include "Timer.h"
#include "TimerMicros.h"
#include "Planner.h"

#define VIBRATION_AMOUNT 40
#define VIBRATION_STEPS 200

GyverPortal portal;

Planner planner(2);

void IRAM_ATTR TimerHandler1(){
  planner.stepper_x.interruptHandler();
}

void IRAM_ATTR TimerHandler2(){
  planner.stepper_y.interruptHandler();
}

void IRAM_ATTR PlannerTimerHandler(){
  planner.interruptHandler();
}

void vibrate(uint16_t value);

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
          planner.set_target_velocity(portal.getInt("slider_vel"));
      }

      if (portal.click("slider_x")){
        planner.set_target_x(portal.getFloat("slider_x"));
        Serial.println(portal.getFloat("slider_x"));
      }

      if (portal.click("slider_y")){
        planner.set_target_y(portal.getFloat("slider_y"));
      }

      if (portal.click("start")) {
        planner.move(); 
        planner.start();
      }

      if (portal.click("stop")) {
        planner.stop();
      }
  }
}

Timer plotter_timer(100);

void setup() {
  Serial.begin(115200);

  planner.init_steppers(&TimerHandler1, &TimerHandler2);
  planner.attach_interrupt_handler(&PlannerTimerHandler);
  setupPortal();
  Serial.println(F("CNC Shield Initialized"));
}

uint32_t myTimer = millis();

enum POSITIONS {PINK, GREEN, BLUE, WHITE};

int positions_x[] = {0, 5000, 0, 5000};
int positions_y[] = {0, 0, 5000, 5000};

bool moving = false;

void loop() {

  portal.tick();
  
  if (moving && planner.done_moving()){
    moving = false;
    Serial.println("done");
    // vibrate(VIBRATION_AMOUNT);
  }

  static int steps_number;
  static char key;
  if (Serial.available() > 1){
    key = Serial.read();
    Serial.println(key);
    switch (key){
      case 'f':
        steps_number = Serial.parseInt();
        Serial.println(steps_number);
        planner.start();
        for (int i = 0; i < abs(steps_number); i++){
          planner.stepper_x.step(_sign(steps_number));
          planner.stepper_y.step(_sign(steps_number));
        }
        break;

      case 'm':
        int position = Serial.parseInt();
        if (position == 4){
          planner.start();
          vibrate(VIBRATION_AMOUNT);
          Serial.println("vibrated");
          break;
        }

        planner.set_target_x(positions_x[position]);
        planner.set_target_y(positions_y[position]);

        planner.move();
        planner.start();

        moving = true;
      break;

    }
  }

  // if (plotter_timer.isReady()){
  //   Serial.print("{P(pos_x:");
  //   Serial.print(planner.get_pos_x());
  //   Serial.print(",pos_y:");
  //   Serial.print(planner.get_pos_y());
  //   Serial.print(",vel:");
  //   Serial.print(planner.get_current_velocity());
  //   Serial.print(")}");
  // }
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

void vibrate(uint16_t value){
  for (uint16_t i = 0; i < value; i++){
    for (int j = 0; j < abs(VIBRATION_STEPS); j++){
      planner.stepper_x.step(_sign(VIBRATION_STEPS));
      planner.stepper_y.step(_sign(VIBRATION_STEPS));
    }
    delay(25);

    for (int j = 0; j < abs(-VIBRATION_STEPS); j++){
      planner.stepper_x.step(_sign(-VIBRATION_STEPS));
      planner.stepper_y.step(_sign(-VIBRATION_STEPS));
    }
    delay(25);
  }
}