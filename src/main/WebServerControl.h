#ifndef WEBSERVERCONTROL_H
#define WEBSERVERCONTROL_H

void handleRoot();
void handleState();

void handleSend();
void handleHome();
void handleStop();

void handlePose();
void handleSpeed();

void initWebServer();
void updateWebServerControl();


#endif