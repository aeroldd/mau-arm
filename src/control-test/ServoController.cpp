#include "ServoController.h"

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void moveServo(uint8_t channel, int angle)
{
  pwm.setPWM(channel, 0, angleToPulse(angle));
}

// Convert angle to PCA9685 pulse
int angleToPulse(int angle)
{
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

void initServoController() {
  
}