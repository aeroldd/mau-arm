#ifndef SERVOCONTROLLER_H
#define SERVOCONTROLLER_H

#include <Arduino.h>

// Servo pulse limits
#define SERVOMIN 125
#define SERVOMAX 510

typedef struct {
  int jointNo;
  double angle;
} Joint;

// Convert angle to PCA9685 pulse
int angleToPulse(int angle);

void moveServo(uint8_t channel, int angle);

void initServoController();

#endif