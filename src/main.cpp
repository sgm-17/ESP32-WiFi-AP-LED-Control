#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

// Constants
const char* ssid = "ESP32AP";
const char* password = "12345678"; // Minimum 8 characters for WPA2 security

bool ledState = LOW;

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
WebServer server(80);

// Function to handle root path "/"
void handleRoot() {
  // HTML content for the webpage with a button to toggle the LED
  String html = "<html>\
  <head>\
    <title>ESP32 LED Control</title>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <style>\
      body { font-family: Arial; text-align: center; }\
      .button { padding: 15px 30px; font-size: 20px; cursor: pointer; }\
    </style>\
    <script>\
      function toggleLED() {\
        fetch('/toggle')\
          .then(response => response.text())\
          .then(state => {\
            document.getElementById('led-status').innerText = state;\
          })\
          .catch(error => console.error('Error:', error));\
      }\
    </script>\
  </head>\
  <body>\
    <h1>ESP32 LED Control</h1>\
    <p>LED is currently: <span id='led-status'>" + String(ledState ? "ON" : "OFF") + "</span></p>\
    <button class=\"button\" onclick=\"toggleLED()\">Toggle LED</button>\
  </body>\
  </html>";

  // Send the webpage to the client
  server.send(200, "text/html", html);
}

// Function to handle LED toggle
void handleToggle() {
  // Toggle the LED state
  if(ledState) {
    ledState = false;
    neopixelWrite(48, 0, 0, 0);
  } else {
    ledState = true;
    neopixelWrite(48, 3, 3, 1);
  }

  // Respond with the current LED state
  server.send(200, "text/plain", ledState ? "ON" : "OFF");
}

void handleNotFound() {
  server.sendHeader("Location", "http://alayshoo.film");
  server.send(302, "text/plain", "");
}


void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  neopixelWrite(48, 0, 0, 0);

  // Setup WiFi in AP mode
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);
  dnsServer.start(DNS_PORT, "*", apIP);

  // Define routes for handling root and toggle actions
  server.on("/", handleRoot);
  server.on("/toggle", HTTP_GET, handleToggle);
  server.onNotFound(handleNotFound);

  // Start the server
  server.begin();
  Serial.println("Server started");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
