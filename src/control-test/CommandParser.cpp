#include "CommandParser.h"
#include <Arduino.h>
#include "Pose.h"
#include "RobotArm.h"

bool verifyJoint(int joint) {
  if (joint >= 0 && joint <= 5) {
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
  int joint;
  double angle;
  sscanf(line.c_str(), "%d %lf", &joint, &angle);

  // SET COMMAMDS

  if (line.equals("HOME")) {
    Serial.println("Set to home position.");
    setPose(90,90,90,90,90);
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
  } 
  Serial.print("joint: ");
  Serial.println(joint);
  Serial.print("angle: ");
  Serial.println(angle);
}