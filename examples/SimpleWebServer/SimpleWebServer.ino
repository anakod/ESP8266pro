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
// This example discover how to create simple Web server to receive and process HTTP requests
//

#include <SoftwareSerial.h>
#include <ESP8266pro.h>
#include <ESP8266proServer.h>

SoftwareSerial espSerial(9, 10); // RX, TX

ESP8266pro wifi(espSerial, Serial);
ESP8266proServer server(wifi, onClientRequest);
String requestPaths[ESP_MAX_CONNECTIONS];

const char* ssid = "YouWiFi";
const char* password = "youpasswordhere";

const int ledPin = 13;

void setup()
{
  pinMode(ledPin, OUTPUT);
  
  espSerial.begin(9600);
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  // Initialize ESP
  wifi.begin();
  
  // Connect to WiFi network
  // In future ESP should automatically reconnect
  // to this network, if requried
  do
  {
    if (!wifi.stationConnect(ssid, password))
      delay(3000);
  } while (wifi.stationIP() == NULL_IP);
  
// Start server on port 80
  server.start(80);
  
  // Get AP ip address
  String ip = wifi.stationIP();
  
  Serial.println();
  Serial.println("=================================");
  Serial.println("Server started: http://" + ip);
  Serial.println("=================================");
}

void loop()
{
  // Process incoming requests
  server.processRequests();
}

// Optional method to process remote response
void printResponse(ESP8266proConnection* connection,
          char* buffer, int length, boolean completed)
{
  Serial.print(buffer);
}

void onClientRequest(ESP8266proConnection* connection,
          char* buffer, int length, boolean completed)
{
  if (strncmp(buffer, "GET ", 4) == 0)
  {
    // We found GET HTTP request
    char* p = strstr(buffer + 4, " ");
    *p = '\0'; // erase string after path
    requestPaths[connection->getId()] = (String)((char*)buffer + 4);
  }
  
  if (completed)
  {
    String path = requestPaths[connection->getId()];
    if (path == "/")
    {
      connection->send(F("HTTP/1.0 200 OK\r\n\r\n"
        "<p><a href='/led1'>LED ON</a>"
        "<p><a href='/led0'>LED OFF</a>"
        "<p><a href='/hello'>HELLO?</a>"
      ));
    }
    else if (path.startsWith("/led"))
    {
      if (path.substring(4) == "1")
        digitalWrite(ledPin, HIGH);
      else
        digitalWrite(ledPin, LOW);
      // Make redirect to main page
      // I'm not recommend fast redirect
      connection->send(F("HTTP/1.0 200 OK\r\n\r\n"
        "<html>"
        "<meta http-equiv='refresh' content='1; url=/' />"
        "SUCCESS"
        "</html>"
      ));
    }
    else if (path.startsWith("/hello"))
      connection->send(F("Hi from ESP8266pro, my friend!"));
    else
    {
      connection->send(F("HTTP/1.0 404 Not Found\r\n\r\n"));
      connection->send(F("<h1>4 0 4</h1>"));
      connection->send(F("Yes, this is true. :("));
    }
    connection->close();
  }
}
