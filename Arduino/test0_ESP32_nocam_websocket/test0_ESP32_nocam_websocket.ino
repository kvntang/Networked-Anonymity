#include <WebServer.h>
#include <WiFi.h>
#include <esp32cam.h>
#include <ArduinoWebsockets.h>

const char* ssid = "xfinitywifi_HUH_Res";
const char* password = "huhwifi9434";

using namespace websockets;

const int ledPin = 5; // Replace with your LED pin

WebsocketsServer wsServer;
WebsocketsClient client; // Define the client globally

void setup() {
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT); // Initialize LED pin as an output

    // Wi-Fi setup
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

    Serial.print("WebSocket Server: ws://");
    Serial.print(WiFi.localIP());
    Serial.println(":81");

    // Start WebSocket server
    wsServer.listen(81); // Listen on port 81
}

void loop() {
    // Accept a new client if there isn't one already
    if (!client.available()) {
        client = wsServer.accept();
    }

    // Check if the client sent a message
    if (client.available()) {
        WebsocketsMessage msg = client.readBlocking();
        Serial.print("Got Message: ");
        Serial.println(msg.data());
        client.send("Echo: " + msg.data());
    }

    delay(500); // Optional: Add a small delay to reduce CPU usage
}
