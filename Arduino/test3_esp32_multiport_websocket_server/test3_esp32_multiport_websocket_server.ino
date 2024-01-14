#include <WebServer.h>
#include <WiFi.h>
#include <esp32cam.h>
#include <ArduinoWebsockets.h>

const char* ssid = "xfinitywifi_HUH_Res";
const char* password = "huhwifi9434";

//Board stuff
const int ledPin = 5;  // Replace with your LED pin

//DUAL PORTS
WebServer server(80); //1. camera streaming
WebSocketsServer wsServer(81); // 2. WebSocket server

static auto loRes = esp32cam::Resolution::find(320, 240);
static auto hiRes = esp32cam::Resolution::find(800, 600)

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

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT || type == WStype_BIN) {
      // Assuming payload is either "0" or "1"
      if (length == 1) {
          if (payload[0] == '0') {
              digitalWrite(ledPin, LOW);  // Turn LED off
              Serial.println("LED turned off");
              wsServer.sendTXT(num, "LED turned off");
          } else if (payload[0] == '1') {
              digitalWrite(ledPin, HIGH); // Turn LED on
              Serial.println("LED turned on");
              wsServer.sendTXT(num, "LED turned on");
          }
      }
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
  Serial.println(":80");
  
  Serial.print("WebSocket Server: ws://");
  Serial.print(WiFi.localIP());
  Serial.println(":81");

  // HTTP server routes
  server.on("/cam-lo.jpg", handleJpgLo);
  server.on("/cam-hi.jpg", handleJpgHi);
  server.begin();

  // WebSocket server setup
  wsServer.begin();
  wsServer.onEvent(onWebSocketEvent);
}

void loop() {
  server.handleClient();
  wsServer.loop();
}
