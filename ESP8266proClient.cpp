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

boolean ESP8266proClient::connectTcp(String addr, int port)
{
	int id = parent.addConnection(this);
	if (id == -1) return false;
	String request = (String)"AT+CIPSTART=" + id + ",\"TCP\",\"" + addr + "\"," + port;
	boolean ok = parent.execute(request);
	if (!ok)
	{
		delay(600);
		ok = parent.execute(request);
	}
	if (!ok)
		close();
	return ok;
}

boolean ESP8266proClient::connectUdp(String addr, int port)
{
	int id = parent.addConnection(this);
	if (id == -1) return false;
	boolean ok = parent.execute((String)"AT+CIPSTART=" + id + ",\"UDP\",\"" + addr + "\"," + port);
	if (!ok) close();
	return ok;
}

boolean ESP8266proClient::close()
{
	boolean linkClosed = false;
	linkClosed = ESP8266proConnection::close();
	if (linkClosed)
		parent.execute("", eCEM_NoLineBreak); // Read "CLOSED" response from ESP
	parent.removeConnection(this);
	return linkClosed;
}

boolean ESP8266proClient::waitResponse(int msTimeOut/* = 5000*/)
{
	// check actual
	int id = getId();
	if (id == -1) return false;
	
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

int ESP8266proClient::getId()
{
	return parent.getConnectionId(this);
}

void ESP8266proClient::onDataReceive(int connectionId, char* buffer, int length, DataReceiveAction action)
{
	if (action == eDRA_Begin) return; // Nothing here
	if (dataCallback != NULL)
		dataCallback(this, buffer, length, action == eDRA_End);
	//Serial.print(buffer);
}