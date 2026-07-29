#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include "WebServerControl.h"
#include "CommandParser.h"
#include "RobotArm.h"

const char* ssid = "Aejaz";
const char* password = "mirdif786";

WebServer server(80);

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
  parseCommand("HOME");

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

void initWebServer() {
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

  server.begin();
}