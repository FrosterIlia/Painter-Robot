#include <Arduino.h>
#include <LittleFS.h>

#include "Stepper.h"
#include "GyverPortal.h"
#include "Timer.h"
#include "TimerMicros.h"
#include "Planner.h"
#include "FileSystem.h"
#include "Parser.h"

FileSystem file_system;
GyverPortal portal(&LittleFS);

Planner planner(2);
Parser parser;

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
void plot_graph();

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
      Serial.println(portal.getFloat("slider_x"));
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

void setup()
{
  Serial.begin(115200);

  planner.init_steppers(&TimerHandler1, &TimerHandler2);
  planner.attach_interrupt_handler(&PlannerTimerHandler);
  setupPortal();
  Serial.println(F("CNC Shield Initialized"));

  if (parser.open("/test.txt"))
  {
    Serial.println("Parser successfully opened the file");
  }
  else
  {
    Serial.println("Parser failed to open the file");
  }

}

void loop()
{

  portal.tick();

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
      while (!parser.is_done())
      {
        Parser::Command command = parser.get_next_cmd();
        Pos position = parser.get_target_pos();
        Serial.printf("%d: %d, %d\r\n", command, position.x, position.y);
      }

      break;
    
    case 'o':
      file_system.print_file("/test.txt");
      break;
    }
  }

  // plot_graph();
}

void setupPortal()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin("dlink-7850", "wsusa58776");
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

void plot_graph()
{
  if (plotter_timer.isReady())
  {
    Serial.print("{P(pos_x:");
    Serial.print(planner.stepper_x.get_pos());
    Serial.print(",pos_y:");
    Serial.print(planner.stepper_y.get_pos());
    Serial.print(",vel:");
    Serial.print(planner.get_current_velocity());
    Serial.print(")}");
  }
}
