// ESP8266pro
// This software distributed under the terms of the MIT license
// (c) Skurydin Alexey, 2014

#ifndef _ESP8266proServer_H_
#define _ESP8266proServer_H_

#include "ESP8266pro.h"
#include "ESP8266proConnectionBase.h"

class ESP8266pro;
class ESP8266proServer;

class ESP8266proServer : public IESP8266proBaseReceiver
{
public:
	ESP8266proServer(ESP8266pro& esp, ConnectionDataCallback callback);
	
	boolean start(int port); // Only one working Server instance per time
	void stop();
	
	boolean processRequests(); // Non blocking operation, if no data received
	void closeAllConnections();
	
	// Internal method
	virtual void onDataReceive(int connectionId, char* buffer, int length, DataReceiveAction action);
	
private:
	ESP8266pro &parent;
	unsigned long lastCheck;
	int serverPort;
	ConnectionDataCallback dataCallback;
	ESP8266proServerConection* virtualConnection[ESP_MAX_CONNECTIONS];
	bool receiveCompleted[ESP_MAX_CONNECTIONS];
};

#endif