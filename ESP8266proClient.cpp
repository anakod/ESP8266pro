// ESP8266pro
// This software distributed under the terms of the MIT license
// (c) Skurydin Alexey, 2014

#include "ESP8266proClient.h"

ESP8266proClient::ESP8266proClient(ESP8266pro& esp) : ESP8266proConnection(esp)
{
}

ESP8266proClient::ESP8266proClient(ESP8266pro& esp, ConnectionDataCallback callback)
	: ESP8266proConnection(esp), dataCallback(callback)
{
}

bool ESP8266proClient::connectTcp(String addr, int port)
{
	uint8_t id = parent.addConnection(this);
	if (id == ESP_INVALID_CONNECTION) return false;
	String request = (String)"AT+CIPSTART=" + id + ",\"TCP\",\"" + addr + "\"," + port;
	bool ok = parent.execute(request);
	if (!ok)
	{
		delay(600);
		ok = parent.execute(request);
	}
	if (!ok)
		close();
	return ok;
}

bool ESP8266proClient::connectUdp(String addr, int port)
{
	uint8_t id = parent.addConnection(this);
	if (id == ESP_INVALID_CONNECTION) return false;
	bool ok = parent.execute((String)"AT+CIPSTART=" + id + ",\"UDP\",\"" + addr + "\"," + port);
	if (!ok) close();
	return ok;
}

bool ESP8266proClient::close()
{
	bool linkClosed = false;
	linkClosed = ESP8266proConnection::close();
	if (linkClosed)
		parent.execute("", eCEM_NoLineBreak); // Read "CLOSED" response from ESP
	parent.removeConnection(this);
	return linkClosed;
}

bool ESP8266proClient::waitResponse(int msTimeOut/* = 5000*/)
{
	// check actual
	uint8_t id = getId();
	if (id == ESP_INVALID_CONNECTION) return false;
	
	unsigned long startMillis = millis();
	do
	{
		if (parent.connectionDataReceive(true))
			return true;
		else
			delay(1);
	} while(millis() - startMillis < msTimeOut);
	return false;
}

uint8_t ESP8266proClient::getId()
{
	return parent.getConnectionId(this);
}

void ESP8266proClient::onDataReceive(uint8_t connectionId, char* buffer, int length, DataReceiveAction action)
{
	if (action == eDRA_Begin) return; // Nothing here
	if (dataCallback != NULL)
		dataCallback(this, buffer, length, action == eDRA_End);
	//Serial.print(buffer);
}