#include "WebServerControl.h"
#include "ServoController.h"
#include "Pose.h"
#include "CommandParser.h"
#include "RobotArm.h"

void setup()
{
  Serial.begin(115200);

  initServoController();
  Serial.println("servo controller init!");

  setHome();
  writeAllServos();
  Serial.println("set to home");

  initWebServer();
  Serial.println("setup finished");
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

  updateWebServerControl();

  updateServos();
}
