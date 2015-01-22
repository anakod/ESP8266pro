// ESP8266pro
// This software distributed under the terms of the MIT license
// (c) Skurydin Alexey, 2014

#ifndef _ESP8266proClient_H_
#define _ESP8266proClient_H_

#include "ESP8266pro.h"
#include "ESP8266proConnectionBase.h"

class ESP8266pro;

class ESP8266proClient : public ESP8266proConnection, public IESP8266proBaseReceiver
{
public:
	ESP8266proClient(ESP8266pro& esp);
	ESP8266proClient(ESP8266pro& esp, ConnectionDataCallback callback);
	bool connectTcp(String addr, int port);
	bool connectUdp(String addr, int port);
	
	virtual bool close();
	
	bool waitResponse(int msTimeOut = 5000);
	
	// Internal methods
	virtual uint8_t getId();
	virtual void onDataReceive(uint8_t connectionId, char* buffer, int length, DataReceiveAction action);
	
private:
	ConnectionDataCallback dataCallback;
};

#endif