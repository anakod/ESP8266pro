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

class ESP8266pro;
class ESP8266Connection;

class IESP8266proBaseReceiver
{
public:
	virtual void onDataReceive(int connectionId, char* buffer, int length, DataReceiveAction action) = 0;
};

class ESP8266proConnection
{
public:
	virtual int getId() = 0;
	
	ESP8266proConnection(ESP8266pro& esp);
	virtual boolean send(String data);
	virtual boolean send(const __FlashStringHelper* data);
	virtual boolean close();

protected:
	boolean internalSend(const __FlashStringHelper* dataP, const char* dataR);
	
protected:
	ESP8266pro &parent;
};

class ESP8266proServerConection : public ESP8266proConnection
{
public:
	ESP8266proServerConection(ESP8266pro& esp, int id);
	
	virtual boolean send(String data);
	virtual boolean send(const __FlashStringHelper* data);
	virtual boolean close();
	virtual int getId();
	
	// Internal methods
	void incrimentUses();
	void decrementUses();
	boolean isUsed();
	void dispose();

private:
	int cid;
	unsigned int uses;
};

#endif