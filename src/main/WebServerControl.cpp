#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include "WebServerControl.h"
#include "CommandParser.h"
#include "RobotArm.h"
#include "Pose.h"
#include "WebPage.h"

const char* ssid = "Aejaz";
const char* password = "mirdif786";

WebServer server(80);

const unsigned long WIFI_TIMEOUT_MS = 15000;
bool webServerStarted = false;

void handleRoot()
{
  server.send_P(200, "text/html", INDEX_HTML);
}

// JSON with every joint's config and live state, used by the GUI
void sendState()
{
  String json = "{\"moving\":";
  json += isMoving() ? "true" : "false";
  json += ",\"joints\":[";

  for (int i = 0; i < NUM_JOINTS; i++)
  {
    const JointConfig& cfg = jointConfig[i];
    if (i > 0) json += ",";
    json += "{\"name\":\""; json += cfg.name;
    json += "\",\"servo\":\""; json += cfg.servo;
    json += "\",\"min\":";    json += String(cfg.minAngle, 1);
    json += ",\"max\":";       json += String(cfg.maxAngle, 1);
    json += ",\"home\":";      json += String(cfg.homeAngle, 1);
    json += ",\"maxSpd\":";    json += String(cfg.maxSpeed, 0);
    json += ",\"spd\":";       json += String(getJointSpeed(i), 0);
    json += ",\"cur\":";       json += String(getJointAngle(i), 1);
    json += ",\"tgt\":";       json += String(getJointTarget(i), 1);
    json += "}";
  }

  json += "]}";
  server.send(200, "application/json", json);
}

void handleState()
{
  sendState();
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
  server.send(200, "text/plain", "OK");
}

void handleStop()
{
  stopAll();
  sendState();
}

// /pose?a=base,shoulder,elbow,wristPitch,wristRotate,gripper
// All six angles are required so a half-sent pose never moves the arm.
void handlePose()
{
  if (!server.hasArg("a"))
  {
    server.send(400, "text/plain", "Missing pose");
    return;
  }

  double a[NUM_JOINTS];
  int n = sscanf(server.arg("a").c_str(), "%lf,%lf,%lf,%lf,%lf,%lf",
                 &a[0], &a[1], &a[2], &a[3], &a[4], &a[5]);

  if (n != NUM_JOINTS)
  {
    server.send(400, "text/plain", "Pose needs 6 angles");
    return;
  }

  for (int i = 0; i < NUM_JOINTS; i++)
  {
    if (a[i] < jointConfig[i].minAngle || a[i] > jointConfig[i].maxAngle)
    {
      server.send(400, "text/plain", String(jointConfig[i].name) + " out of range");
      return;
    }
  }

  setPose(a[BASE], a[SHOULDER], a[ELBOW], a[WRIST_PITCH], a[WRIST_ROTATE], a[GRIPPER]);
  server.send(200, "text/plain", "OK");
}

void handleSpeed()
{
  if (!server.hasArg("joint") || !server.hasArg("dps"))
  {
    server.send(400, "text/plain", "Missing joint or dps");
    return;
  }

  int joint = server.arg("joint").toInt();
  if (joint < 0 || joint >= NUM_JOINTS)
  {
    server.send(400, "text/plain", "Invalid joint");
    return;
  }

  setJointSpeed(joint, server.arg("dps").toDouble());
  server.send(200, "text/plain", "OK");
}

void initWebServer() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > WIFI_TIMEOUT_MS) {
      Serial.println("\nWiFi connect timed out, web control disabled. Serial control still works.");
      return;
    }
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP Address: ");

  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/state", handleState);
  server.on("/pose", handlePose);
  server.on("/speed", handleSpeed);
  server.on("/home", handleHome);
  server.on("/stop", handleStop);

  server.onNotFound([](){
    server.send(404, "text/plain", "Page not found");
  });

  server.begin();
  webServerStarted = true;
}

void updateWebServerControl() {
  if (!webServerStarted) return;
  server.handleClient();
}