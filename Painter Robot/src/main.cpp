#include <Arduino.h>
#include <LittleFS.h>
#include "SoftServo.h"

#include "Stepper.h"
#include "GyverPortal.h"
#include "Timer.h"
#include "TimerMicros.h"
#include "Planner.h"
#include "FileSystem.h"
#include "Parser.h"
#include "SerialPlotter.h"

#define SERVO_PIN GPIO_NUM_23

#define SERVO_UP_POSITION 0
#define SERVO_DOWN_POSITION 70

#define SERVO_MIN_IMPULSE 1100
#define SERVO_MAX_IMPULSE 2000

FileSystem file_system;
GyverPortal portal(&LittleFS);

Planner planner(2);
Parser parser;

SoftServo pen_servo;

void IRAM_ATTR TimerHandler1()
{
  planner.stepper_x.interruptHandler();
}

void IRAM_ATTR TimerHandler2()
{
  planner.stepper_y.interruptHandler();
}

void IRAM_ATTR PlannerTimerHandler()
{
  planner.interruptHandler();
}

void setupPortal();
bool paint_cmd();

void build()
{
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP.SLIDER("slider_vel", 500, 0, 1000, 0.1, 2);

  GP.BUTTON("start", "Start");
  GP.BUTTON("stop", "Stop");

  GP.SLIDER("slider_x", 0, 0, 7000, 1);
  GP.SLIDER("slider_y", 0, 0, 7000, 1);

  GP.FILE_UPLOAD("Upload Intructions");

  GP.BUILD_END();
}

void action()
{
  if (portal.click())
  {
    if (portal.click("slider_vel"))
    {
      planner.set_target_velocity(portal.getInt("slider_vel"));
    }

    if (portal.click("slider_x"))
    {
      planner.set_target_x(portal.getFloat("slider_x"));
    }

    if (portal.click("slider_y"))
    {
      planner.set_target_y(portal.getFloat("slider_y"));
    }

    if (portal.click("start"))
    {
      planner.move();
      planner.start();
    }

    if (portal.click("stop"))
    {
      planner.stop();
    }
  }

  if (portal.upload())
  {
    Serial.print("Upload: ");
    Serial.print(portal.fileName());
    Serial.print(", from: ");
    Serial.println(portal.uploadName());
    portal.saveFile('/' + portal.fileName());
  }

  if (portal.uploadEnd())
  {
    Serial.print("Uploaded file: ");
    Serial.print(portal.fileName());
    Serial.print(", from: ");
    Serial.println(portal.uploadName());
  }
}

Timer plotter_timer(100);
Timer paintning_timer(120);

SerialPlotter<1> plotter;

void setup()
{
  Serial.begin(115200);

  planner.init_steppers(&TimerHandler1, &TimerHandler2);
  planner.attach_interrupt_handler(&PlannerTimerHandler);
  setupPortal();
  pen_servo.attach(SERVO_PIN, SERVO_MIN_IMPULSE, SERVO_MAX_IMPULSE);
  pen_servo.delayMode();
  pen_servo.write(SERVO_UP_POSITION);
  Serial.println(F("CNC Shield Initialized"));

  if (parser.open("/test.txt"))
  {
    Serial.println("Parser successfully opened the file");
  }
  else
  {
    Serial.println("Parser failed to open the file");
  }
  planner.start();

  auto& plot1 = plotter.add_plot<3, int>("P");
  plot1.attach_parameter("pos_x", []() { return planner.stepper_x.get_pos(); });
  plot1.attach_parameter("pos_y", []() { return planner.stepper_y.get_pos(); });
  plot1.attach_parameter("vel", []() { return planner.get_current_velocity(); });

}

bool painting = false;

void loop()
{
  portal.tick();
  pen_servo.tick();

  if (painting && planner.done_moving())
  {
    delay(1);
    if (!paint_cmd())
    {
      painting = false;
    }
  }

  char key;
  if (Serial.available() > 0)
  {
    key = Serial.read();
    switch (key)
    {
    case 'f':
      file_system.print_file_system_contents();
      break;

    case 'n':
    {
      paint_cmd();
      break;
    }

    case 'p':
      painting = !painting;
      break;

    case 'o':
      file_system.print_file("/test.txt");
      break;

    case 'u':
    {
      pen_servo.write(SERVO_UP_POSITION);
      break;
    }

    case 'd':
      pen_servo.write(SERVO_DOWN_POSITION);
      break;

    case 'x':
    {
      int x_pos = Serial.parseInt();

      planner.set_target_x(x_pos);
      Serial.println(F("x_pos set"));
      break;
    }
    case 'y':
    {
      int y_pos = Serial.parseInt();

      planner.set_target_y(y_pos);
      Serial.println(F("y_pos set"));
      break;
    }
    case 's':
      Serial.println(F("Start"));
      planner.move();
      break;
    }
  }

  plotter.plot();
}

void setupPortal()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin("BCIT Robotics Club", "IWillBuildARobot");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  portal.attachBuild(build);
  portal.attach(action);
  portal.start();
}


bool paint_cmd()
{
  Parser::Command cmd = parser.get_next_cmd();
  Pos position = parser.get_target_pos();

  Serial.print((uint8_t)cmd);
  Serial.printf("Position: %d:%d\r\n", position.x, position.y);

  switch (cmd)
  {
  case Parser::Command::UP:
    Serial.println(F("Servo up"));
    delay(500);
    pen_servo.write(SERVO_UP_POSITION);
    break;

  case Parser::Command::DOWN:
    pen_servo.write(SERVO_DOWN_POSITION);
    delay(500);
    Serial.println(F("Servo down"));
    break;

  case Parser::Command::MOVE:
    planner.set_target_x(position.x);
    planner.set_target_y(position.y);
    planner.move();
    planner.start();
    break;

  case Parser::Command::INVALID:
    Serial.println(F("Invalid Command"));
    break;

  case Parser::Command::END_OF_FILE:
    Serial.println(F("End of File"));
    return false;
  }
  return true;
}