// ESP8266pro
// This software distributed under the terms of the MIT license
// (c) Skurydin Alexey, 2014

#include "ESP8266pro.h"
#include "ESP8266proConnectionBase.h"

ESP8266proConnection::ESP8266proConnection(ESP8266pro& esp) : parent(esp)
{
}

boolean ESP8266proConnection::send(String data)
{
	return internalSend(NULL, data.c_str());
}

boolean ESP8266proConnection::send(const __FlashStringHelper* data)
{
	return internalSend(data, NULL);
}

boolean ESP8266proConnection::internalSend(const __FlashStringHelper* dataP, const char* dataR)
{
	if (dataR == NULL)
		dataR = "";

	bool ok;
	int len = 0;
	if (dataP != NULL)
		len += strlen_P(reinterpret_cast<PGM_P>(dataP));
	len += strlen(dataR);
	
	for (unsigned char i = 0; i < 3; i++)
	{
		int id = getId();
		if (id == -1) return false;
		parent.execute((String)"AT+CIPSEND=" + id + "," + len, eCEM_NoResponse);	
		
		// Write request data
		if (dataP != NULL)
			parent.writeString(dataP);
		
		ok = parent.execute(dataR, eCEM_NoLineBreak);
		if (ok)
			break; // Success
		else if (parent.getState() == ePS_Busy)
			continue; // Trying to repeat
		else
			return false; // Error
	}
	if (!ok) return false;
	
	// Wait "SEND OK" response
	for (unsigned char i = 0; i < 3; i++)
	{
		if (getId() == -1) break; // Already closed..
		if (parent.getState() == ePS_Completed || parent.getState() == ePS_Error) break;
		parent.execute("", eCEM_NoLineBreak);
	}
	
	return parent.getState() == ePS_Completed;
}

boolean ESP8266proConnection::close()
{
	int id = getId();
	boolean ok;
	if (id != -1)
		return parent.execute((String)"AT+CIPCLOSE=" + id, eCEM_ShortTimeOut);
	else
		return false;
}

///////////////////////////////////////////////////////////////////////

ESP8266proServerConection::ESP8266proServerConection(ESP8266pro& esp, int id)
	: ESP8266proConnection(esp), cid(id)
{
	uses = 0;
}

int ESP8266proServerConection::getId()
{
	return cid;
}

boolean ESP8266proServerConection::send(String data)
{
	if (cid == -1) return false; // Already losted link
	return ESP8266proConnection::send(data);
}

boolean ESP8266proServerConection::send(const __FlashStringHelper* data)
{
	if (cid == -1) return false; // Already losted link
	return ESP8266proConnection::send(data);
}

boolean ESP8266proServerConection::close()
{
	if (cid == -1) return false; // Already losted link
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

boolean ESP8266proServerConection::isUsed()
{
	return uses > 0;
}

void ESP8266proServerConection::dispose()
{
	cid = -1;
}