#include "WebServerControl.h"
#include "ServoController.h"
#include "Pose.h"
#include "CommandParser.h"
#include "RobotArm.h"

void setup()
{
  Serial.begin(115200);
  initWebServer();

  setHome();
}

String line;

void loop()
{
  if (Serial.available()) {
    line = Serial.readStringUntil('\n');
    line.trim();

    if (line.length() > 0) {
      parseCommand(line);
    }
  }
  //Serial.println("test");
  updateWebServerControl();

  updateServos();
}
