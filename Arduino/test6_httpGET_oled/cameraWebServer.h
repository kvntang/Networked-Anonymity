//camerWebServer.h file

#ifndef cameraWebServer_h
#define cameraWebServer_h

//Wifi + Cam
#include <WebServer.h>
#include <WiFi.h>
#include "esp32cam.h"
//Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

extern WebServer server;
extern Adafruit_SSD1306 display;
extern int globalNumber;

//basic functions
void serveJpg();
void handleJpgLo();
void handleJpgHi();
void handleSetNumber();

//setup
//void updateDisplay();
void setupCameraServer(const String& ssid, const String& password, String& mac_address, IPAddress& ip_address);
void setupDisplay(String text);
void displayStuff(String mac_address, IPAddress ip_address, int num);

//looping
void loopCameraWebServer();

#endif
