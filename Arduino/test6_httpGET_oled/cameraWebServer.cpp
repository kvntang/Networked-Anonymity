//cameraWebServer.cpp file

#include "cameraWebServer.h"

//this is all the extra code

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define I2C_SDA 14
#define I2C_SCL 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); //display

using namespace esp32cam;
WebServer server; //server

static auto loRes = esp32cam::Resolution::find(320, 240);
static auto hiRes = esp32cam::Resolution::find(800, 600);
extern int globalNumber;


////NEW FUNCTIONS ///////////////////////////////////////////////////////////
void setupDisplay(String text){
  Wire.begin(I2C_SDA, I2C_SCL);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(text);;
  display.display();
}


void setupCameraServer(const String& ssid, const String& password, String& mac_address, IPAddress& ip_address) {
  Serial.println();
  
  // Camera Setup
  {
    using namespace esp32cam;
    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(hiRes); // Ensure 'hiRes' is defined somewhere as a valid resolution
    cfg.setBufferCount(2);
    cfg.setJpeg(80);
 
    bool ok = Camera.begin(cfg);
    Serial.println(ok ? "CAMERA OK" : "CAMERA FAIL");
  }

  // WiFi setup
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  mac_address = WiFi.macAddress(); // Capture MAC address after connection
  ip_address = WiFi.localIP();     // Capture IP address after connection
  
  // HTTP server routes
  server.on("/cam-lo.jpg", handleJpgLo);
  server.on("/cam-hi.jpg", handleJpgHi);
  server.on("/setNumber", handleSetNumber);
  server.begin();

  // No need to return values explicitly; they are returned via reference parameters
}

void displayStuff(String mac_address, IPAddress ip_address, int num){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
    display.print("MAC: "); display.println(mac_address);
  display.print("IP: "); display.println(ip_address);
  display.print("Receiving: "); display.println(num);
  display.display();
}

void loopCameraWebServer(){
  server.handleClient();
  delay(10);
}


void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);

  // Display Title
  display.println(F("ESP32 CAM Module"));

  // Display WiFi status
  if (WiFi.status() == WL_CONNECTED) {
    display.println("WiFi: Connected");
  } else {
    display.println("WiFi: Not Connected");
  }

  // Display MAC and IP Address
  display.println("MAC: " + WiFi.macAddress());
  if (WiFi.status() == WL_CONNECTED) {
    display.println("IP: " + WiFi.localIP().toString());
  } else {
    display.println("IP: Not Available");
  }

  // Calculate hours, minutes, and seconds from millis
  unsigned long currentMillis = millis();
  unsigned long seconds = currentMillis / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;

  // Correct for days, if necessary
  seconds %= 60; // Seconds remaining after converting to minutes
  minutes %= 60; // Minutes remaining after converting to hours

  // Display the current time in HH:MM:SS format
  display.setCursor(0, 56); // Adjust position as needed
  display.print("Time: ");
  if (hours < 10) display.print('0');
  display.print(hours);
  display.print(':');
  if (minutes < 10) display.print('0');
  display.print(minutes);
  display.print(':');
  if (seconds < 10) display.print('0');
  display.println(seconds);

  display.display();
}


//BASIC FUNCTIONS///////////////////////////////////////////////////////////////////////////////
void serveJpg()
{
  auto frame = esp32cam::capture();
  if (frame == nullptr) {
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");
    return;
  }
  
//  Serial.printf("CAPTURE OK %dx%d %db\n", frame->getWidth(), frame->getHeight(),
//                static_cast<int>(frame->size()));
 
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
