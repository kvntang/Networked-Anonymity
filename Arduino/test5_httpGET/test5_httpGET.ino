//use ESP32 Wrover Module

#include <WebServer.h>
#include <WiFi.h>
#include <esp32cam.h>

const char* ssid = "xfinitywifi_HUH_Res";
const char* password = "huhwifi9434";

using namespace esp32cam;



WebServer server;

static auto loRes = esp32cam::Resolution::find(320, 240);
static auto hiRes = esp32cam::Resolution::find(800, 600);

//Outputs
const int ledPin = 5;  // Replace with your LED pin
int globalNumber = 0; // Global variable to store the number

//FUNCTIONS///////////////////////////////////////////////////////////////////////////////
void serveJpg()
{
  auto frame = esp32cam::capture();
  if (frame == nullptr) {
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");
    return;
  }
  
  Serial.printf("CAPTURE OK %dx%d %db\n", frame->getWidth(), frame->getHeight(),
                static_cast<int>(frame->size()));
 
  server.setContentLength(frame->size());
  server.send(200, "image/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);
}
 
void handleJpgLo()
{
  if (!esp32cam::Camera.changeResolution(loRes)) {
    Serial.println("SET-LO-RES FAIL");
  }
  serveJpg();
}
 
void handleJpgHi()
{
  if (!esp32cam::Camera.changeResolution(hiRes)) {
    Serial.println("SET-HI-RES FAIL");
  }
  serveJpg();
}

void handleSetNumber() {
  if (server.hasArg("value")) {
    globalNumber = server.arg("value").toInt();
    Serial.println("Received number: " + String(globalNumber));
    server.send(200, "text/plain", "Number set to: " + String(globalNumber));
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

//////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);  // Initialize LED pin as an output
  
  //Camera Setup
  Serial.println();
  {
    using namespace esp32cam;
    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(hiRes);
    cfg.setBufferCount(2);
    cfg.setJpeg(80);
 
    bool ok = Camera.begin(cfg);
    Serial.println(ok ? "CAMERA OK" : "CAMERA FAIL");
  }

  //Wifi setup
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);

  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // Print IP address for HTTP and WebSocket server
  Serial.print("Camera Stream: http://");
  Serial.print(WiFi.localIP());

  

  // HTTP server routes
  server.on("/cam-lo.jpg", handleJpgLo);
  server.on("/cam-hi.jpg", handleJpgHi);
  server.on("/setNumber", handleSetNumber);
  server.begin();

}

void loop() {
  server.handleClient();

 
  delay(100);
}
