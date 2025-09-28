#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Wi-Fi settings
const char* ssid = "Wi-Fi Name/ssid";
const char* password = "Wi-Fi Password";

// OLED settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// WebServer
ESP8266WebServer server(80);

// Current text to display
String displayText = "Hello!";

// Webpage HTML
const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>ESP8266 OLED Control</title>
<style>
body { font-family: Arial; text-align: center; background: #222; color: white; }
input { width: 200px; }
</style>
</head>
<body>
<h1>ESP8266 OLED Control</h1>
<p>Enter text to display on OLED:</p>
<input type="text" id="textInput" value="Hello!">
<button onclick="sendText()">Update</button>
<script>
function sendText() {
    let txt = document.getElementById('textInput').value;
    fetch("/update?text=" + encodeURIComponent(txt))
    .then(response => response.text())
    .then(alert);
}
</script>
</body>
</html>
)rawliteral";

// Serve main webpage
void handleRoot() {
  server.send_P(200, "text/html", webpage);
}

// Handle text update
void handleUpdate() {
  if (server.hasArg("text")) {
    displayText = server.arg("text");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println(displayText);
    display.display();
    server.send(200, "text/plain", "Text updated!");
  } else {
    server.send(400, "text/plain", "No text provided!");
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(14, 12); // D5=SDA, D6=SCL

  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found");
    while(1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Connecting WiFi...");
  display.display();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  String ipStr = WiFi.localIP().toString();
  Serial.println("IP address: " + ipStr);

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Connected!");
  display.println("IP:");
  display.println(ipStr);
  display.display();

  // Start server
  server.on("/", handleRoot);
  server.on("/update", handleUpdate);
  server.begin();
}

void loop() {
  server.handleClient();
}
