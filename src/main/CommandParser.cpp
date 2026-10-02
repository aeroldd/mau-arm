#include "CommandParser.h"
#include <Arduino.h>
#include "Pose.h"
#include "RobotArm.h"

bool verifyJoint(int joint) {
  if (joint >= 0 && joint < NUM_JOINTS) {
    return true;
  }
  return false;
}

bool verifyAngle(double angle) {
  if (angle >= 0 && angle <= 180) {
    return true;
  }
  return false;
}

void parseCommand(String line) {
  int joint = -1;
  double angle = -1;

  // SET COMMAMDS

  if (line.equalsIgnoreCase("HOME")) {
    Serial.println("Set to home position.");
    setHome();
    return;
  }

  if (sscanf(line.c_str(), "%d %lf", &joint, &angle) != 2) {
    Serial.println("Invalid command. Use: <joint> <angle>  or  HOME");
    return;
  }

  if (verifyJoint(joint)) {
    if (verifyAngle(angle)) {
      setJointAngle(joint, angle);
    }
    else {
      Serial.println("Invalid angle.");
      return;
      }
  }
  else {
    Serial.println("Invalid joint.");
    return;
  }
  Serial.print("joint: ");
  Serial.println(joint);
  Serial.print("angle: ");
  Serial.println(angle);
}