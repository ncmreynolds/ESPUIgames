/*
 *	An Arduino library to create simple games accessible through a web portal.
 *
 *	https://github.com/ncmreynolds/ESPUIgames
 *
 *	Released under LGPL-2.1 see https://github.com/ncmreynolds/ESPUIgames/LICENSE for full license
 *
 */
#ifndef ESPUIgames_cpp
#define ESPUIgames_cpp
#include "ESPUIgames.h"


ESPUIgames::ESPUIgames()	//Constructor function
{
}

ESPUIgames::~ESPUIgames()	//Destructor function
{
}

#if defined(ESP8266) || defined(ESP32)
void ICACHE_FLASH_ATTR ESPUIgames::begin()	{
#else
void ESPUIgames::begin()	{
#endif
	if(debug_uart_ != nullptr)
	{
		debug_uart_->println(F("ESPUIgames started"));
	}
}

#if defined(ESP8266) || defined(ESP32)
void ICACHE_FLASH_ATTR ESPUIgames::debug(Stream &terminalStream)
#else
void ESPUIgames::debug(Stream &terminalStream)
#endif
{
	debug_uart_ = &terminalStream;		//Set the stream used for the terminal
	#if defined(ESP8266)
	if(&terminalStream == &Serial)
	{
		  debug_uart_->write(17);			//Send an XON to stop the hung terminal after reset on ESP8266
	}
	#endif
}

#endif
