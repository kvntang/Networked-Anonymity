//use ESP32 Wrover Module
//ino file

#include "cameraWebServer.h"

const char* ssid = "xfinitywifi_HUH_Res";
const char* password = "huhwifi9434";
String mac_address;
IPAddress ip_address;
int globalNumber = 0;

//////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  setupDisplay("system initiating");
  setupCameraServer(ssid, password, mac_address, ip_address);//returns mac and ip
  displayStuff(mac_address, ip_address, globalNumber);//display mac and ip
}

void loop() {
  loopCameraWebServer();
  displayStuff(mac_address, ip_address, globalNumber);//display mac and ip
}
