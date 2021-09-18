/*
 * Project particle-test-local
 * Description:
 * Author:
 * Date:
 */


#include "Particle.h"
#include "softap_http.h"
#include "http-server-util.h"

// games
#include "g_EatingTheFood.h"
#include "g_ExploringTheTouchpads.h"
#include "g_EngagingConsistently.h"

// enables simultaneous execution of application and system thread
SYSTEM_THREAD(ENABLED);


// This is for hosting http during listen mode
void myPages(const char* url, ResponseCallback* cb, void* cbArg, Reader* body, Writer* result, void* reserved)
{
  if (!strcmp(url, "/helloworld")) 
  {
       
      int addr = 10;
      uint16_t value = 12345;
      EEPROM.put(addr, value);
      
      // send the response code 200, the mime type "text/html"
      cb(cbArg, 0, 200, "text/html", nullptr);
      // send the page content
      result->write("<h2>hello world!</h2>");
      return;
  }
  if (!strcmp(url, "/index")) 
  {
      // send the response code 200, the mime type "text/html"
      cb(cbArg, 0, 200, "text/html", nullptr);
      // send the page content

      uint16_t objec;
      int address = 10;
      EEPROM.get(address, objec);

      Serial.printf("value: %d", int(objec));
      result->write("<h2>default!!!</h2>");
      return;
  }
  
  cb(cbArg, 0, 404, "text/plain", nullptr);
  return;
}

STARTUP(softap_set_application_page_handler(myPages, nullptr));



// ***************** http server stuff *****************
bool system_ready = false;
IPAddress broadcastAddress;
// ***************** end http server stuff *****************


long last_print = 0;
int GAME_TO_PLAY;
int NEXT_GAME_TO_PLAY;

// setup() runs once, when the device is first turned on.
void setup() {

  // Put initialization like pinMode and begin functions here.

  // TODO read GAME_TO_PLAY from eeprom!
  // what if no GAME_TO_PLAY ever written to eeprom?  ??????????????????????????????????????
  GAME_TO_PLAY = 2;
  NEXT_GAME_TO_PLAY = 2;

  hub.Initialize("game_ID_here_TODO");

}

// loop() runs over and over again, as quickly as it can execute.
void loop() {

    long int millis_start = millis();

    millis_start = millis();
    if (WiFi.ready() && system_ready == false)
    {
        broadcastAddress = mgschwan_getBroadcastAddress();
        system_ready = true;
        mgschwan_setupNetwork(); //Open TCP Port
        Log.info("Wifi Ready. Ip Address %s",  WiFi.localIP());
    }
    else {
        //Waiting for the Wifi to become ready        
    }
    
    long int millis_step_0 = millis() - millis_start;
    millis_start = millis();

    if (system_ready) 
    {
        mgschwan_MDNS_loop();
        int new_game_selected = mgschwan_serve_webinterface();

        if (new_game_selected >= 0 && new_game_selected != GAME_TO_PLAY)
        {
            // TODO
            // write new game to eeprom
            Log.info("New game selected %i", new_game_selected);
            NEXT_GAME_TO_PLAY = new_game_selected;
        }

    }

    long int millis_step_1 = millis() - millis_start;
    millis_start = millis();

    hub.Run(20);

    long int millis_step_2 = millis() - millis_start;
    millis_start = millis();
    
    bool trial_done = false;

    if (GAME_TO_PLAY == 0)
    {
        trial_done = EatingTheFood_Loop();
    }
    else if (GAME_TO_PLAY == 1)
    {
        trial_done = ExploringTheTouchpads_Loop();
    }
    else if (GAME_TO_PLAY == 2)
    {
        trial_done = EngagingConsistently_Loop();
    }
    else
    {
        Log.error("Invalid game selected!");
    }
    
    if (trial_done && (NEXT_GAME_TO_PLAY != GAME_TO_PLAY))
    {
        GAME_TO_PLAY = NEXT_GAME_TO_PLAY;
    }

    long int millis_step_3 = millis() - millis_start;
    millis_start = millis();

    if (millis() - last_print > 2000)
    {

        last_print = millis();
        Log.info("ran loop... %i", last_print);
        Log.info("    millis 0: %i", millis_step_0);
        Log.info("    millis 1: %i", millis_step_1);
        Log.info("    millis 2: %i", millis_step_2);
        Log.info("    millis 3: %i", millis_step_3);
    }

  
}