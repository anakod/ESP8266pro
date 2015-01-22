// ESP8266pro
// This software distributed under the terms of the MIT license
// (c) Skurydin Alexey, 2014

#ifndef _ESP8266pro_Parser_H_
#define _ESP8266pro_Parser_H_

//#define NODEBUG // Defined to reduce code size. Disable all debugging!

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Stream.h>

// Configurable by user
// IP receive buffer size
#define ESP_RECEIVE_BUFSIZE			80

// Internal params, depends on ESP Firmware
#define ESP_LINE_WRAPPER			"\r\n"
#define ESP_MAX_CONNECTIONS			5
#define ESP_STATUS_OK				"OK"
#define ESP_STATUS_SEND_OK			"SEND OK"
#define ESP_STATUS_ERR				"ERROR"
#define ESP_STATUS_NO_CHANGE		"no change"
#define ESP_STATUS_NO_LINK			"link is not"
#define ESP_STATUS_ONLINK			"link is builded"
#define ESP_IP_DATA_HANDLER			"+IPD,"

enum OutputDebugMode
{
	eODM_None = 0, // Disable debug output
	eODM_Data = 1, // Standard debug mode
	eODM_Dump = 2, // Not recomended, only for deep debuging purposes (ECHO OFF)
	eODM_FullDump = 6, // Not recomended, only for deep debuging purposes (ECHO ON)
};

enum ProcessingState
{
	ePS_NotInitialized,
	ePS_Ready,
	ePS_OK,
	ePS_Completed,
	ePS_Error,
	ePS_NoResponse
};

enum DataReceiveAction
{
	eDRA_Begin,
	eDRA_Packet,
	eDRA_End
};

enum CommandExecutionMode
{
	eCEM_DeafaultWithSelector, // Send cmd, apply response selector, wait "OK"
	eCEM_NoResponse,
	eCEM_NoLineBreak,
	eCEM_ShortTimeOut,
	eCEM_LongTimeOut
};

class ESP8266pro_Parser
{
public:
	ESP8266pro_Parser(Stream &espStreamReference);
	ESP8266pro_Parser(Stream &espStreamReference, Stream &debugStreamReference);
	~ESP8266pro_Parser();
	
	bool initializeParser(OutputDebugMode debugOutMode = eODM_Data);
	void restart();
	
	bool execute(const String& msg, CommandExecutionMode mode = eCEM_DeafaultWithSelector);
	ProcessingState getState();
	String getLine(uint8_t lineId);
	String getLineItem(uint8_t lineId, uint8_t itemId);
	int getLinesCount();
	
	// Internal methods
	void writeString(const __FlashStringHelper* data);
	bool connectionDataReceive(bool waitData = false);
	
protected:
	void resetParser();
	virtual void onDataReceive(uint8_t connectionId, char* buffer, int length, DataReceiveAction action);
#ifndef NODEBUG
	void debugPrint(const String& text, bool lineWrap = true);
	void debugPrint(const char* text, bool lineWrap = true);
#endif

private:
	void processConnectionDataReceive(bool processData = true);
	String readLine();
	bool parseResponse(const char* selector, int msTimeOut = 1500);
	String trimResponse(String originalLine);
	
private:
	Stream &espStream;
	Stream *debugStream;
	String response;
	ProcessingState state;
#ifndef NODEBUG
	OutputDebugMode debugMode;
#endif
};

#endif