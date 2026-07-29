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
String page = R"rawliteral(
<!DOCTYPE html>
<html>

<head>

<title>Robot Arm Control</title>

<style>

body {
  font-family: Arial;
  background:#222;
  color:white;
  text-align:center;
}

.container {
  width:400px;
  margin:auto;
}

.joint {
  background:#333;
  padding:15px;
  margin:10px;
  border-radius:10px;
}

input[type=range] {
  width:90%;
}

.value {
  font-size:20px;
  color:#00ff99;
}

button {
  padding:12px 25px;
  font-size:18px;
  margin:10px;
  border-radius:8px;
}

.home {
  background:#00aa55;
  color:white;
}

</style>

</head>


<body>

<div class="container">

<h1>Robot Arm</h1>


<div class="joint">
<h3>Base</h3>
<input type="range" min="0" max="180" value="90"
oninput="update(0,this.value)">
<p class="value" id="j0">90°</p>
</div>


<div class="joint">
<h3>Shoulder</h3>
<input type="range" min="0" max="180" value="90"
oninput="update(1,this.value)">
<p class="value" id="j1">90°</p>
</div>


<div class="joint">
<h3>Elbow</h3>
<input type="range" min="0" max="180" value="90"
oninput="update(2,this.value)">
<p class="value" id="j2">90°</p>
</div>


<div class="joint">
<h3>Wrist</h3>
<input type="range" min="0" max="180" value="90"
oninput="update(3,this.value)">
<p class="value" id="j3">90°</p>
</div>


<div class="joint">
<h3>Gripper</h3>
<input type="range" min="0" max="180" value="90"
oninput="update(4,this.value)">
<p class="value" id="j4">90°</p>
</div>


<button class="home" onclick="home()">
HOME
</button>


</div>


<script>

function update(joint,angle)
{
 document.getElementById("j"+joint).innerHTML=angle+"°";

 fetch("/servo?joint="+joint+"&angle="+angle);
}


function home()
{
 fetch("/home");
}

</script>


</body>
</html>
)rawliteral";


server.send(200,"text/html",page);

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

void handleServo()
{
  int joint = server.arg("joint").toInt();
  int angle = server.arg("angle").toInt();

  setJointAngle(joint, angle);

  server.send(200,"text/plain","OK");
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

double targetAngles[5]  = {90,90,90,90,90};

unsigned long lastServoUpdate = 0;

int servoSpeed = 1; // degrees per update
int servoInterval = 20; // ms

void setJointAngle(int jointNo, double angle)
{
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;

  targetAngles[jointNo] = angle;
}

void updateServos()
{
  if (millis() - lastServoUpdate < servoInterval)
    return;

  lastServoUpdate = millis();


  for (int i = 0; i < 5; i++)
  {
    if (currentAngles[i] < targetAngles[i])
    {
      currentAngles[i] += servoSpeed;

      if(currentAngles[i] > targetAngles[i])
        currentAngles[i] = targetAngles[i];
    }


    if (currentAngles[i] > targetAngles[i])
    {
      currentAngles[i] -= servoSpeed;

      if(currentAngles[i] < targetAngles[i])
        currentAngles[i] = targetAngles[i];
    }


    moveJointServo(i, currentAngles[i]);
  }
}

void moveJointServo(int jointNo, double angle)
{
  switch(jointNo)
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
  server.on("/servo", handleServo);
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

  updateServos();
}

