#ifndef ROBOTARM_H
#define ROBOTARM_H

// Servo channels
#define BASE_SERVO            0
#define SHOULDER_L_SERVO      1
#define SHOULDER_R_SERVO      2
#define ELBOW_L_SERVO         3
#define ELBOW_R_SERVO         4
#define WRIST_PITCH_SERVO     5
#define GRIPPER_SERVO         6

// Joint definitions
#define BASE 0
#define SHOULDER 1
#define ELBOW 2
#define WRIST 3
#define GRIPPER 4

#define NUM_JOINTS 5

void setJointAngle(int jointNo, double angle);

void moveJointServo(int jointNo, double angle);

// Immediately write target angles to all servos (no smoothing)
void writeAllServos();

void updateServos();

#endif