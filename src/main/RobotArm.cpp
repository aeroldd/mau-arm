#include <Arduino.h>
#include "RobotArm.h"

#include "ServoController.h"

// ---- Tune these to your arm's mechanics ----
// minAngle/maxAngle: set them to where each joint hits something (frame, cables,
// the table, the other links) and back off a few degrees.
// Gripper: limit the closed end so the MG90S doesn't stall against its fingers.
const JointConfig jointConfig[NUM_JOINTS] = {
  // name            servo              min   max  home  speed  maxSpeed
  { "Base",          "DS15",              0,  180,  90,    45,    90 },
  { "Shoulder",      "2x MG996R",         0,  180,  90,    30,    60 },
  { "Elbow",         "2x MG995",          0,  180,  90,    30,    60 },
  { "Wrist Pitch",   "MG996R",            0,  180,  90,    60,   120 },
  { "Wrist Rotate",  "MG90S",             0,  180,  90,    90,   180 },
  { "Gripper",       "MG90S",             0,  180,  90,    90,   180 },
};

double currentAngles[NUM_JOINTS];

double targetAngles[NUM_JOINTS];

double jointSpeeds[NUM_JOINTS]; // deg/s

unsigned long lastServoUpdate = 0;

const unsigned long servoInterval = 20; // ms

// If the loop stalls (e.g. a slow web request), don't jump to catch up
const unsigned long maxStepTime = 50; // ms

static bool validJoint(int jointNo)
{
  return jointNo >= 0 && jointNo < NUM_JOINTS;
}

void initArm()
{
  for (int i = 0; i < NUM_JOINTS; i++)
  {
    currentAngles[i] = jointConfig[i].homeAngle;
    targetAngles[i]  = jointConfig[i].homeAngle;
    jointSpeeds[i]   = jointConfig[i].defaultSpeed;
  }
}

void setJointAngle(int jointNo, double angle)
{
  if (!validJoint(jointNo)) return;

  const JointConfig& cfg = jointConfig[jointNo];
  if (angle < cfg.minAngle) angle = cfg.minAngle;
  if (angle > cfg.maxAngle) angle = cfg.maxAngle;

  targetAngles[jointNo] = angle;
}

void setJointSpeed(int jointNo, double degPerSec)
{
  if (!validJoint(jointNo)) return;

  if (degPerSec < 1) degPerSec = 1;
  if (degPerSec > jointConfig[jointNo].maxSpeed) degPerSec = jointConfig[jointNo].maxSpeed;

  jointSpeeds[jointNo] = degPerSec;
}

double getJointAngle(int jointNo)  { return validJoint(jointNo) ? currentAngles[jointNo] : 0; }
double getJointTarget(int jointNo) { return validJoint(jointNo) ? targetAngles[jointNo] : 0; }
double getJointSpeed(int jointNo)  { return validJoint(jointNo) ? jointSpeeds[jointNo] : 0; }

bool isMoving()
{
  for (int i = 0; i < NUM_JOINTS; i++)
    if (currentAngles[i] != targetAngles[i]) return true;
  return false;
}

void stopAll()
{
  for (int i = 0; i < NUM_JOINTS; i++)
    targetAngles[i] = currentAngles[i];
}

void moveJointServo(int jointNo, double angle)
{
  switch(jointNo)
  {
    case BASE:
      moveServo(BASE_SERVO, angle);
      break;

    // Opposing pairs: both sides are always written together
    case SHOULDER:
      moveServo(SHOULDER_L_SERVO, angle);
      moveServo(SHOULDER_R_SERVO, 180-angle);
      break;

    case ELBOW:
      moveServo(ELBOW_L_SERVO, angle);
      moveServo(ELBOW_R_SERVO, 180-angle);
      break;

    case WRIST_PITCH:
      moveServo(WRIST_PITCH_SERVO, angle);
      break;

    case WRIST_ROTATE:
      moveServo(WRIST_ROTATE_SERVO, angle);
      break;

    case GRIPPER:
      moveServo(GRIPPER_SERVO, angle);
      break;
  }
}

void writeAllServos()
{
  // Light joints first, heavy ones last, with a pause between each so all
  // servos don't draw stall current at the same moment (brownout).
  const int order[NUM_JOINTS] = { GRIPPER, WRIST_ROTATE, WRIST_PITCH, ELBOW, SHOULDER, BASE };

  for (int k = 0; k < NUM_JOINTS; k++)
  {
    int i = order[k];
    currentAngles[i] = targetAngles[i];
    moveJointServo(i, currentAngles[i]);
    delay(300);
  }
}

void updateServos()
{
  unsigned long now = millis();
  unsigned long elapsed = now - lastServoUpdate;

  if (elapsed < servoInterval)
    return;

  lastServoUpdate = now;

  if (elapsed > maxStepTime)
    elapsed = maxStepTime;

  for (int i = 0; i < NUM_JOINTS; i++)
  {
    if (currentAngles[i] == targetAngles[i])
      continue;

    double step = jointSpeeds[i] * elapsed / 1000.0;

    if (currentAngles[i] < targetAngles[i])
    {
      currentAngles[i] += step;

      if (currentAngles[i] > targetAngles[i])
        currentAngles[i] = targetAngles[i];
    }
    else
    {
      currentAngles[i] -= step;

      if (currentAngles[i] < targetAngles[i])
        currentAngles[i] = targetAngles[i];
    }

    moveJointServo(i, currentAngles[i]);
  }
}
