#include "Pose.h"
#include "RobotArm.h"

void setPose(
  double base,
  double shoulder,
  double elbow,
  double wristPitch,
  double wristRotate,
  double gripper
) {
  setJointAngle(BASE, base);
  setJointAngle(SHOULDER, shoulder);
  setJointAngle(ELBOW, elbow);
  setJointAngle(WRIST_PITCH, wristPitch);
  setJointAngle(WRIST_ROTATE, wristRotate);
  setJointAngle(GRIPPER, gripper);
}

void setHome() {
  for (int i = 0; i < NUM_JOINTS; i++)
    setJointAngle(i, jointConfig[i].homeAngle);
}
