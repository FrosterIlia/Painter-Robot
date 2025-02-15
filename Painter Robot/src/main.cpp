#include <Arduino.h>

const int enPin=8;
const int stepXPin = 2; //X.STEP
const int dirXPin = 5; // X.DIR
const int stepYPin = 3; //Y.STEP
const int dirYPin = 6; // Y.DIR
const int stepZPin = 4; //Z.STEP
const int dirZPin = 7; // Z.DIR

int stepPin=stepXPin;
int dirPin=dirXPin;

const int stepsPerRev=200;
int pulseWidthMicros = 10;  // microseconds
int millisBtwnSteps = 100;

void setup() {
  Serial.begin(9600);
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, LOW);
  pinMode(stepXPin, OUTPUT);
  pinMode(dirXPin, OUTPUT);

  pinMode(stepYPin, OUTPUT);
  pinMode(dirYPin, OUTPUT);
 
  Serial.println(F("CNC Shield Initialized"));
}

void loop() {

  static uint16_t steps_number;
  static bool direction;

  static char key;
  if (Serial.available() > 1){
    key = Serial.read();
    Serial.println(key);
    switch (key){
      case 'f':
        steps_number = Serial.parseInt();
        direction = true;
        digitalWrite(dirXPin, direction);
        for (uint16_t i = 0; i < steps_number; i++){
          digitalWrite(stepXPin, HIGH);
          delayMicroseconds(pulseWidthMicros);
          digitalWrite(stepXPin, LOW);
          delayMicroseconds(millisBtwnSteps);
        }
        break;

        case 'b':
          steps_number = Serial.parseInt();
          direction = false;
          digitalWrite(dirXPin, direction);
          for (uint16_t i = 0; i < steps_number; i++){
            digitalWrite(stepXPin, HIGH);
            delayMicroseconds(pulseWidthMicros);
            digitalWrite(stepXPin, LOW);
            delayMicroseconds(millisBtwnSteps);
          }
          break;

          case 'q':
            steps_number = Serial.parseInt();
            direction = true;
            digitalWrite(dirYPin, direction);
            for (uint16_t i = 0; i < steps_number; i++){
              digitalWrite(stepYPin, HIGH);
              delayMicroseconds(pulseWidthMicros);
              digitalWrite(stepYPin, LOW);
              delayMicroseconds(millisBtwnSteps);
            }
            break;

          case 'w':
            steps_number = Serial.parseInt();
            direction = false;
            digitalWrite(dirYPin, direction);
            for (uint16_t i = 0; i < steps_number; i++){
              digitalWrite(stepYPin, HIGH);
              delayMicroseconds(pulseWidthMicros);
              digitalWrite(stepYPin, LOW);
              delayMicroseconds(millisBtwnSteps);
            }
            break;
    }
  }
}