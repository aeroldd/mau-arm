#include <Arduino.h>
#include "RobotArm.h"

#include "ServoController.h"

// base, shoulder, elbow, pitch, wrist revolution
Joint joints[] = {{0,0}, {1,0}, {2,0} ,{3, 0}, {4, 0}};
double currentAngles[5] = {90,90,90,90,90};

double targetAngles[5]  = {90,90,90,90,90};

unsigned long lastServoUpdate = 0;

int servoSpeed = 1; // degrees per update
int servoInterval = 20; // ms

void setJointAngle(int jointNo, double angle)
{
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;

  targetAngles[jointNo] = angle;
}

void moveJointServo(int jointNo, double angle)
{
  switch(jointNo)
  {
    case BASE:
      moveServo(BASE_SERVO, angle);
      break;

    case SHOULDER:
      moveServo(SHOULDER_L_SERVO, angle);
      moveServo(SHOULDER_R_SERVO, 180-angle);
      break;

    case ELBOW:
      moveServo(ELBOW_L_SERVO, angle);
      moveServo(ELBOW_R_SERVO, 180-angle);
      break;

    case WRIST:
      moveServo(WRIST_PITCH_SERVO, angle);
      break;

    case GRIPPER:
      moveServo(GRIPPER_SERVO, angle);
      break;
  }
}


void updateServos()
{
  if (millis() - lastServoUpdate < servoInterval)
    return;

  lastServoUpdate = millis();


  for (int i = 0; i < 5; i++)
  {
    if (currentAngles[i] < targetAngles[i])
    {
      currentAngles[i] += servoSpeed;

      if(currentAngles[i] > targetAngles[i])
        currentAngles[i] = targetAngles[i];
    }


    if (currentAngles[i] > targetAngles[i])
    {
      currentAngles[i] -= servoSpeed;

      if(currentAngles[i] < targetAngles[i])
        currentAngles[i] = targetAngles[i];
    }


    moveJointServo(i, currentAngles[i]);
  }
}