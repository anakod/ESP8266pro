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
// This example discover how you can send HTTP requests/download remote pages
//

#include <SoftwareSerial.h>
#include <ESP8266pro.h>
#include <ESP8266proClient.h>

SoftwareSerial espSerial(9, 10); // RX, TX

ESP8266pro wifi(espSerial, Serial); // Serial, DebugSerial

const char* ssid = "YouWiFi";
const char* password = "youpasswordhere";

void setup()
{
  espSerial.begin(9600);
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("Starting");
  
  // Initialize ESP
  wifi.begin(eODM_None); // Disable all debug messages
  
  // Connect to WiFi network
  // In future ESP should automatically reconnect
  // to this network, if requried
  do
  {
    if (!wifi.stationConnect(ssid, password))
      delay(3000);
  } while (wifi.stationIP() == NULL_IP);
  
  Serial.print("ESP IP: ");
  Serial.println(wifi.stationIP());
}

void loop()
{
  // Send request
  ESP8266proClient con(wifi, printResponse);
  con.connectTcp("37.59.251.26", 80);
  con.send("GET / HTTP/1.0\r\nHost: simple.anakod.ru\r\n\r\n");
  con.waitResponse();
  con.close();
  
  delay(5000);
}

// Optional method to process remote response
void printResponse(ESP8266proConnection* connection,
          char* buffer, int length, boolean completed)
{
  Serial.print(buffer);
}