# ESP8266pro
ESP8266 Arduino library - easy to use and powerfull WiFi Module adapter

## Some examples
#### Send HTTP request
```C
ESP8266pro wifi(espSerial, Serial); // Serial, DebugSerial

// Initialize module
wifi.begin();

// Connect to WiFi network
wifi.stationConnect(ssid, password);
Serial.println(wifi.stationIP());

// Send request
ESP8266proClient con(wifi);
con.connectTcp("37.59.251.26", 80);
con.send("GET / HTTP/1.0\r\nHost: simple.anakod.ru\r\n\r\n");
con.waitResponse();
con.close(); // Completed!
```

#### Software Access point
```C
// Create access point
wifi.accessPointStart("ESP_AP", "123456789");

// Get AP IP address
String ip = wifi.accessPointIP();
Serial.println(ip);

delay(10000);

// Quit from Access Point mode
wifi.accessPointStop();
```

#### Simple HTTP web server
```C
ESP8266proServer server(wifi, onClientRequest);

// Start server on port 80
server.start(80);
```

```C
void loop()
{
  // Process incoming requests
  server.processRequests();
}

void onClientRequest(ESP8266proConnection* connection,
          char* buffer, int length, boolean completed)
{
  if (completed)
  {
    connection->send("HTTP/1.0 200 OK\r\n\r\n");
    connection->send("Hi from ESP8266pro!");
    connection->close();
  }
}
```

## Connection
During development I have been used SoftwareSerial library, and connect ESP8266 to digital pins 9 and 10 on Arduino board.
But you can use HardwareSerial instead this, or additional hardware serial port (if you have). No any limitation here.
```C
ESP8266pro wifi(/*ESP8266_SERIAL*/, /*OPTIONAL_DEBUG_SERIAL*/);
```

## Before beginning of work

1. Update ESP8266 firmware to lastest version before using this library (at least AT v0.20)
2. Verify, what WiFi module UART works on 9600 speed (you can choose different speed, if you want)
3. Increase _SS_MAX_RX_BUFF in Arduino SoftwareSerial.h to prevent data loss (because by default, Arduino has very small buffers for software and hardware serials). I recommend size of 256 bytes for that:

```C
//In SoftwareSerial.h:
#define _SS_MAX_RX_BUFF 256
```

If you using HardwareSerial instead of software, you should update SERIAL_BUFFER_SIZE constant in HardwareSerial.cpp.
