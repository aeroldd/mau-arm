#ifndef SERVOCONTROLLER_H
#define SERVOCONTROLLER_H

#include <Arduino.h>

// Servo pulse limits (PCA9685 ticks at 50Hz, 4096 ticks = 20ms)
#define SERVOMIN 125
#define SERVOMAX 510

// Convert angle to PCA9685 pulse
int angleToPulse(double angle);

// Write an angle (0-180) to a PCA9685 channel, applying that channel's trim
void moveServo(uint8_t channel, double angle);

void initServoController();

#endif
