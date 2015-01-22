// ESP8266pro
// This software distributed under the terms of the MIT license
// (c) Skurydin Alexey, 2014

#include "ESP8266pro.h"
#include "ESP8266proConnectionBase.h"

ESP8266proConnection::ESP8266proConnection(ESP8266pro& esp) : parent(esp)
{
}

bool ESP8266proConnection::send(String data)
{
	return internalSend(NULL, data.c_str());
}

bool ESP8266proConnection::send(const __FlashStringHelper* data)
{
	return internalSend(data, NULL);
}

bool ESP8266proConnection::internalSend(const __FlashStringHelper* dataP, const char* dataR)
{
	bool ok;
	int len = 0;
	if (dataP != NULL)
		len += strlen_P(reinterpret_cast<PGM_P>(dataP));
	if (dataR != NULL)
		len += strlen(dataR);

	uint8_t id = getId();
	if (id == ESP_INVALID_CONNECTION) return false;
	parent.execute((String)"AT+CIPSEND=" + id + "," + len, eCEM_NoResponse);	
	
	// Write request data
	if (dataP != NULL)
		parent.writeString(dataP);
	
	ok = parent.execute(dataR!= NULL ? dataR : "", eCEM_NoLineBreak);
	if (!ok) return false;
	
	// Wait "SEND OK" response
	for (uint8_t i = 0; i < 3; i++)
	{
		if (getId() == ESP_INVALID_CONNECTION) return false; // Already closed..
		if (parent.getState() == ePS_Completed) return true;
		if (!parent.execute("", eCEM_NoLineBreak)) return false;
	}
	
	return false;
}

bool ESP8266proConnection::close()
{
	uint8_t id = getId();
	bool ok;
	if (id != ESP_INVALID_CONNECTION)
		return parent.execute((String)"AT+CIPCLOSE=" + id, eCEM_ShortTimeOut);
	else
		return false;
}

///////////////////////////////////////////////////////////////////////

ESP8266proServerConection::ESP8266proServerConection(ESP8266pro& esp, uint8_t id)
	: ESP8266proConnection(esp), cid(id)
{
	uses = 0;
}

uint8_t ESP8266proServerConection::getId()
{
	return cid;
}

bool ESP8266proServerConection::send(String data)
{
	if (cid == ESP_INVALID_CONNECTION) return false; // Already losted link
	return ESP8266proConnection::send(data);
}

bool ESP8266proServerConection::send(const __FlashStringHelper* data)
{
	if (cid == ESP_INVALID_CONNECTION) return false; // Already losted link
	return ESP8266proConnection::send(data);
}

bool ESP8266proServerConection::close()
{
	if (cid == ESP_INVALID_CONNECTION) return false; // Already losted link
	return ESP8266proConnection::close();
}

void ESP8266proServerConection::incrimentUses()
{
	uses++;
}

void ESP8266proServerConection::decrementUses()
{
	uses--;
}

/*bool ESP8266proServerConection::isUsed()
{
	return uses > 0;
}*/

void ESP8266proServerConection::dispose()
{
	cid = ESP_INVALID_CONNECTION;
}