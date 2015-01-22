// ESP8266pro
// This software distributed under the terms of the MIT license
// (c) Skurydin Alexey, 2014

#ifndef _ESP8266proConnectionBase_H_
#define _ESP8266proConnectionBase_H_

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define ESP_INVALID_CONNECTION			((uint8_t)255)

class ESP8266pro;
class ESP8266Connection;
class ESP8266proServer;

class IESP8266proBaseReceiver
{
public:
	virtual void onDataReceive(uint8_t connectionId, char* buffer, int length, DataReceiveAction action) = 0;
};

class ESP8266proConnection
{
public:
	virtual uint8_t getId() = 0;
	
	ESP8266proConnection(ESP8266pro& esp);
	virtual bool send(String data);
	virtual bool send(const __FlashStringHelper* data);
	virtual bool close();

protected:
	bool internalSend(const __FlashStringHelper* dataP, const char* dataR);
	
protected:
	ESP8266pro &parent;
};

class ESP8266proServerConection : public ESP8266proConnection
{
public:
	ESP8266proServerConection(ESP8266pro& esp, uint8_t id);
	
	virtual bool send(String data);
	virtual bool send(const __FlashStringHelper* data);
	virtual bool close();
	virtual uint8_t getId();
	
	// Internal methods
	void incrimentUses();
	void decrementUses();
	//bool isUsed();
	void dispose();
	
	friend class ESP8266proServer;

private:
	uint8_t cid;
	uint8_t uses;
};

#endif