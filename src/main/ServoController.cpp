#include "ServoController.h"

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Per-channel trim in degrees, added to every command for that channel.
// Use this to line up the opposing shoulder (1/2) and elbow (3/4) pairs so
// they don't fight each other at the same commanded angle.
const double channelTrim[16] = {
  0, // 0  base
  0, // 1  shoulder L
  0, // 2  shoulder R
  0, // 3  elbow L
  0, // 4  elbow R
  0, // 5  wrist pitch
  0, // 6  gripper
  0, // 7  wrist rotate
  0, 0, 0, 0, 0, 0, 0, 0
};

void moveServo(uint8_t channel, double angle)
{
  if (channel > 15) return;

  angle += channelTrim[channel];
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;

  pwm.setPWM(channel, 0, angleToPulse(angle));
}

// Convert angle to PCA9685 pulse
int angleToPulse(double angle)
{
  return (int)lround(SERVOMIN + (SERVOMAX - SERVOMIN) * angle / 180.0);
}

void initServoController() {
  pwm.begin();
  pwm.setPWMFreq(50);
}
