#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// WIFI SHIT
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Aejaz";
const char* password = "mirdif786";

WebServer server(80);

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void handleRoot()
{
  String page = 
  "<!DOCTYPE html>"
  "<html>"
  "<head>"
  "<title>ESP32 Robot Arm</title>"
  "</head>"

  "<body>"
  "<h1>Robot Arm Controller</h1>"

  "<form action='/send' method='GET'>"
  "<label>Command:</label><br>"
  "<input type='text' name='cmd' placeholder='Example: 0 90'>"
  "<br><br>"
  "<button type='submit'>Send</button>"
  "</form>"

  "<br>"
  "<a href='/home'>Set Home</a>"

  "</body>"
  "</html>";

  server.send(200, "text/html", page);
}

void handleSend()
{
  if (server.hasArg("cmd"))
  {
    String cmd = server.arg("cmd");

    Serial.print("WEB CMD: ");
    Serial.println(cmd);

    parseCommand(cmd);

    server.send(200, "text/html",
    "<h2>Command sent</h2><a href='/'>Back</a>");
  }
  else
  {
    server.send(400, "text/plain", "No command received");
  }
}

void handleHome()
{
  setHome();

  server.send(200, "text/html",
  "<h2>Robot moved to Home</h2><a href='/'>Back</a>");
}

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
double currentAngles[5] = {90,90,90,90,90};

void setJointAngle(int jointNo, double targetAngle)
{
  targetAngle = constrain(targetAngle, 0, 180);

  double startAngle = currentAngles[jointNo];

  int steps = abs(targetAngle - startAngle);

  if (steps == 0) return;

  for (int i = 0; i <= steps; i++)
  {
    double angle = startAngle + 
      (targetAngle - startAngle) * i / steps;


    switch (jointNo)
    {
      case BASE:
        moveServo(BASE_SERVO, angle);
        break;

      case SHOULDER:
        moveServo(SHOULDER_L_SERVO, angle);
        moveServo(SHOULDER_R_SERVO, 180-angle);
        break;

      case ELBOW:
        moveServo(ELBOW_L_SERVO, angle);
        moveServo(ELBOW_R_SERVO, 180-angle);
        break;

      case WRIST:
        moveServo(WRIST_PITCH_SERVO, angle);
        break;

      case GRIPPER:
        moveServo(GRIPPER_SERVO, angle);
        break;
    }

    delay(20); // speed control
  }

  currentAngles[jointNo] = targetAngle;
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

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP Address: ");

  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
server.on("/send", handleSend);
server.on("/home", handleHome);

server.onNotFound([](){
  server.send(404, "text/plain", "Page not found");
});

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

  server.begin();

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
  server.handleClient();
}

