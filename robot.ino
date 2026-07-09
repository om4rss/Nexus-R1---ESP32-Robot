#include "BluetoothSerial.h"
#include <WiFi.h>
#include <WebServer.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled!
#endif

// Network Credentials
const char* ssid = "Nexus R1 Car";
const char* password = "admin1234";

BluetoothSerial SerialBT;
WebServer server(80);

// Pin Allocation Matrix
const int pinENA = 25;
const int pinIN1 = 15;
const int pinIN2 = 14;
const int pinIN3 = 26;
const int pinIN4 = 27;
const int pinENB = 4;

void setMotors(bool ena, bool in1, bool in2, bool in3, bool in4, bool enb) {
  digitalWrite(pinENA, ena);
  digitalWrite(pinIN1, in1);
  digitalWrite(pinIN2, in2);
  digitalWrite(pinIN3, in3);
  digitalWrite(pinIN4, in4);
  digitalWrite(pinENB, enb);
}

// Wi-Fi HTTP Request Route Handlers
void handleForward()  { setMotors(HIGH, LOW, HIGH, HIGH, LOW, HIGH); server.send(200, "text/plain", "OK_FORWARD"); }
void handleBackward() { setMotors(HIGH, HIGH, LOW, LOW, HIGH, HIGH); server.send(200, "text/plain", "OK_BACKWARD"); }
void handleLeft()     { setMotors(HIGH, HIGH, LOW, HIGH, LOW, HIGH); server.send(200, "text/plain", "OK_LEFT"); }
void handleRight()    { setMotors(HIGH, LOW, HIGH, LOW, HIGH, HIGH); server.send(200, "text/plain", "OK_RIGHT"); }
void handleStop()     { setMotors(LOW, LOW, LOW, LOW, LOW, LOW);    server.send(200, "text/plain", "OK_STOP"); }

void setup() {
  Serial.begin(115200);
  
  pinMode(pinENA, OUTPUT);
  pinMode(pinIN1, OUTPUT);
  pinMode(pinIN2, OUTPUT);
  pinMode(pinIN3, OUTPUT);
  pinMode(pinIN4, OUTPUT); 
  pinMode(pinENB, OUTPUT);
  
  // Safe state on boot
  setMotors(LOW, LOW, LOW, LOW, LOW, LOW);

  // Initialize Bluetooth Classic Stack
  SerialBT.begin("Nexus ESP32 Car");
  Serial.println("Bluetooth Core Online: Ready to pair with mobile app.");

  // Initialize Local Wi-Fi Core
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi Network");
  
  // Fail-safe connection gate (max 10 seconds wait)
  int connectionCounter = 0;
  while (WiFi.status() != WL_CONNECTED && connectionCounter < 20) {
    delay(500);
    Serial.print(".");
    connectionCounter++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi Connection Established.");
    Serial.print("Target IP Address: ");
    Serial.println(WiFi.localIP()); 
  } else {
    Serial.println("\nWi-Fi Network Timeout. Continuing in Bluetooth-Only Mode.");
  }

  // Bind HTTP endpoints directly to execution vectors
  server.on("/F", handleForward);
  server.on("/B", handleBackward);
  server.on("/L", handleLeft);
  server.on("/R", handleRight);
  server.on("/S", handleStop);
  
  server.begin();
}

void loop() {
  // Channel 1: Watch for incoming Bluetooth raw characters (Mobile App Interface)
  if (SerialBT.available()) {
    char command = SerialBT.read();
    
    switch (command) {
      case 'F': setMotors(HIGH, LOW, HIGH, HIGH, LOW, HIGH); break;
      case 'B': setMotors(HIGH, HIGH, LOW, LOW, HIGH, HIGH); break;
      case 'L': setMotors(HIGH, HIGH, LOW, HIGH, LOW, HIGH); break;
      case 'R': setMotors(HIGH, LOW, HIGH, LOW, HIGH, HIGH); break;
      case 'S': setMotors(LOW, LOW, LOW, LOW, LOW, LOW);    break;
      default:  break;
    }
  }

  // Watch for incoming HTTP REST requests (Python AI Interface)
  server.handleClient();
}