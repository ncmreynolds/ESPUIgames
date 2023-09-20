/*
 *	An Arduino library to create simple games accessible through a web portal.
 *
 *	https://github.com/ncmreynolds/ESPUIgames
 *
 *	Released under LGPL-2.1 see https://github.com/ncmreynolds/ESPUIgames/LICENSE for full license
 *
 */
#ifndef ESPUIgames_h
#define ESPUIgames_h
#include <Arduino.h>

class ESPUIgames	{

	public:
		ESPUIgames();														//Constructor function
		~ESPUIgames();														//Destructor function
		void begin();															//Start the ESPUIgames
		void ESPUIgames::debug(Stream &);									//Start debugging on a stream
	protected:
	private:
		Stream *debug_uart_ = nullptr;											//The stream used for the debugging
};
#endif
