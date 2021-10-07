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
#include "g_000_EatingTheFood.h"
#include "g_001_ExploringTheTouchpads.h"
#include "g_002_EngagingConsistently.h"
#include "g_003_AvoidingUnlitTouchpads.h"
#include "g_004_LearningTheLights.h"
#include "g_005_MasteringTheLights.h"
#include "g_006_RespondingQuickly.h"
#include "g_007_LearningBrightness.h"
#include "g_008_LearningDoubleSequences.h"
#include "g_009_LearningLongerSequences.h"
#include "g_010_MatchingTwoColors.h"
#include "g_011_MatchingMoreColors.h"

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
int EVER_STORED_ADDRESS = 10;  // what are valid addresses?
int GAME_ADDRESS = 20;  // what are valid addresses?

// setup() runs once, when the device is first turned on.
void setup() {

  // Put initialization like pinMode and begin functions here.

  // TODO read GAME_TO_PLAY from eeprom!
  // what if no GAME_TO_PLAY ever written to eeprom?  ??????????????????????????????????????
    

    bool ever_stored = false;  // have we ever stored a game to eeprom? check value against "checksum"
    uint16_t ever_stored_check;
    EEPROM.get(EVER_STORED_ADDRESS, ever_stored_check);

    if (ever_stored_check == 12345)
    {
        ever_stored = true;
    }

    if (ever_stored)
    {
        uint16_t game_val;
        EEPROM.get(GAME_ADDRESS, game_val);
        GAME_TO_PLAY = game_val;
        NEXT_GAME_TO_PLAY = game_val;
    }
    else
    {
        GAME_TO_PLAY = 0;
        NEXT_GAME_TO_PLAY = 0;

        uint16_t value = 12345;
        EEPROM.put(EVER_STORED_ADDRESS, value);
        value = GAME_TO_PLAY;
        EEPROM.put(GAME_ADDRESS, value);
    }

    // TODO upon game switch, need to also write to eeprom!

    hub.Initialize("game_ID_here_TODO");
    // in games it was: hub.Initialize(__FILE__);

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
        int new_game_selected = mgschwan_serve_webinterface(GAME_TO_PLAY, NEXT_GAME_TO_PLAY);

        if (new_game_selected >= 0 && new_game_selected != GAME_TO_PLAY)
        {
            // TODO
            // write new game to eeprom
            Log.info("New game selected %i", new_game_selected);
            NEXT_GAME_TO_PLAY = new_game_selected;
            
            // why set it here? edge case: user selects game, then hub resets / turns off before trial completes. still starts on most recently selected game in this case.
            uint16_t value = NEXT_GAME_TO_PLAY;
            EEPROM.put(GAME_ADDRESS, value);
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
    else if (GAME_TO_PLAY == 3)
    {
        trial_done = AvoidingUnlitTouchpads_Loop();
    }
    else if (GAME_TO_PLAY == 4)
    {
        trial_done = LearningTheLights_Loop();
    }
    else if (GAME_TO_PLAY == 5)
    {
        trial_done = MasteringTheLights_Loop();
    }
    else if (GAME_TO_PLAY == 6)
    {
        trial_done = RespondingQuickly_Loop();
    }
    else if (GAME_TO_PLAY == 7)
    {
        trial_done = LearningBrightness_Loop();
    }
    else if (GAME_TO_PLAY == 8)
    {
        trial_done = LearningDoubleSequences_Loop();
    }
    else if (GAME_TO_PLAY == 9)
    {
        trial_done = LearningLongerSequences_Loop();
    }
    else if (GAME_TO_PLAY == 10)
    {
        trial_done = MatchingTwoColors_Loop();
    }
    else if (GAME_TO_PLAY == 11)
    {
        trial_done = MatchingMoreColors_Loop();
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