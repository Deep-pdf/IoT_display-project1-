#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Wi-Fi Credentials
const char* ssid = "Deep";
const char* password = "43211234";

// Start WebServer on port 80
WebServer server(80);

// HTML Page stored in ESP32 memory
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 OLED Control</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
    input[type=text] { padding: 10px; width: 80%; max-width: 300px; margin-bottom: 20px; font-size: 16px;}
    input[type=submit] { padding: 10px 20px; font-size: 16px; background-color: #4CAF50; color: white; border: none; border-radius: 5px; cursor: pointer; }
  </style>
</head>
<body>
  <h2>Send Message to OLED</h2>
  <form action="/send" method="POST">
    <input type="text" name="message" placeholder="Type your message here..." required>
    <br>
    <input type="submit" value="Send to ESP32">
  </form>
</body>
</html>
)rawliteral";

// Function to serve the main HTML page
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

// Function to handle the form submission
void handleSend() {
  if (server.hasArg("message")) {
    String msg = server.arg("message");
    
    // Clear display and print new message
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2); // Make the text larger
    display.println(msg);
    display.display();
    
    // Redirect back to the main page after sending
    server.sendHeader("Location", "/");
    server.send(303);
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize OLED (0x3C is the standard I2C address for these screens)
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Connecting to WiFi...");
  display.display();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Display IP Address on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi Connected!");
  display.println("Go to IP on phone:");
  display.println();
  display.setTextSize(1);
  display.println(WiFi.localIP());
  display.display();

  // Print IP to Serial Monitor as backup
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Setup Web Server Routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/send", HTTP_POST, handleSend);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Listen for incoming client requests
  server.handleClient();
}