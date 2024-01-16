//use ESP32 Wrover Module

#include <WebServer.h>
#include <WiFi.h>
#include <esp32cam.h>
#include <ArduinoWebsockets.h>

const char* ssid = "xfinitywifi_HUH_Res";
const char* password = "huhwifi9434";

using namespace websockets;

//Board stuff
const int ledPin = 5;  // Replace with your LED pin

//DUAL PORTS
WebServer server(80); //1. camera streaming
WebsocketsServer wsServer;
WebsocketsClient client; // Define the client globally

static auto loRes = esp32cam::Resolution::find(320, 240);
static auto hiRes = esp32cam::Resolution::find(800, 600);

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
  Serial.println(":80");
  
  Serial.print("WebSocket Server: ws://");
  Serial.print(WiFi.localIP());
  Serial.println(":81");

  // HTTP server routes
  server.on("/cam-lo.jpg", handleJpgLo);
  server.on("/cam-hi.jpg", handleJpgHi);
  server.begin();

  // Start WebSocket server
  wsServer.listen(81);   // Listen on port 81
}

void loop() {
  //1. HTTP server
  server.handleClient();

  //2. WebSocket server
  if (!client.available()) {
        client = wsServer.accept();
    }
  if (client.available()) {
      WebsocketsMessage msg = client.readBlocking();
      Serial.print("Got Message: ");
      Serial.println(msg.data());
      client.send("Echo: " + msg.data());
  }

  delay(500); // Optional: Add a small delay to reduce CPU usage

}
