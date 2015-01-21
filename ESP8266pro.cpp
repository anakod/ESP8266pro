// ESP8266pro
// This software distributed under the terms of the MIT license
// (c) Skurydin Alexey, 2014

#include "ESP8266pro.h"

ESP8266pro::ESP8266pro(Stream &espStreamReference)
	: ESP8266pro_Parser(espStreamReference)
{
}

ESP8266pro::ESP8266pro(Stream &espStreamReference, Stream &debugStreamReference)
	: ESP8266pro_Parser(espStreamReference, debugStreamReference)
{
}

bool ESP8266pro::begin(OutputDebugMode debugOutMode/* = eODM_Data*/)
{
	#ifndef NODEBUG
	debugPrint("<ESP8266pro>");
	#endif
	
	if (!initializeParser(debugOutMode))
		restart(); // Trying to restart module
	if (!initializeParser(debugOutMode))
	{
		#ifndef NODEBUG
		debugPrint("<ERROR>");
		#endif
		return false;
	}
	
	// 1 = Station mode
	execute("AT+CWMODE=1");
	
	// 1 = multiple connection
	if (!execute("AT+CIPMUX=1"))
	{
		restart(); // Trying to restart module
		execute("AT+CIPMUX=1");
	}
}

bool ESP8266pro::stationConnect(const String& ssid, const String& password)
{
	return execute("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"", eCEM_LongTimeOut);
}

bool ESP8266pro::stationDisconnect()
{
	return execute("AT+CWQAP", eCEM_LongTimeOut);
}

String ESP8266pro::stationIP()
{
	if (!execute("AT+CIPSTA?")) return NULL_IP;
	return getLine(0);
}

String ESP8266pro::stationMAC()
{
	if (!execute("AT+CIPSTAMAC?")) return "";
	return getLine(0);
}

bool ESP8266pro::accessPointStart(const String& ssid, const String& password, EncriptionMode encription/* = eEM_WPA2_PSK*/, int wifiChannel/* = 5*/)
{
	if (!execute("AT+CWMODE=3")) return false; // 3 = AP + Station mode
	return execute("AT+CWSAP=\"" + ssid + "\",\"" + password + "\"," + wifiChannel + "," + encription, eCEM_LongTimeOut);
}

bool ESP8266pro::accessPointStop()
{
	execute("AT+CWMODE=1"); // 1 = Station mode
}

String ESP8266pro::accessPointIP()
{
	if (!execute("AT+CIPAP?")) return NULL_IP;
	return getLine(0);
}

String ESP8266pro::accessPointMAC()
{
	if (!execute("AT+CIPAPMAC?")) return NULL_IP;
	return getLine(0);
}

void ESP8266pro::onDataReceive(int connectionId, char* buffer, int length, DataReceiveAction action)
{
	if (connectionId >= 0 && connectionId < ESP_MAX_CONNECTIONS)
	{
		if (connections[connectionId] != NULL)
			connections[connectionId]->onDataReceive(connectionId, buffer, length, action);
		else
			server->onDataReceive(connectionId, buffer, length, action);
	}
}

int ESP8266pro::addConnection(IESP8266proBaseReceiver* target)
{
	for (int i = ESP_MAX_CONNECTIONS - 1; i >= 0; i--)
	{
		if (connections[i] == NULL)
		{
			connections[i] = target;
			return i;
		}
	}
	return -1;
}

int ESP8266pro::getConnectionId(IESP8266proBaseReceiver* target)
{
	for (int i = ESP_MAX_CONNECTIONS - 1; i >= 0; i--)
		if (connections[i] == target)
			return i;
	return -1;
}

void ESP8266pro::removeConnection(IESP8266proBaseReceiver* target)
{
	for (int i = ESP_MAX_CONNECTIONS - 1; i >= 0; i--)
		if (connections[i] == target)
			connections[i] = NULL;
}

boolean ESP8266pro::setServer(IESP8266proBaseReceiver* serverInstance)
{
	// Allowed only if server not started OR for reset server
	if (serverInstance == NULL || server == NULL)
	{
		server = serverInstance;
		return true;
	}
	else
		return false;
}