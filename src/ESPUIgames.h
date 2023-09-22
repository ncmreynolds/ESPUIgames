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
#include <ESPUI.h>

class ESPUIgames	{

	public:
		enum class gameType: uint8_t {
		  simon = 0,
		  whackamole
		};
		ESPUIgames();														//Constructor function
		~ESPUIgames();														//Destructor function
		void addGameTab();														//Add ESPUIgame tab
		void debug(Stream &);												//Start debugging on a stream
		char* title();														//Returns a pointer to the title, for the sketch to do ESPUI.begin(game.title())
		void runFsm();														//Run the game, must be called continuously
		//Game settings
		void type(gameType t);
		void setTitle(const char* title);									//Set the title used on the page
		void setTabTitle(const char* title);								//Set the title used on the game tab, which you may want to be different
		void setLength(uint8_t length);										//Set the game length in turns
		void setWinContent(const char* label, const char* content);			//Set the content used in the win popup
		void setLoseContent(const char* label, const char* content);		//Set the content used in the lose popup
		//Help
		void setHelpTabTitle(const char* title);							//Set the title used on the help tab
		void setHelpContent(const char* label, const char* content);		//Set the content used in the help tab
		void addHelpTab();													//Add ESPUIgame help tab
		//Controls
		void enableStartSwitch(const char* label = nullptr);				//Add a 'game start' switch
		bool addPlayButton(char* title, char* label, ControlColor colour);
	protected:
	private:
		Stream *debug_uart_ = nullptr;										//The stream used for the debugging
		//Game basics
		//bool firstRun = true;												//Have to do some things in the first run of the FSM
		gameType typeOfGame = gameType::simon;								//The type of game
		char* gameTitle = nullptr;											//Game title on page
		uint8_t gameLength = 4;												//Length of game in turns, should that be important for the specific game
		uint8_t* gameChoices = nullptr;										//Series of game choices if that should be important for the specific game
		bool* buttonTriggered = nullptr;
		static constexpr uint8_t maximumNumberOfGameButtons = 16;			//Max number of game buttons
		ControlColor defaultGameColour = ControlColor::Wetasphalt;			//Default colour for most controls
		ControlColor highlightGameColour = ControlColor::Carrot;			//Highlight colour for most controls
		ControlColor wonGameColour = ControlColor::Emerald;					//Set all controls to this colour when a player wins
		ControlColor loseGameColour = ControlColor::Alizarin;				//Set all controls to this colour when a player loses
		static constexpr uint8_t numberOfGameColours = 8;					//Number of colours in usual palette
		ControlColor gameColourPalette[numberOfGameColours] = {				//Colours for randomly picking
			ControlColor::Alizarin /*Red*/,
			ControlColor::Emerald /*Green*/,
			ControlColor::Peterriver /*Blue*/,
			ControlColor::Sunflower /*Yellow*/,
			ControlColor::Carrot /*Orange*/,
			ControlColor::Wetasphalt /*Dark blue*/,
			ControlColor::Turquoise /*Turquoise*/,
			ControlColor::Dark /*Dark grey*/
			};
		//Game Finite state machine
		bool gameEnabled = true;
		bool gameStarted = false;
		bool gameLost = false;
		bool gameWon = false;
		bool computerTurn = true;
		uint8_t gamePosition = 0;											//How far in is the game, in turns
		uint16_t successLevel = 0;											//What is the current success level
		uint32_t gameTimer = 0;
		uint32_t gameDelay = 5000;
		uint32_t maxGameDelay = 5000;
		uint32_t gameSpeedupAmount = 250;
		uint32_t gameMoveTimeout = 15000;
		bool buttonLit = false;												//Is any game button lit?
		const uint32_t playButtonOnTime = 750;								//Time the play button is lit
		const uint32_t playButtonOffTime = 1000;							//Time between lighting play buttons in a sequence
		const uint32_t minimumOnTime = 250;
		uint32_t gameCheckInterval = 10000;
		uint32_t lastGameCheckTime = 0;
		uint8_t buttonPushed;
		uint8_t buttonReleased;
		void startNewGame();												//Start a new game
		void stopCurrentGame();												//Stop a running game
		void resetGame();													//Reset to go again
		void loseGame();													//Lose the game
		void lightEverything(ControlColor colour);
		void newMole();
		void whackMole(uint8_t index);
		void gameSpeedup();													//Speed up the game a little
		void gameSpeedReset();												//Reset the game to slightly less than the starting speed
		//Tabs
		uint16_t gameTabIDs[2];												//Storage for the game tab IDs
		char* gameTabLabels[2] = {nullptr, nullptr};
		//Win/lose
		uint16_t winWidgetId = 0;
		bool winWidgetVisible = false;
		char* gameWinLabel = nullptr;										//Label for the win box
		char* gameWinContent = nullptr;										//What's in the win box
		void hideWinWidget();
		void showWinWidget();
		uint16_t loseWidgetId = 0;
		bool loseWidgetVisible = false;
		char* gameLoseLabel = nullptr;										//Label for the lose box
		char* gameLoseContent = nullptr;									//What's in the lose box
		void hideLoseWidget();
		void showLoseWidget();
		//Help
		uint16_t helpLabelID = 0;
		char* gameHelpLabel = nullptr;										//Label for the help box
		char* gameHelpContent = nullptr;									//What's in the help box
		//Start switch
		bool startSwitchEnabled = false;									//Whether to add a start switch or not
		uint16_t startSwitchWidgetId = 0;									//ID for game switch
		char* startSwitchLabel;
		//Play buttons
		uint16_t gamePlayButtonIDs[maximumNumberOfGameButtons];
		uint8_t numberOfGamePlayButtons = 0;								//Number of game buttons
		uint8_t numberOfLitPlayButtons = 0;
		char* gamePlayButtonTitle[maximumNumberOfGameButtons];				//Button titles set at runtime
		char* gamePlayButtonLabel[maximumNumberOfGameButtons];				//Button labels set at runtime
		ControlColor gamePlayButtonColour[maximumNumberOfGameButtons];		//Button colours set at runtime
		void lightButton(uint8_t index);									//Light up a specific button
		void lightButton(uint8_t index, ControlColor colour);				//Light up a specific button a specific colour
		void extinguishButton(uint8_t index);								//Extinguish a specific button
		uint8_t buttonIndexFromId(uint16_t id);								//Find a play button index give the ESPUI control ID
		//Callback functions
		static void playButtonCallback(Control* sender, int type);			//Callback for play buttons
		static void startSwitchCallback(Control* sender, int value);		//Callback for the game start/end switch
		//String literals used for button labels, debug output etc
		static constexpr char* gameTabLabel0 PROGMEM = "Game";
		static constexpr char* gameTabLabel1 PROGMEM = "Help";
		static constexpr char* gameHelp0 PROGMEM = "Play back the sequence";
		static constexpr char* gameHelp1 PROGMEM = "Push the buttons as they light";
		static constexpr char* gameTypeDescriptionSimon PROGMEM = "Simon-says";
		static constexpr char* gameTypeDescriptionWhackAmole PROGMEM = "Whack-a-mole";
		static constexpr char* defaultStartSwitchLabel PROGMEM = "Start";
		static constexpr char* debugColonSpace PROGMEM = ": ";
		static constexpr char* debugControlColonSpace PROGMEM = "Control: ";
		static constexpr char* debugDashGt PROGMEM = "->";
		static constexpr char* debugOk PROGMEM = "OK";
		static constexpr char* debugFailed PROGMEM = "failed";
		static constexpr char* debugDown PROGMEM = "down";
		static constexpr char* debugUp PROGMEM = "up";
		static constexpr char* debugOn PROGMEM = "on";
		static constexpr char* debugOff PROGMEM = "off";
		static constexpr char* debugGameSpace PROGMEM = "Game ";
		static constexpr char* debugWon PROGMEM = "won";
		static constexpr char* debugTimedOut PROGMEM = "timed out";
		static constexpr char* debugReset PROGMEM = "reset";
		static constexpr char* debugStopped PROGMEM = "stopped";
		static constexpr char* debugLost PROGMEM = "lost";
		static constexpr char* debugPlayerSpace PROGMEM = "Player ";
		static constexpr char* debugComputerSpace PROGMEM = "Computer ";
		static constexpr char* debugTurn PROGMEM = "turn";
		static constexpr char* debugShowingSpace PROGMEM = "Showing ";
		static constexpr char* debugHidingSpace PROGMEM = "Hiding ";
		static constexpr char* debugWinSpace PROGMEM = "win ";
		static constexpr char* debugLoseSpace PROGMEM = "lose ";
		static constexpr char* debugWidget PROGMEM = "widget";
};
extern ESPUIgames game;	//Create an instance of the class, as only one is practically usable at a time
#endif
