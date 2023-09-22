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
	{ HeapSelectIram doAllocationsInIRAM;
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
		ESPUI.setPanelWide(startSwitchWidgetId, true);
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
	}
	if(startSwitchEnabled == true || loseWidgetId != 0 || winWidgetId !=0)
	{
		ESPUI.addControl(ControlType::Separator, "", "", ControlColor::None, gameTabIDs[0]);
	}
	//Add the play buttons
	if(numberOfGameButtons == 0)
	{
		numberOfGameButtons = 4;
		for(uint8_t index = 0; index < numberOfGameButtons; index++)
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
			ESPUI.setPanelWide(gamePlayButtonIDs[index], true);
		}
	}
	//Set the pushed button flags to 'none'
	buttonLit = false;
	buttonPushed = numberOfGameButtons;
	buttonReleased = numberOfGameButtons;
	//Assign storage for the game choices, if necessary
	if(typeOfGame == gameType::simon)
	{
		gameChoices = new uint8_t[gameLength];
	}
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}

void ICACHE_FLASH_ATTR ESPUIgames::addHelpTab()
{
	#ifdef ESP8266
	{ HeapSelectIram doAllocationsInIRAM;
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
	ESPUI.setPanelWide(helpLabelID, true);
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}

void ICACHE_FLASH_ATTR ESPUIgames::debug(Stream &terminalStream)
{
	#ifdef ESP8266
	{ HeapSelectIram doAllocationsInIRAM;
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
	{ HeapSelectIram doAllocationsInIRAM;
	#endif
	typeOfGame = t;	//Set the game type
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}

void ESPUIgames::setLength(uint8_t length)
{
	#ifdef ESP8266
	{ HeapSelectIram doAllocationsInIRAM;
	#endif
	gameLength = length;
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}

void ESPUIgames::playButtonCallback(Control* sender, int value)
{
	#ifdef ESP8266
	{ HeapSelectIram doAllocationsInIRAM;
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
	{ HeapSelectIram doAllocationsInIRAM;
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
				if(game.debug_uart_ != nullptr)
				{
				  game.debug_uart_->println(F("Game beginning"));
				}
				game.startNewGame();
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
			break;
    }
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
void ESPUIgames::setTitle(const char* title)
{
	#ifdef ESP8266
	{ HeapSelectIram doAllocationsInIRAM;
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
	{ HeapSelectIram doAllocationsInIRAM;
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
	{ HeapSelectIram doAllocationsInIRAM;
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
void ESPUIgames::setLoseContent(const char* label, const char* content)
{
	#ifdef ESP8266
	{ HeapSelectIram doAllocationsInIRAM;
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
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}

void ESPUIgames::setHelpTabTitle(const char* title)
{
	#ifdef ESP8266
	{ HeapSelectIram doAllocationsInIRAM;
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
	{ HeapSelectIram doAllocationsInIRAM;
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
	{ HeapSelectIram doAllocationsInIRAM;
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

bool ESPUIgames::addPlayButton(char* title, char* label, ControlColor colour)
{
	#ifdef ESP8266
	{ HeapSelectIram doAllocationsInIRAM;
	#endif
	if(numberOfGameButtons == maximumNumberOfGameButtons)
	{
		return false;
	}
	return true;
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
void ESPUIgames::lightButton(uint8_t index)
{
	#ifdef ESP8266
	{ HeapSelectIram doAllocationsInIRAM;
	#endif
	ESPUI.getControl(gamePlayButtonIDs[index])->color = gamePlayButtonColour[index];
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
	{ HeapSelectIram doAllocationsInIRAM;
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
	{ HeapSelectIram doAllocationsInIRAM;
	#endif
	if(startSwitchEnabled == true)
	{
		ESPUI.getControl(startSwitchWidgetId)->color = colour;
		ESPUI.updateControl(startSwitchWidgetId);
	}
	for(uint8_t index = 0; index < numberOfGameButtons; index++)
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
	{ HeapSelectIram doAllocationsInIRAM;
	#endif
	for(uint8_t index = 0; index < numberOfGameButtons; index++)
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
	{ HeapSelectIram doAllocationsInIRAM;
	#endif
	if(debug_uart_ != nullptr)
	{
		debug_uart_->print(F("New "));
		if(typeOfGame == gameType::simon)
		{
			debug_uart_->print(gameTypeDescriptionSimon);
		}
		else if(typeOfGame == gameType::whackamole)
		{
			debug_uart_->print(gameTypeDescriptionWhackAmole);
		}
		debug_uart_->printf_P(PSTR("game, %02u turns long\r\n"),gameLength);
	}
	for(uint8_t i = 0; i < gameLength;i++)
	{
		gameChoices[i] = random(0,4);
		if(debug_uart_ != nullptr)
		{
			debug_uart_->print(gameChoices[i]);
			debug_uart_->print(' ');
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
	for(uint8_t index = 0; index < numberOfGameButtons; index++)
	{
		extinguishButton(index);
	}
	hideLoseWidget();
	hideWinWidget();
	gameLost = false;
	gameStarted = true;
	gameDelay = 5000; //Wait 5s before starting the game
	gameTimer = millis();
	successLevel = 0;
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
}
void ESPUIgames::stopCurrentGame()
{
	#ifdef ESP8266
	{ HeapSelectIram doAllocationsInIRAM;
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
	for(uint8_t index = 0; index < numberOfGameButtons; index++)
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
	{ HeapSelectIram doAllocationsInIRAM;
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
	for(uint8_t index = 0; index < numberOfGameButtons; index++)
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
	{ HeapSelectIram doAllocationsInIRAM;
	#endif
	if(typeOfGame == gameType::simon)
	{
		/*
		if(firstRun == true)
		{
			if(winWidgetId != 0)
			{
				ESPUI.updateVisibility(winWidgetId, false);
			}
			if(loseWidgetId != 0)
			{
				ESPUI.updateVisibility(loseWidgetId, false);
			}
			firstRun = false;
		}
		*/
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
			if(buttonPushed != numberOfGameButtons)
			{
				buttonPushed = numberOfGameButtons; //Cancel the early push;
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
				if(buttonPushed != numberOfGameButtons)
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
						else  //Wrong button, light them all
						{
							buttonLit = true;
							gameLost = true;
							lightEverything(loseGameColour);
							showLoseWidget();
							//meshEventToAnnounce = meshHackFail;
							if(debug_uart_ != nullptr)
							{
								debug_uart_->print(debugGameSpace);
								debug_uart_->println(debugLost);
							}
						}
						buttonPushed = numberOfGameButtons; //Cancel the push;
					}
				}
				if(buttonReleased != numberOfGameButtons) //Released
				{
					if(gameLost == false && buttonLit == true && millis() - gameTimer > minimumOnTime)
					{
						extinguishButton(gameChoices[gamePosition]);
						buttonLit = false;
						gamePosition++;
					}
					buttonReleased = numberOfGameButtons;
				}
				if(buttonLit == true) //Time the button out if held a long time
				{
					if(millis() - gameTimer > gameDelay)
					{
						if(gameLost == true)
						{
							stopCurrentGame();
							//disableUplink();
							gameLost = false;
						}
						else
						{
							extinguishButton(gameChoices[gamePosition]);
							buttonLit = false;
							gamePosition++;
						}
					}
				}
				if(millis() - gameTimer > gameMoveTimeout)  //Player didn't make a move
				{
					buttonLit = true;
					gameLost = true;
					lightEverything(loseGameColour);
					showLoseWidget();
					//meshEventToAnnounce = meshHackFail;
					gameTimer = millis();
					gameDelay = playButtonOnTime;
					buttonPushed = numberOfGameButtons; //Cancel the push;
					if(debug_uart_ != nullptr)
					{
						debug_uart_->print(debugGameSpace);
						debug_uart_->println(debugTimedOut);
					}
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
	}
	#ifdef ESP8266
	} // HeapSelectIram
	#endif
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
		ESPUI.jsonReload();
		loseWidgetVisible = false;
	}
}
void ESPUIgames::showLoseWidget()
{
	if(loseWidgetId != 0 && loseWidgetVisible == false)
	{
		if(debug_uart_ != nullptr)
		{
			debug_uart_->print(debugShowingSpace);
			debug_uart_->print(debugLoseSpace);
			debug_uart_->println(debugWidget);
		}
		ESPUI.updateVisibility(loseWidgetId, true);
		ESPUI.jsonReload();
		loseWidgetVisible = true;
	}
}
ESPUIgames game;	//Create an instance of the class, as only one is practically usable at a time
#endif
