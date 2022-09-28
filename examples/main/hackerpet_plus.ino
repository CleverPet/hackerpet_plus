/*
 * Project hackerpet_plus
 * Description:
 * Author:
 * Date:
 * VERSION:
 */


#include "Particle.h"
#include "softap_http.h"
#include "hotspot-http-server.h"

# include "../lib/MDNS/src/Buffer.h"
# include "../lib/MDNS/src/Buffer.cpp"
# include "../lib/MDNS/src/Record.h"
# include "../lib/MDNS/src/Record.cpp"
# include "../lib/MDNS/src/Label.h"
# include "../lib/MDNS/src/Label.cpp"
# include "../lib/MDNS/src/MDNS.h"
# include "../lib/MDNS/src/MDNS.cpp"

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
    
    // this is used by hackerpet for reports
    hub.Initialize("game_ID_here_TODO");
    // in games it was: hub.Initialize(__FILE__);
}

unsigned long lastmemcheck = 0;
unsigned long FREE_MEMORY;

// loop() runs over and over again, as quickly as it can execute.
void loop() {
    

    // serve webpage, read/write eeprom as config changes
    // Serial.println("[[calling]]: configMan.Run();");
    configMan.Run();
    
    // Serial.println("[[calling]]: hub.Run(20);");
    // run the hub
    // if testing on a particle photon by itself with no hub, comment this line to avoid seg fault
    hub.Run(20);

    // run the loop for the current active game
    // Serial.println("[[calling]]: gameMan.Run();");
    gameMan.Run();

    // every 10 seconds, print the free memory as a serial heartbeat    

    if ((millis() - lastmemcheck) > 10000) {        
        // Serial.print(Time.timeStr());
        // Serial.println("[[calling]]: free memory");
        
        FREE_MEMORY = System.freeMemory();

        lastmemcheck = millis();

        Serial.printlnf("\tMILLIS: %lu\tSYSTEM MEMORY=%lu", lastmemcheck, FREE_MEMORY);
    }
}
