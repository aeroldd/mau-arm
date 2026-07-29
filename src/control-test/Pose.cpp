#include "Pose.h"
#include "RobotArm.h"

void setPose(
  double base,
  double shoulder,
  double elbow,
  double wrist,
  double gripper
) {
  setJointAngle(BASE, base);
  setJointAngle(SHOULDER, shoulder);
  setJointAngle(ELBOW, elbow);
  setJointAngle(WRIST, wrist);
  setJointAngle(GRIPPER, gripper);
}

void setHome() {
  setPose(90,90,90,90,90);
}