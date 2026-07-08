#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Servo pulse limits
#define SERVOMIN 125
#define SERVOMAX 510

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

typedef struct {
  int jointNo;
  double angle;
} Joint;

// base, shoulder, elbow, pitch, wrist revolution
Joint joints[] = {{0,0}, {1,0}, {2,0} ,{3, 0}, {4, 0}};

void setJointAngle(int jointNo, double angle) {
  // set the angles in the struct
  joints[jointNo].angle = angle;

  // move the associated servos for each joint
  // joint 1, which is the shoulder has two opposing servos.
  // joint 2, the elbow has two opposing servos as well.

  switch (jointNo) {
    case 0:
      moveServo(BASE_SERVO, angle);
      break;
    case 1:
      moveServo(SHOULDER_L_SERVO, angle);
      moveServo(SHOULDER_R_SERVO, 180-angle);
      break;
    case 2:
      moveServo(ELBOW_L_SERVO, angle);
      moveServo(ELBOW_R_SERVO, 180 - angle);
      break;
    case 3:
      moveServo(WRIST_PITCH_SERVO, angle);
      break;
    case 4:
      moveServo(GRIPPER_SERVO, angle);
      break;
  }
}

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

// Convert angle to PCA9685 pulse
int angleToPulse(int angle)
{
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

void moveServo(uint8_t channel, int angle)
{
  pwm.setPWM(channel, 0, angleToPulse(angle));
}

void setup()
{
  Serial.begin(115200);

  pwm.begin();
  pwm.setPWMFreq(50);

  delay(1000);

  // // Start all at 0°
  // moveServo(BASE,0);
  // moveServo(SHOULDER_L,0);
  // moveServo(SHOULDER_R,180);
  // moveServo(ELBOW_L,180);
  // moveServo(ELBOW_R,0);
  // moveServo(WRIST_PITCH,0);
  // moveServo(GRIPPER,0);

  // setJointAngle(0, 0);

  // delay(2000);

  // // Move all together from 0° to 90°
  // for(int angle=0; angle<90; angle++)
  // {
  //   moveServo(BASE,angle);
  //   moveServo(SHOULDER_L,angle);
  //   moveServo(SHOULDER_R,180-angle);
  //   moveServo(ELBOW_L,180-angle);
  //   moveServo(ELBOW_R,angle);
  //   moveServo(WRIST_PITCH,angle);
  //   moveServo(GRIPPER,angle);

  //   delay(20);
  // }

  setHome();

  Serial.println("Reached 90 degrees.");
  Serial.println("test");
}

String line;

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

void loop()
{
  if (Serial.available()) {
    line = Serial.readStringUntil('\n');
    line.trim();

    if (line.length() > 0) {
      parseCommand(line);
    }
  }
  //Serial.println("test");
}

