// ESP8266pro
// This software distributed under the terms of the MIT license
// (c) Skurydin Alexey, 2014

#ifndef _ESP8266pro_H_
#define _ESP8266pro_H_

class IESP8266proBaseReceiver;

#include "ESP8266pro_Parser.h"
#include "ESP8266proConnectionBase.h"

enum EncriptionMode
{
	eEM_OPEN			= 0,
	eEM_WEP             = 1,
	eEM_WPA_PSK         = 2,
	eEM_WPA2_PSK        = 3,
	eEM_WPA_WPA2_PSK    = 4
};

#define NULL_IP "0.0.0.0"

typedef void (*ConnectionDataCallback) (ESP8266proConnection* source, char* buffer, int length, boolean completed);

class ESP8266pro : public ESP8266pro_Parser
{
public:
	ESP8266pro(Stream &espStreamReference);
	ESP8266pro(Stream &espStreamReference, Stream &debugStreamReference);
	
	bool begin(OutputDebugMode debugOutMode = eODM_Data);
	
	bool stationConnect(const String& ssid, const String& password);
	bool stationDisconnect();
	String stationIP();
	String stationMAC();
	
	bool accessPointStart(const String& ssid, const String& password, EncriptionMode encription = eEM_WPA2_PSK, int wifiChannel = 5);
	bool accessPointStop();
	String accessPointIP();
	String accessPointMAC();
	
	// Internal methods!
	int addConnection(IESP8266proBaseReceiver* target);
	int getConnectionId(IESP8266proBaseReceiver* target);
	void removeConnection(IESP8266proBaseReceiver* target);
	boolean setServer(IESP8266proBaseReceiver* serverInstance);
	
protected:
	virtual void onDataReceive(int connectionId, char* buffer, int length, DataReceiveAction action);
	
private:
	IESP8266proBaseReceiver* connections[ESP_MAX_CONNECTIONS];
	IESP8266proBaseReceiver* server;
};

#endif