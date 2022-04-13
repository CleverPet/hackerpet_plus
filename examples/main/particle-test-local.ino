/*
 * Project particle-test-local
 * Description:
 * Author:
 * Date:
 * VERSION:
 */


#include "Particle.h"
#include "softap_http.h"
#include "hotspot-http-server.h"

// new

#include "config-manager.h"
#include "game-manager.h"

// classes
HubInterface hub;
GameManager gameMan(&hub);    // which game to play given gameId from config
ConfigManager configMan(&hub, &gameMan);  // handles config of hub and game via webpage and eeprom

// enables simultaneous execution of application and system thread
SYSTEM_THREAD(ENABLED);

// hotspot
STARTUP(softap_set_application_page_handler(myPages, nullptr));

// setup() runs once, when the device is first turned on.
void setup() {

    configMan.Initialize();  // also initializes game manager
    hub.Initialize("game_ID_here_TODO");
    // in games it was: hub.Initialize(__FILE__);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {

    // serve webpage, read/write eeprom as config changes
    configMan.Run();
    
    // ************************************ DISABLE FOR TESTING WITHOUT HUB ************************************
    // run the hub
    hub.Run(20);
    // ************************************ ************************************ ************************************

    // run the loop for the current active game
    gameMan.Run();
}
