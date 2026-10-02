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

bool verifyAngle(int joint, double angle) {
  if (angle >= jointConfig[joint].minAngle && angle <= jointConfig[joint].maxAngle) {
    return true;
  }
  return false;
}

void printHelp() {
  Serial.println("Commands:");
  Serial.println("  <joint> <angle>        move a joint (0 base, 1 shoulder, 2 elbow,");
  Serial.println("                         3 wrist pitch, 4 wrist rotate, 5 gripper)");
  Serial.println("  SPEED <joint> <deg/s>  set joint speed");
  Serial.println("  HOME                   go to home pose");
  Serial.println("  STOP                   freeze all joints where they are");
}

void parseCommand(String line) {
  int joint = -1;
  double angle = -1;
  double speed = -1;

  // SET COMMAMDS

  if (line.equalsIgnoreCase("HOME")) {
    Serial.println("Set to home position.");
    setHome();
    return;
  }

  if (line.equalsIgnoreCase("STOP")) {
    Serial.println("Stopped.");
    stopAll();
    return;
  }

  if (line.equalsIgnoreCase("HELP")) {
    printHelp();
    return;
  }

  if (sscanf(line.c_str(), "SPEED %d %lf", &joint, &speed) == 2 ||
      sscanf(line.c_str(), "speed %d %lf", &joint, &speed) == 2) {
    if (!verifyJoint(joint)) {
      Serial.println("Invalid joint.");
      return;
    }
    setJointSpeed(joint, speed);
    Serial.print(jointConfig[joint].name);
    Serial.print(" speed: ");
    Serial.print(getJointSpeed(joint));
    Serial.println(" deg/s");
    return;
  }

  if (sscanf(line.c_str(), "%d %lf", &joint, &angle) != 2) {
    Serial.println("Invalid command. Type HELP for commands.");
    return;
  }

  if (verifyJoint(joint)) {
    if (verifyAngle(joint, angle)) {
      setJointAngle(joint, angle);
    }
    else {
      Serial.print("Invalid angle. ");
      Serial.print(jointConfig[joint].name);
      Serial.print(" range is ");
      Serial.print(jointConfig[joint].minAngle);
      Serial.print(" - ");
      Serial.println(jointConfig[joint].maxAngle);
      return;
      }
  }
  else {
    Serial.println("Invalid joint.");
    return;
  }
  Serial.print("joint: ");
  Serial.println(jointConfig[joint].name);
  Serial.print("angle: ");
  Serial.println(angle);
}
