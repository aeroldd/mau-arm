#ifndef ROBOTARM_H
#define ROBOTARM_H

// Servo channels (PCA9685)
#define BASE_SERVO            0
#define SHOULDER_L_SERVO      1
#define SHOULDER_R_SERVO      2
#define ELBOW_L_SERVO         3
#define ELBOW_R_SERVO         4
#define WRIST_PITCH_SERVO     5
#define GRIPPER_SERVO         6
#define WRIST_ROTATE_SERVO    7

// Joint definitions
#define BASE          0
#define SHOULDER      1
#define ELBOW         2
#define WRIST_PITCH   3
#define WRIST_ROTATE  4
#define GRIPPER       5

#define NUM_JOINTS 6

typedef struct {
  const char* name;
  const char* servo;     // servo model, shown in the web GUI
  double minAngle;       // soft limits: the arm will never be commanded outside these
  double maxAngle;
  double homeAngle;
  double defaultSpeed;   // deg/s
  double maxSpeed;       // deg/s, hard cap no matter what the GUI asks for
} JointConfig;

extern const JointConfig jointConfig[NUM_JOINTS];

// Load home angles and default speeds from jointConfig
void initArm();

void setJointAngle(int jointNo, double angle);
void setJointSpeed(int jointNo, double degPerSec);

double getJointAngle(int jointNo);   // where the servo is right now
double getJointTarget(int jointNo);  // where it is heading
double getJointSpeed(int jointNo);

bool isMoving();

// Freeze every joint where it currently is
void stopAll();

void moveJointServo(int jointNo, double angle);

// Startup only: write target angles to all servos one joint at a time
// (servo positions are unknown at power-on, so this can't be smoothed)
void writeAllServos();

void updateServos();

#endif
