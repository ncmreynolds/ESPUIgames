/*
 * 
 * An example of a simple one-player simon-says game
 * 
 * Difficulty and labels on the buttons are left as default
 * 
 * The Wi-Fi connection/AP settings are done in the same manner as ESPUI
 * 
 */

#include <DNSServer.h>
#include <ESPUI.h>
#include <ESPUIgames.h>

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;

#if defined(ESP32)
#include <WiFi.h>
#else
// esp8266
#include <ESP8266WiFi.h>
#include <umm_malloc/umm_heap_select.h>
#ifndef MMU_IRAM_HEAP
#warning Try MMU option '2nd heap shared' in 'tools' IDE menu (cf. https://arduino-esp8266.readthedocs.io/en/latest/mmu.html#option-summary)
#warning use decorators: { HeapSelectIram doAllocationsInIRAM; ESPUI.addControl(...) ... } (cf. https://arduino-esp8266.readthedocs.io/en/latest/mmu.html#how-to-select-heap)
#warning then check http://<ip>/heap
#endif // MMU_IRAM_HEAP
#ifndef DEBUG_ESP_OOM
#error on ESP8266 and ESPUI, you must define OOM debug option when developping
#endif
#endif

const char* ssid = "ESPUIsimon-says";
const char* password = "espui";
const char* hostname = "espui";

void setup() {
    Serial.begin(115200);

#if defined(ESP32)
    WiFi.setHostname(hostname);
#else
    WiFi.hostname(hostname);
#endif
    // try to connect to existing network
    WiFi.begin(ssid, password);
    Serial.print("\n\nTry to connect to existing network");

    {
        uint8_t timeout = 10;

        // Wait for connection, 5s timeout
        do
        {
            delay(500);
            Serial.print(".");
            timeout--;
        } while (timeout && WiFi.status() != WL_CONNECTED);

        // not connected -> create hotspot
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.print("\n\nCreating hotspot");

            WiFi.mode(WIFI_AP);
            delay(100);
            WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
#if defined(ESP32)
            uint32_t chipid = 0;
            for (int i = 0; i < 17; i = i + 8)
            {
                chipid |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
            }
#else
            uint32_t chipid = ESP.getChipId();
#endif
            char ap_ssid[25];
            snprintf(ap_ssid, 26, "ESPUI-%08X", chipid);
            WiFi.softAP(/* SSID */ ap_ssid, /* pre-shared key */ NULL, /* channel */ 1, /* hidden */ 0, /* max connections */ 1, /* ftm_responder */ false);  //Limit the connection so only one player

            timeout = 5;

            do
            {
                delay(500);
                Serial.print(".");
                timeout--;
            } while (timeout);
        }
    }

    dnsServer.start(DNS_PORT, "*", apIP);

    Serial.println("\n\nWiFi parameters:");
    Serial.print("Mode: ");
    Serial.println(WiFi.getMode() == WIFI_AP ? "Station" : "Client");
    Serial.print("IP address: ");
    Serial.println(WiFi.getMode() == WIFI_AP ? WiFi.softAPIP() : WiFi.localIP());

  game.debug(Serial);
  game.type(ESPUIgames::gameType::simon);
  //Game tab
  game.setTabTitle("This is the game");
  game.setTitle("Simon says!");
  game.enableStartSwitch("Start the game");
  game.setWinContent("You have won!","Congratulations, you have won."); //Setting this makes a widget pop up if you win, not setting it relies on colours alone to show it
  game.setLoseContent("Sorry, you lose!","Flick the switch to try again"); //Setting this makes a widget pop up if you lose, not setting it relies on colours alone to show it
  game.addGameTab(); //Builds all the game controls
  //Help tab
  game.setHelpTabTitle("This is the help");
  game.setHelpContent("How to play this game","Toggle the 'start the game' switch to start the game.<br /><br />Repeat the sequence of buttons back on the screen as it happens, eventually you will win.<br /><br />When you win all the controls turn green.<br /><br />If you get something wrong, all the controls turn red.<br /><br />You can restart the game by flicking the 'Start the game' switch.");
  game.addHelpTab(); //Builds all the help controls
  ESPUI.begin(game.title());  //ESPUI is started from the sketch in case you want to add your own controls and pages as well before starting ESPUI
  //ESPUI.beginLITTLEFS(game.title());  //ESPUI is started from the sketch in case you want to add your own controls and pages as well before starting ESPUI
}

void loop() {
    dnsServer.processNextRequest();
    game.runFsm();  //Run the game finite state machine
}
