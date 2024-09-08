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

void ICACHE_FLASH_ATTR ESPUIgames::addGameTab()
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	if(debug_uart_ != nullptr)
	{
		debug_uart_->print(F("Starting game "));
		if(typeOfGame == gameType::simon)
		{
			debug_uart_->println(gameTypeDescriptionSimon);
		}
		else if(typeOfGame == gameType::whackamole)
		{
			debug_uart_->println(gameTypeDescriptionWhackAmole);
		}
	}
	//Assign the game title, if necessary
	if(gameTitle == nullptr)
	{
		if(typeOfGame == gameType::simon)
		{
			gameTitle = new char[strlen(gameTypeDescriptionSimon) + 1];
			strcpy(gameTitle, gameTypeDescriptionSimon);
		}
		else if(typeOfGame == gameType::whackamole)
		{
			gameTitle = new char[strlen(gameTypeDescriptionWhackAmole) + 1];
			strcpy(gameTitle, gameTypeDescriptionWhackAmole);
		}
	}
	//Create the game tab
	if(gameTabLabels[0] == nullptr)	//Set default tab title, if necessary
	{
		gameTabLabels[0] = gameTabLabel0;
	}
	gameTabIDs[0] = ESPUI.addControl(ControlType::Tab, gameTabLabels[0], gameTabLabels[0]);
	if(debug_uart_ != nullptr)
	{
		debug_uart_->print(F("Creating game objects for: "));
		debug_uart_->println(gameTitle);
	}
	//Add the start game switch
	if(startSwitchEnabled == true)
	{
		if(debug_uart_ != nullptr)
		{
			debug_uart_->print(F("Adding start switch: "));
			debug_uart_->println(startSwitchLabel);
		}
		startSwitchWidgetId = ESPUI.addControl(ControlType::Switcher, startSwitchLabel, startSwitchLabel, defaultGameColour, gameTabIDs[0], &startSwitchCallback);
		//ESPUI.setPanelWide(startSwitchWidgetId, true);
	}
	//Add the win box
	if(gameWinLabel != nullptr && gameWinContent != nullptr)
	{
		if(debug_uart_ != nullptr)
		{
			debug_uart_->print(F("Adding win content: "));
			debug_uart_->print(gameWinLabel);
			debug_uart_->print('/');
			debug_uart_->println(gameWinContent);
		}
		winWidgetId = ESPUI.addControl(ControlType::Label, gameWinLabel, gameWinContent, wonGameColour, gameTabIDs[0]);
		ESPUI.updateVisibility(winWidgetId, false);
	}
	//Add the lose box
	if(gameLoseLabel != nullptr && gameLoseContent != nullptr)
	{
		if(debug_uart_ != nullptr)
		{
			debug_uart_->print(F("Adding lose content: "));
			debug_uart_->print(gameLoseLabel);
			debug_uart_->print('/');
			debug_uart_->println(gameLoseContent);
		}
		loseWidgetId = ESPUI.addControl(ControlType::Label, gameLoseLabel, gameLoseContent, loseGameColour, gameTabIDs[0]);
		ESPUI.updateVisibility(loseWidgetId, false);
		if(gameTotalLossContent != nullptr)
		{
			if(debug_uart_ != nullptr)
			{
				debug_uart_->print(F("Adding lose content: "));
				debug_uart_->print(gameLoseLabel);
				debug_uart_->print('/');
				debug_uart_->println(gameTotalLossContent);
			}
			totalLossWidgetId = ESPUI.addControl(ControlType::Label, gameLoseLabel, gameTotalLossContent, loseGameColour, gameTabIDs[0]);
			ESPUI.updateVisibility(totalLossWidgetId, false);
		}
	}
	if(startSwitchEnabled == true || loseWidgetId != 0 || winWidgetId !=0)
	{
		ESPUI.addControl(ControlType::Separator, "", "", ControlColor::None, gameTabIDs[0]);
	}
	//Add the play buttons
	if(numberOfGamePlayButtons == 0)
	{
		if(typeOfGame == gameType::simon)
		{
			numberOfGamePlayButtons = 4;
		}
		else if(typeOfGame == gameType::whackamole)
		{
			numberOfGamePlayButtons = 6;
		}
		for(uint8_t index = 0; index < numberOfGamePlayButtons; index++)
		{
			gamePlayButtonTitle[index] = new char[12];
			sprintf_P(gamePlayButtonTitle[index], PSTR("Button %u"), index);
			gamePlayButtonLabel[index] = new char[3];
			sprintf_P(gamePlayButtonLabel[index], PSTR("%u"), index);
			gamePlayButtonColour[index] = gameColourPalette[index%numberOfGameColours];
			if(debug_uart_ != nullptr)
			{
				debug_uart_->print(F("Adding default button: "));
				debug_uart_->println(gamePlayButtonLabel[index]);
			}
			gamePlayButtonIDs[index] = ESPUI.addControl(ControlType::Button, gamePlayButtonTitle[index], gamePlayButtonLabel[index], defaultGameColour, gameTabIDs[0], &playButtonCallback);
			//ESPUI.setPanelWide(gamePlayButtonIDs[index], true);
		}
	}
	else
	{
		for(uint8_t index = 0; index < numberOfGamePlayButtons; index++)
		{
			if(debug_uart_ != nullptr)
			{
				debug_uart_->print(F("Adding button: "));
				debug_uart_->print(index);
				debug_uart_->print('-');
				debug_uart_->print(gamePlayButtonTitle[index]);
				debug_uart_->print('/');
				debug_uart_->println(gamePlayButtonLabel[index]);
			}
			gamePlayButtonIDs[index] = ESPUI.addControl(ControlType::Button, gamePlayButtonTitle[index], gamePlayButtonLabel[index], defaultGameColour, gameTabIDs[0], &playButtonCallback);
			//ESPUI.setPanelWide(gamePlayButtonIDs[index], true);
		}
	}
	//Set the pushed button flags to 'none'
	buttonLit = false;
	buttonPushed = numberOfGamePlayButtons;
	buttonReleased = numberOfGamePlayButtons;
	//Assign storage for the game choices, if necessary
	if(typeOfGame == gameType::simon)
	{
		gameChoices = new uint8_t[maximumGameLength];
	}
	else if(typeOfGame == gameType::whackamole)
	{
		buttonTriggered = new bool[numberOfGamePlayButtons];
	}
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}

void ICACHE_FLASH_ATTR ESPUIgames::addHelpTab()
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	//Set default tab title, if necessary
	if(gameTabLabels[1] == nullptr)
	{
		gameTabLabels[1] = gameTabLabel1;
	}
	//Create the game tab
	gameTabIDs[1] = ESPUI.addControl(ControlType::Tab, gameTabLabels[1], gameTabLabels[1]);
	//Add the help
	if(gameHelpLabel == nullptr)
	{
		gameHelpLabel = gameTabLabel1;
	}
	if(gameHelpContent == nullptr)
	{
		if(typeOfGame == gameType::simon)
		{
			gameHelpContent = gameHelp0;
		}
		else if(typeOfGame == gameType::whackamole)
		{
			gameHelpContent = gameHelp1;
		}
	}
	if(debug_uart_ != nullptr)
	{
		debug_uart_->print(F("Adding help content: "));
		debug_uart_->print(gameHelpLabel);
		debug_uart_->print('/');
		debug_uart_->println(gameHelpContent);
	}
	helpLabelID = ESPUI.addControl(ControlType::Label, gameHelpLabel, gameHelpContent, defaultGameColour, gameTabIDs[1]);
	//ESPUI.setPanelWide(helpLabelID, true);
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}

void ESPUIgames::hideHelpTab()
{
	ESPUI.updateVisibility(gameTabIDs[1], false);
}

void ESPUIgames::showHelpTab()
{
	ESPUI.updateVisibility(gameTabIDs[1], true);
}

void ICACHE_FLASH_ATTR ESPUIgames::debug(Stream &terminalStream)
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	debug_uart_ = &terminalStream;		//Set the stream used for the terminal
	#if defined(ESP8266)
	if(&terminalStream == &Serial)
	{
		  debug_uart_->write(17);			//Send an XON to stop the hung terminal after reset on ESP8266
	}
	#endif
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}

void ESPUIgames::type(gameType t)
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	typeOfGame = t;	//Set the game type
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}

void ESPUIgames::setLength(uint8_t length)
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	gameLength = length;
	if(debug_uart_ != nullptr)
	{
		  debug_uart_->print(F("Game length: "));
		  debug_uart_->println(gameLength);
	}
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
void ESPUIgames::setMaximumAttempts(uint8_t number)
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	maximumAttempts = number;
	if(debug_uart_ != nullptr)
	{
		  debug_uart_->print(F("Maximum attempts: "));
		  debug_uart_->println(maximumAttempts);
	}
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
void ESPUIgames::setGameSpeedup(uint32_t number)
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	gameSpeedupAmount = number;
	if(debug_uart_ != nullptr)
	{
		  debug_uart_->print(F("Speedup amount(ms): "));
		  debug_uart_->println(gameSpeedupAmount);
	}
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
void ESPUIgames::playButtonCallback(Control* sender, int value)
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	if(game.debug_uart_ != nullptr)
	{
		game.debug_uart_->print(debugControlColonSpace);
		game.debug_uart_->print(long(sender));
		game.debug_uart_->print(debugDashGt);
	}
    switch (value)
    {
		case B_DOWN:
			if(game.debug_uart_ != nullptr)
			{
				game.debug_uart_->println(debugDown);
			}
			game.buttonPushed = game.buttonIndexFromId(sender->id);
			break;

		case B_UP:
			if(game.debug_uart_ != nullptr)
			{
				game.debug_uart_->println(debugUp);
			}
			game.buttonReleased = game.buttonIndexFromId(sender->id);
			break;
    }
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
void ESPUIgames::startSwitchCallback(Control* sender, int value)
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	if(game.debug_uart_ != nullptr)
	{
		game.debug_uart_->print(debugControlColonSpace);
		game.debug_uart_->print(long(sender));
		game.debug_uart_->print(debugDashGt);
	}
    switch (value)
    {
		case S_ACTIVE:
			if(game.debug_uart_ != nullptr)
			{
				game.debug_uart_->println(debugOn);
			}
			if(game.gameStarted == false)
			{
				if(game.gameEnabled == true)
				{
					if(game.currentAttempt < game.maximumAttempts || game.maximumAttempts == 0)
					{
						if(game.debug_uart_ != nullptr)
						{
						  game.debug_uart_->println(F("Game beginning"));
						}
						game.startNewGame();
					}
					else
					{
						if(game.debug_uart_ != nullptr)
						{
						  game.debug_uart_->println(F("Game attempts exhausted"));
						}
					}
				}
				else
				{
					ESPUI.updateSwitcher(game.startSwitchWidgetId, false);
					if(game.debug_uart_ != nullptr)
					{
					  game.debug_uart_->println(F("Game not enabled"));
					}
				}
			}
			else
			{
				if(game.debug_uart_ != nullptr)
				{
				  game.debug_uart_->println(F("Game already started"));
				}
			}
			break;

		case S_INACTIVE:
			if(game.debug_uart_ != nullptr)
			{
				game.debug_uart_->println(debugOff);
			}
			if(game.maximumAttempts == 0 || game.currentAttempt < game.maximumAttempts)
			{
				ESPUI.getControl(game.startSwitchWidgetId)->color = game.defaultGameColour;
				ESPUI.updateControl(game.startSwitchWidgetId);
				if(game.gameWon == true || game.gameLost == true)
				{
					game.resetGame();
					//meshEventToAnnounce = meshHackFail;
				}
				else if(game.gameStarted == true)
				{
					game.stopCurrentGame();
				}
			}
			break;
    }
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
void ESPUIgames::setTitle(const char* title)
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	if(gameTitle != nullptr)
	{
		delete gameTitle;
	}
	gameTitle = new char[strlen(title) + 1];
	strcpy(gameTitle, title);
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
char* ESPUIgames::title()
{
	return gameTitle;
}
void ESPUIgames::setTabTitle(const char* title)
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	if(gameTabLabels[0] != nullptr)
	{
		delete gameTabLabels[0];
	}
	gameTabLabels[0] = new char[strlen(title) + 1];
	strcpy(gameTabLabels[0], title);
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
void ESPUIgames::setWinContent(const char* label, const char* content)
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	if(gameWinLabel != nullptr)
	{
		delete gameWinLabel;
	}
	if(gameWinContent != nullptr)
	{
		delete gameWinContent;
	}
	gameWinLabel = new char[strlen(label) + 1];
	strcpy(gameWinLabel, label);
	gameWinContent = new char[strlen(content) + 1];
	strcpy(gameWinContent, content);
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
void ESPUIgames::setLoseContent(const char* label, const char* content, const char* completeLossContent)
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	if(gameLoseLabel != nullptr)
	{
		delete gameLoseLabel;
	}
	if(gameLoseContent != nullptr)
	{
		delete gameLoseContent;
	}
	gameLoseLabel = new char[strlen(label) + 1];
	strcpy(gameLoseLabel, label);
	gameLoseContent = new char[strlen(content) + 1];
	strcpy(gameLoseContent, content);
	if(completeLossContent != nullptr)
	{
		gameTotalLossContent = new char[strlen(completeLossContent) + 1];
		strcpy(gameTotalLossContent, completeLossContent);
	}
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}

void ESPUIgames::hideGameTab()
{
	ESPUI.updateVisibility(gameTabIDs[0], false);
	ESPUI.jsonReload();	//Needed to force a refresh
}

void ESPUIgames::showGameTab()
{
	ESPUI.updateVisibility(gameTabIDs[0], true);
	ESPUI.jsonReload();	//Needed to force a refresh
}

void ESPUIgames::setHelpTabTitle(const char* title)
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	if(gameTabLabels[1] != nullptr)
	{
		delete gameTabLabels[1];
	}
	gameTabLabels[1] = new char[strlen(title) + 1];
	strcpy(gameTabLabels[1], title);
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
void ESPUIgames::setHelpContent(const char* label, const char* content)
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	if(gameHelpLabel != nullptr)
	{
		delete gameHelpLabel;
	}
	if(gameHelpContent != nullptr)
	{
		delete gameHelpContent;
	}
	gameHelpLabel = new char[strlen(label) + 1];
	strcpy(gameHelpLabel, label);
	gameHelpContent = new char[strlen(content) + 1];
	strcpy(gameHelpContent, content);
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
void ESPUIgames::enableStartSwitch(const char* label)
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	if(label != nullptr)
	{
		startSwitchLabel = new char[strlen(label) + 1];
		strcpy(startSwitchLabel, label);
	}
	else
	{
		startSwitchLabel = defaultStartSwitchLabel;
	}
	startSwitchEnabled = true;
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}

bool ESPUIgames::addPlayButton(const char* title, const char* label, const ControlColor colour)
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	if(numberOfGamePlayButtons == maximumNumberOfGameButtons)
	{
		return false;
	}
	gamePlayButtonTitle[numberOfGamePlayButtons] = new char[strlen(title) + 1];
	strcpy(gamePlayButtonTitle[numberOfGamePlayButtons], title);
	gamePlayButtonLabel[numberOfGamePlayButtons] = new char[strlen(label) + 1];
	strcpy(gamePlayButtonLabel[numberOfGamePlayButtons], label);
	gamePlayButtonColour[numberOfGamePlayButtons] = colour;
	numberOfGamePlayButtons++;
	return true;
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
void ESPUIgames::lightButton(uint8_t index)
{
	lightButton(index, gamePlayButtonColour[index]);
}
void ESPUIgames::lightButton(uint8_t index, ControlColor colour)
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	ESPUI.getControl(gamePlayButtonIDs[index])->color = colour;
	ESPUI.updateControl(gamePlayButtonIDs[index]);
	if(debug_uart_ != nullptr)
	{
		debug_uart_->printf_P(PSTR("Lit button %u\n\r"),index);
	}
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
void ESPUIgames::extinguishButton(uint8_t index)
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	ESPUI.getControl(gamePlayButtonIDs[index])->color = defaultGameColour;
	ESPUI.updateControl(gamePlayButtonIDs[index]);
	if(debug_uart_ != nullptr)
	{
		debug_uart_->printf_P(PSTR("Extinguished button %u\n\r"),index);
	}
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
void ESPUIgames::lightEverything(ControlColor colour)
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	if(startSwitchEnabled == true)
	{
		ESPUI.getControl(startSwitchWidgetId)->color = colour;
		ESPUI.updateControl(startSwitchWidgetId);
	}
	for(uint8_t index = 0; index < numberOfGamePlayButtons; index++)
	{
		ESPUI.getControl(gamePlayButtonIDs[index])->color = colour;
		ESPUI.updateControl(gamePlayButtonIDs[index]);
	}
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
uint8_t ESPUIgames::buttonIndexFromId(uint16_t id)
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	for(uint8_t index = 0; index < numberOfGamePlayButtons; index++)
	{
		if(gamePlayButtonIDs[index] == id)
		{
			return(index);
		}
	}
	return(0);
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}

void ESPUIgames::startNewGame()
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	if(debug_uart_ != nullptr)
	{
		debug_uart_->print(F("New "));
		if(typeOfGame == gameType::simon)
		{
			debug_uart_->print(gameTypeDescriptionSimon);
			debug_uart_->printf_P(PSTR("game, %02u turns long\r\n"),gameLength);
		}
		else if(typeOfGame == gameType::whackamole)
		{
			debug_uart_->println(gameTypeDescriptionWhackAmole);
		}
	}
	if(typeOfGame == gameType::simon)	//Decide game moves
	{
		for(uint8_t i = 0; i < gameLength;i++)
		{
			gameChoices[i] = random(0,4);
			if(debug_uart_ != nullptr)
			{
				debug_uart_->print(gameChoices[i]);
				debug_uart_->print(' ');
			}
		}
	}
	else if(typeOfGame == gameType::whackamole)	//Clear moles
	{
		gameDelay = 5000;
		maxGameDelay = 5000;
		for(uint8_t index = 0; index < numberOfGamePlayButtons;index++)
		{
			buttonTriggered[index] = false;
		}
	}
	if(debug_uart_ != nullptr)
	{
		debug_uart_->println();
	}
	if(startSwitchEnabled == true)
	{
		ESPUI.getControl(startSwitchWidgetId)->color = highlightGameColour;
		ESPUI.updateControl(startSwitchWidgetId);
	}
	for(uint8_t index = 0; index < numberOfGamePlayButtons; index++)
	{
		extinguishButton(index);
	}
	hideLoseWidget();
	hideWinWidget();
	gameLost = false;
	gameWon = false;
	numberOfLitPlayButtons = 0;
	gameStarted = true;
	//gameDelay = 5000; //Wait 5s before starting the game
	gameTimer = millis();
	successLevel = 0;
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
void ESPUIgames::stopCurrentGame()
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	successLevel = 0;
	gameStarted = false;
	gamePosition = 0;
	computerTurn = true;
	buttonLit = false;
	gameWon = false;
	/*
	if(startSwitchEnabled == true)
	{
		ESPUI.getControl(startSwitchWidgetId)->color = defaultGameColour;
		ESPUI.updateControl(startSwitchWidgetId);
	}
	for(uint8_t index = 0; index < numberOfGamePlayButtons; index++)
	{
		extinguishButton(index);
	}
	*/
	//meshEventToAnnounce = meshHackFail;
	if(debug_uart_ != nullptr)
	{
		debug_uart_->print(debugGameSpace);
		debug_uart_->println(debugStopped);
	}
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
void ESPUIgames::resetGame()
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	successLevel = 0;
	gameStarted = false;
	gamePosition = 0;
	computerTurn = true;
	buttonLit = false;
	gameWon = false;
	if(startSwitchEnabled == true)
	{
		ESPUI.getControl(startSwitchWidgetId)->color = defaultGameColour;
		ESPUI.updateControl(startSwitchWidgetId);
	}
	for(uint8_t index = 0; index < numberOfGamePlayButtons; index++)
	{
		extinguishButton(index);
	}
	hideWinWidget();
	hideLoseWidget();
	if(debug_uart_ != nullptr)
	{
		debug_uart_->print(debugGameSpace);
		debug_uart_->println(debugReset);
	}
	//meshEventToAnnounce = meshHackFail;
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
void ESPUIgames::runFsm()
{
	#ifdef ESP8266
	{ //HeapSelectIram doAllocationsInIRAM;
	#endif
	if(typeOfGame == gameType::simon)
	{
		if(gameEnabled == false && millis() - lastGameCheckTime > gameCheckInterval)  //Only enable the game if ICE is up
		{
			lastGameCheckTime = millis();
			/*
			if(m2mMesh.joined() == true)
			{
				gameEnabled = true;
				#ifdef SERIAL_DEBUG
				debug_uart_->println(F("Game online"));
				#endif
			}
			else
			{
				#ifdef SERIAL_DEBUG
				debug_uart_->println(F("Game offline"));
				#endif
			}
			*/
		}
		if(gameStarted == false)
		{
			/*
			if(millis() - blinkTimer > blinkInterval)
			{
				blinkTimer = millis();
				if(redLEDfitted)
				{
					if(redLEDstate == true)
					{
						digitalWrite(redLEDpin,LOW);
						blinkInterval = maxBlinkInterval;
						redLEDstate = false;
					}
					else
					{
						digitalWrite(redLEDpin,HIGH);
						blinkInterval = minBlinkInterval;
						redLEDstate = true;
					}
				}
			}
			*/
			if(buttonPushed != numberOfGamePlayButtons)
			{
				buttonPushed = numberOfGamePlayButtons; //Cancel the early push;
			}
		}
		else
		{
			if(successLevel == 0)
			{
				/*
				if(greenLEDstate == true || redLEDstate == false)
				{
					if(greenLEDfitted)
					{
						greenLEDstate = false;
						digitalWrite(greenLEDpin,LOW);
					}
					if(redLEDfitted)
					{
						redLEDstate = true;
						digitalWrite(redLEDpin,HIGH);
					}
				}
				*/
			}
			else if(successLevel == gameLength)
			{
				/*
				if(greenLEDstate == false || redLEDstate == true)
				{
					if(redLEDfitted)
					{
						redLEDstate = false;
						digitalWrite(redLEDpin,LOW);
					}
					if(greenLEDfitted)
					{
						greenLEDstate = true;
						digitalWrite(greenLEDpin,HIGH);
					}
				}
				*/
			}
			//else if(millis() - blinkTimer > blinkInterval) //Swap the LEDs at a rate relative to the success level
			{
				/*
				blinkTimer = millis();
				if(redLEDfitted && greenLEDfitted)
				{
					if(redLEDstate == true)
					{
						redLEDstate = false;
						digitalWrite(redLEDpin,LOW);
						greenLEDstate = true;
						digitalWrite(greenLEDpin,HIGH);
						blinkInterval = maxBlinkInterval * gameLength / (gameLength - successLevel);
					}
					else
					{
						greenLEDstate = false;
						digitalWrite(greenLEDpin,LOW);
						redLEDstate = true;
						digitalWrite(redLEDpin,HIGH);
						blinkInterval = maxBlinkInterval * gameLength / successLevel;
					}
				}
				*/
			}
			/*
			 * Playing the game with the user
			 * 
			 */
			if(computerTurn == true)
			{
				if(millis() - gameTimer > gameDelay)
				{
					gameTimer = millis();
					if(buttonLit == false)
					{
						lightButton(gameChoices[gamePosition]);
						gameDelay = playButtonOnTime;
						buttonLit = true;
					}
					else
					{
						extinguishButton(gameChoices[gamePosition]);
						buttonLit = false;
						gameDelay = playButtonOffTime;
						gamePosition++;
					}
				}
				if(gamePosition > successLevel)  //Swap to player
				{
					gamePosition = 0;
					computerTurn = false;
					if(debug_uart_ != nullptr)
					{
						debug_uart_->print(debugPlayerSpace);
						debug_uart_->println(debugTurn);
					}
				}
			}
			else //Player turn
			{
				if(buttonPushed != numberOfGamePlayButtons)
				{
					if(buttonLit == false)  //Check pushed button
					{
						gameTimer = millis();
						gameDelay = playButtonOnTime;
						if(buttonPushed == gameChoices[gamePosition]) //Right button
						{
							lightButton(buttonPushed);
							buttonLit = true;
						}
						else  //Wrong button, you lose
						{
							loseGame();
						}
						buttonPushed = numberOfGamePlayButtons; //Cancel the push;
					}
				}
				if(buttonReleased != numberOfGamePlayButtons) //Released
				{
					if(gameLost == false && buttonLit == true && millis() - gameTimer > minimumOnTime)
					{
						extinguishButton(gameChoices[gamePosition]);
						buttonLit = false;
						gamePosition++;
					}
					buttonReleased = numberOfGamePlayButtons;
				}
				if(buttonLit == true) //Time the button out if held a long time
				{
					if(millis() - gameTimer > gameDelay)
					{
						if(gameLost == true)
						{
							stopCurrentGame();
						}
						else
						{
							extinguishButton(gameChoices[gamePosition]);
							buttonLit = false;
							gamePosition++;
						}
					}
				}
				if(millis() - gameTimer > gameMoveTimeout)  //Player didn't make a move, they lose
				{
					loseGame();
				}
				if(gamePosition > successLevel)
				{
					successLevel = gamePosition;
					if(successLevel == gameLength)
					{
						gameWon = true;
						gameStarted = false;
						//meshEventToAnnounce = meshHackSucceed;
						if(debug_uart_ != nullptr)
						{
							debug_uart_->print(debugGameSpace);
							debug_uart_->println(debugWon);
						}
						//Light up all the buttons
						lightEverything(wonGameColour);
						//Pop up a widget
						showWinWidget();
						/*
						if(redLEDfitted)
						{
							redLEDstate = false;
							digitalWrite(redLEDpin,LOW);
						}
						if(greenLEDfitted)
						{
							greenLEDstate = true;
							digitalWrite(greenLEDpin,HIGH);
						}
						*/
						#if defined(FILE_BROWSER)
						//ESPUI.setEnabled(exampleFileViewControl, true);
						//ESPUI.updateControl(exampleFileViewControl);
						//ESPUI.setEnabled(exampleFileDownloadControl, true);
						//ESPUI.updateControl(exampleFileDownloadControl);
						//ESPUI.setEnabled(exampleFileDeleteControl, true);
						//ESPUI.updateControl(exampleFileDeleteControl);
						//ESPUI.removeControl(fileSystemLockedLabel,true);
						//addFilesFromDirectory("files/", 1, fileSystemTab);
						#endif
					}
					else
					{
						computerTurn = true;
						gameTimer = millis();
						gameDelay = playButtonOnTime;
						gamePosition = 0;
						if(debug_uart_ != nullptr)
						{
							debug_uart_->print(debugComputerSpace);
							debug_uart_->println(debugTurn);
						}
					}
				}
			}
		}
	}
	else if(typeOfGame == gameType::whackamole)
	{
		if(gameStarted == false)
		{
			/*
			if(millis() - blinkTimer > blinkInterval)
			{
				blinkTimer = millis();
				if(redLEDfitted)
				{
					if(redLEDstate == true)
					{
						digitalWrite(redLEDpin,LOW);
						blinkInterval = maxBlinkInterval;
						redLEDstate = false;
					}
					else
					{
						digitalWrite(redLEDpin,HIGH);
						blinkInterval = minBlinkInterval;
						redLEDstate = true;
					}
				}
			}
			*/
		}
		else
		{
			/*
			if(millis() - lastMeshMessage > alertTimeout)
			{
				stopGame();
			}
			*/
			if(successLevel == gameLength)
			{
				gameWon = true;
				gameStarted = false;
				//meshEventToAnnounce = meshHackSucceed;
				if(debug_uart_ != nullptr)
				{
					debug_uart_->print(debugGameSpace);
					debug_uart_->println(debugWon);
				}
				//Light up all the buttons
				lightEverything(wonGameColour);
				//Pop up a widget
				showWinWidget();
				/*
				if(redLEDfitted)
				{
					redLEDstate = false;
					digitalWrite(redLEDpin,LOW);
				}
				if(greenLEDfitted)
				{
					greenLEDstate = true;
					digitalWrite(greenLEDpin,HIGH);
				}
				*/
				#if defined(FILE_BROWSER)
				//ESPUI.setEnabled(exampleFileViewControl, true);
				//ESPUI.updateControl(exampleFileViewControl);
				//ESPUI.setEnabled(exampleFileDownloadControl, true);
				//ESPUI.updateControl(exampleFileDownloadControl);
				//ESPUI.setEnabled(exampleFileDeleteControl, true);
				//ESPUI.updateControl(exampleFileDeleteControl);
				//ESPUI.removeControl(fileSystemLockedLabel,true);
				//addFilesFromDirectory("files/", 1, fileSystemTab);
				#endif
			}
			else if(gameLost == false)
			{
				newMole();
			}
			if(buttonPushed < numberOfGamePlayButtons)
			{
				whackMole(buttonPushed); //Do this here rather than in the callback, to fix stability issue
			}
		}
	}
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
void ESPUIgames::loseGame()
{
	currentAttempt++;	//Increase the attempt number
	if(typeOfGame == gameType::simon)
	{
		gameLength++;	//Make the game harder
		if(gameLength > maximumGameLength)
		{
			gameLength = maximumGameLength;
		}
		if(maxGameDelay > 1000)
		{
			maxGameDelay = maxGameDelay - gameSpeedupAmount;
		}
	}
	gameDelay = maxGameDelay;
	buttonLit = true;
	gameLost = true;
	lightEverything(loseGameColour);
	showLoseWidget();
	gameTimer = millis();
	buttonPushed = numberOfGamePlayButtons; //Cancel any push;
	//meshEventToAnnounce = meshHackFail;
	if(debug_uart_ != nullptr)
	{
		debug_uart_->print(debugGameSpace);
		debug_uart_->print(debugLost);
		debug_uart_->print(spaceAttemptspace);
		debug_uart_->print(currentAttempt);
		debug_uart_->print('/');
		debug_uart_->print(maximumAttempts);
		debug_uart_->print(F(" length: "));
		debug_uart_->print(gameLength);
		debug_uart_->print(F(" move time: "));
		debug_uart_->println(gameDelay);
	}
}
void ESPUIgames::newMole()
{
	if(numberOfLitPlayButtons < numberOfGamePlayButtons)
	{
		if(millis() - gameTimer > gameDelay)
		{
			gameTimer = millis();
			uint8_t randomButton = random(0, numberOfGamePlayButtons);  //Pick a random mole
			if(buttonTriggered[randomButton] == false)
			{
				lightButton(randomButton, ControlColor::Sunflower);
				buttonTriggered[randomButton] = true;
				numberOfLitPlayButtons++;
				if(debug_uart_ != nullptr)
				{
					debug_uart_->printf("New mole %02u ID %02u total %02u\n",randomButton,gamePlayButtonIDs[randomButton],numberOfLitPlayButtons);
				}
				return;
			}
			else
			{
				lightButton(randomButton, ControlColor::Alizarin);
				gameSpeedup();
				return;
			}
			if(debug_uart_ != nullptr)
			{
				debug_uart_->print('.');
			}
		}
	}
	else if(numberOfLitPlayButtons == numberOfGamePlayButtons)
	{
		loseGame();
	}
}
void ESPUIgames::whackMole(uint8_t index)
{
	if(buttonTriggered[index])
	{
		buttonPushed = numberOfGamePlayButtons;
		buttonTriggered[index] = false;
		if(numberOfLitPlayButtons > 0)
		{
			numberOfLitPlayButtons--;
		}
		extinguishButton(index);
		successLevel++;
		if(debug_uart_ != nullptr)
		{
			debug_uart_->printf("Whacked mole %02u now %02u, score %03u!\n", index, numberOfLitPlayButtons, successLevel);
		}
		if(numberOfLitPlayButtons == 0)
		{
			if(random(0,5) == 4)
			{
				gameSpeedReset();
			}
			else
			{
				gameSpeedup();
			}
		}
	}
	else
	{
		buttonPushed = numberOfGamePlayButtons; //Cancel the incorrect push;
	}
}

void ESPUIgames::gameSpeedup()
{
	if(gameDelay > gameSpeedupAmount)
	{
		gameDelay -= gameSpeedupAmount;
		//displayAlertLevel();
		if(debug_uart_ != nullptr)
		{
			debug_uart_->printf_P(PSTR("Moles sped up to every %ums\n\r"),gameDelay);
		}
	}
	else
	{
		gameDelay = gameSpeedupAmount;
	}
}
void ESPUIgames::gameSpeedReset()
{
	gameDelay = maxGameDelay;
	if(maxGameDelay > gameSpeedupAmount)
	{
		maxGameDelay = maxGameDelay - gameSpeedupAmount;
		//displayAlertLevel();
		if(debug_uart_ != nullptr)
		{
			debug_uart_->printf_P(PSTR("Mole interval reset, now %ums\n\r"),maxGameDelay);
		}
	}
}
void ESPUIgames::hideWinWidget()
{
	if(winWidgetId != 0 && winWidgetVisible == true)
	{
		if(debug_uart_ != nullptr)
		{
			debug_uart_->print(debugHidingSpace);
			debug_uart_->print(debugWinSpace);
			debug_uart_->println(debugWidget);
		}
		ESPUI.updateVisibility(winWidgetId, false);
		//ESPUI.updateControl(winWidgetId);
		ESPUI.jsonReload();
		winWidgetVisible = false;
	}
}
void ESPUIgames::showWinWidget()
{
	if(winWidgetId != 0 && winWidgetVisible == false)
	{
		if(debug_uart_ != nullptr)
		{
			debug_uart_->print(debugShowingSpace);
			debug_uart_->print(debugWinSpace);
			debug_uart_->println(debugWidget);
		}
		ESPUI.updateVisibility(winWidgetId, true);
		//ESPUI.updateControl(winWidgetId);
		ESPUI.jsonReload();
		winWidgetVisible = true;
	}
}
void ESPUIgames::hideLoseWidget()
{
	if(loseWidgetId != 0 && loseWidgetVisible == true)
	{
		if(debug_uart_ != nullptr)
		{
			debug_uart_->print(debugHidingSpace);
			debug_uart_->print(debugLoseSpace);
			debug_uart_->println(debugWidget);
		}
		ESPUI.updateVisibility(loseWidgetId, false);
		if(gameTotalLossContent != nullptr)
		{
			ESPUI.updateVisibility(totalLossWidgetId, false);
		}
		//ESPUI.updateControl(loseWidgetId);
		ESPUI.jsonReload();
		loseWidgetVisible = false;
	}
}
void ESPUIgames::showLoseWidget()
{
	if(loseWidgetId != 0 && loseWidgetVisible == false)
	{
		if(gameTotalLossContent != nullptr && maximumAttempts > 0 && currentAttempt >= maximumAttempts)
		{
			if(debug_uart_ != nullptr)
			{
				debug_uart_->print(debugShowingSpace);
				debug_uart_->print(debugTotalLossSpace);
				debug_uart_->println(debugWidget);
			}
			ESPUI.updateVisibility(totalLossWidgetId, true);
			gameTotallyLost = true;
			//ESPUI.updateControl(loseWidgetId);
		}
		else
		{
			if(debug_uart_ != nullptr)
			{
				debug_uart_->print(debugShowingSpace);
				debug_uart_->print(debugLoseSpace);
				debug_uart_->println(debugWidget);
			}
			ESPUI.updateVisibility(loseWidgetId, true);
			//ESPUI.updateControl(loseWidgetId);
		}
		ESPUI.jsonReload();
		loseWidgetVisible = true;
	}
}
bool ESPUIgames::playing()
{
	return gameStarted;
}
bool ESPUIgames::won()
{
	return gameWon;
}
bool ESPUIgames::lost()
{
	return gameLost || gameTotallyLost;
}

ESPUIgames game;	//Create an instance of the class, as only one is practically usable at a time
#endif
