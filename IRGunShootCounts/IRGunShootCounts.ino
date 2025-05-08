#include <WiFi.h>
#include <IRremote.h>
#include <WebServer.h>

#define IR_PIN 14
#define MAX_HITS 8

// WiFi credentials
const char* ssid = "Xtracodenary";
const char* password = "Xtra#1234";

// Hit counter
int hitCount = 0;
bool isOut = false;

// Web server on port 80
WebServer server(80);

void setup() {
  Serial.begin(115200);

  // Start IR Receiver
  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("IR Receiver started...");

  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Handle HTTP requests
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  server.handleClient();

  if (IrReceiver.decode()) {
    Serial.print("IR Signal Detected: ");
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);

    if (!isOut) {
      hitCount++;
      Serial.print("Hit Count: ");
      Serial.println(hitCount);

      if (hitCount >= MAX_HITS) {
        isOut = true;
        Serial.println("Player is OUT!");
      }
    }

    IrReceiver.resume();  // Ready for next
  }
}

// Handle web page request
void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='utf-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<meta http-equiv='refresh' content='2'>";  // Auto-refresh every 2 seconds
  html += "<title>Laser Tag - Hit Counter</title>";
  html += "<style>body{font-family:sans-serif;text-align:center;padding:40px;}h1{color:#333;}p{font-size:24px;}</style>";
  html += "</head><body>";
  html += "<h1>Laser Tag Player</h1>";
  html += "<p><strong>Hits Received:</strong> " + String(hitCount) + "</p>";

  if (isOut)
    html += "<p style='color:red; font-weight:bold;'>⚠ Player is OUT!</p>";
  else
    html += "<p style='color:green;'>🟢 Still in the game!</p>";

  html += "</body></html>";

  server.send(200, "text/html", html);
}
