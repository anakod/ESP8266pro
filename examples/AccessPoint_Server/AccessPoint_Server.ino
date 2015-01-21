// This software distributed under the terms of the MIT license
// (c) Skurydin Alexey, 2014
//
// Before beginning of work:
// 1. Update ESP8266 firmware to lastest version before using this library (at least AT v0.20)
// 2. Verify, what WiFi module UART works on 9600 speed (also you can choose different speed,
//    but don't forget about software serial limitations)
// 3. Increase _SS_MAX_RX_BUFF in Arduino SoftwareSerial.h to prevent data loss (because by
//    default, Arduino has very small buffers for software and hardware serials). I recommend
//    size of 256 bytes for that.
//
// This example discover how you can create Software Access Point and start small Web server
//

#include <SoftwareSerial.h>
#include <ESP8266pro.h>
#include <ESP8266proServer.h>

SoftwareSerial espSerial(9, 10); // RX, TX

ESP8266pro wifi(espSerial, Serial); // ESP, DBG
ESP8266proServer server(wifi, onClientRequest);

const char* ssid = "ESP_AP";
const char* password = "123456789";

void setup()
{
  espSerial.begin(9600);
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  // Initialize ESP
  wifi.begin();
  
  // Create access point
  wifi.accessPointStart(ssid, password);
  
  // Start server on port 80
  server.start(80);
  
  // Get AP IP address
  String ip = wifi.accessPointIP();
  
  Serial.println();
  Serial.println("=================================");
  Serial.println("1. Connect to wifi network " + String(ssid));
  Serial.println("2. Open: http://" + ip);
  Serial.println("=================================");
}

void loop()
{
  // Process incoming requests
  server.processRequests();
  
  // Serial commands. Just for test.
  char cmd = Serial.read();
  if (cmd == 's') // Stop
  {
    wifi.accessPointStop();
  }
  else if (cmd == 'o') // Open AP
  {
    wifi.accessPointStop();
    wifi.accessPointStart("ESP_OPEN", "", eEM_OPEN);
  } 
}

void onClientRequest(ESP8266proConnection* connection,
          char* buffer, int length, boolean completed)
{
  if (completed)
  {
    Serial.println(F("onClientRequest"));
    
    // Send response from PROGMEM to miminize SRAM usage
    connection->send(F("HTTP/1.0 200 OK"));
    connection->send(F("Server: ESP8266pro\r\n"));
    connection->send(F("Content-Type: text/html\r\n\r\n"));
    connection->send(F("<p style='color: #444'>Hello from <b>ESP8266pro</b></p>"));
    connection->close();
  }
}
