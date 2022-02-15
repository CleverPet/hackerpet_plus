
#include "config-manager.h"

#include <Particle.h>

using namespace std;

#include <MDNS.h>
using namespace mdns;





ConfigManager::ConfigManager(HubInterface * hub, GameManager * gameMan)
{
    _hub = hub;
    _gameMan = gameMan;
    _htmlMan = new HtmlManager();
}

bool ConfigManager::_sched_char_to_string(char * char_tmp, String & str)
{
            
    String str2(char_tmp);
    str = str2;
    return true;
}

bool ConfigManager::_sched_string_to_char(char * char_tmp, String & str)
{
    for (int k = 0; k < 5; k++)
    {
        char_tmp[k] = str[k];
    }
    return true;
}

bool ConfigManager::Initialize()
{
    _system_ready = false;

    bool ever_stored = false;  // have we ever stored a game to eeprom? check value against "checksum"
    uint16_t ever_stored_check;
    EEPROM.get(_EVER_STORED_EEP_ADDRESS, ever_stored_check);

    if (ever_stored_check == _EVER_STORED_CHECK_VALUE)
    {
        ever_stored = true;
    }

    if (ever_stored)
    {
        EEPROM.get(_GAME_EEP_ADDRESS, _game_to_play);
        _next_game_to_play = _game_to_play;
        _gameMan->Initialize(_next_game_to_play);
        //_gameMan->set_next_game(_next_game_to_play);

        EEPROM.get(_TIME_ZONE_EEP_ADDRESS, _time_zone_offset);
        Time.zone(_time_zone_offset);
        
        EEPROM.get(_DST_EEP_ADDRESS, _dst_on);
        if (_dst_on)
        {
            Time.beginDST();
        }

        EEPROM.get(_HUB_MODE_EEP_ADDRESS, _hub_mode);

        char char_tmp[6];

        EEPROM.get(_SCHED_WEEKDAY_FROM_ADDRESS, char_tmp);
        Log.info("char_tmp 0:");
        Log.info(char_tmp);
        _sched_char_to_string(char_tmp, _weekday_from);

        EEPROM.get(_SCHED_WEEKDAY_TO_ADDRESS, char_tmp);
        Log.info("char_tmp 1:");
        Log.info(char_tmp);
        _sched_char_to_string(char_tmp, _weekday_to);

        EEPROM.get(_SCHED_WEEKEND_FROM_ADDRESS, char_tmp);
        Log.info("char_tmp 2:");
        Log.info(char_tmp);
        _sched_char_to_string(char_tmp, _weekend_from);

        EEPROM.get(_SCHED_WEEKEND_TO_ADDRESS, char_tmp);
        Log.info("char_tmp 3:");
        Log.info(char_tmp);
        _sched_char_to_string(char_tmp, _weekend_to);

        _last_hub_mode = _hub_mode;

    }
    else
    {
        // default values for a "new" hub:
        _game_to_play = 0;
        _next_game_to_play = 0;

        _time_zone_offset = 0.0;  // TIME_ZONE_OFFSET = 0.0;
        _dst_on = false;  // DST_ON = false;
        _hub_mode = _HUB_MODE_STAY_ON;  // HUB_MODE = HUB_MODE_STAY_ON;
  
        EEPROM.put(_GAME_EEP_ADDRESS, _next_game_to_play);
        _gameMan->Initialize(_next_game_to_play);
        //_gameMan->set_next_game(_next_game_to_play);

        EEPROM.put(_TIME_ZONE_EEP_ADDRESS, _time_zone_offset);
        Time.zone(_time_zone_offset);

        EEPROM.put(_DST_EEP_ADDRESS, _dst_on);

        EEPROM.put(_HUB_MODE_EEP_ADDRESS, _hub_mode);

        EEPROM.put(_EVER_STORED_EEP_ADDRESS, _EVER_STORED_CHECK_VALUE);

        _weekday_from = "09:00";
        _weekday_to = "16:00";

        _weekend_from = "11:00";
        _weekend_to = "15:00";

        char char_tmp[6];
        char_tmp[5] = 0;

        _sched_string_to_char(char_tmp, _weekday_from);
        EEPROM.put(_SCHED_WEEKDAY_FROM_ADDRESS, char_tmp);
        
        _sched_string_to_char(char_tmp, _weekday_to);
        EEPROM.put(_SCHED_WEEKDAY_TO_ADDRESS, char_tmp);

        _sched_string_to_char(char_tmp, _weekend_from);
        EEPROM.put(_SCHED_WEEKEND_FROM_ADDRESS, char_tmp);

        _sched_string_to_char(char_tmp, _weekend_to);
        EEPROM.put(_SCHED_WEEKEND_TO_ADDRESS, char_tmp);

        _last_hub_mode = _hub_mode;
    }

    return true;

}

bool ConfigManager::Run()
{

    // interface
    //  of config / parameters from config manager above, to game manager and hub below
    //  also other way around, from hub to config manager for webpage display

    // TODO: set Hub mode based on _hub_mode !!! use _hub


    if (WiFi.ready() && _system_ready == false)
    {
        _broadcastAddress = mgschwan_getBroadcastAddress();
        _system_ready = true;
        mgschwan_setupNetwork(); //Open TCP Port
        Log.info("Wifi Ready. Ip Address %s",  WiFi.localIP());
    }
    else {
        //Waiting for the Wifi to become ready        
    }
    
    if (_system_ready) 
    {
        mgschwan_MDNS_loop();

        _display_error_msg = "<b> Your hub is functioning normally.</b>";
        if (_hub->IsHubOutOfFood())
        {
            _display_error_msg = "<b> Your hub is out of food. Please place food in the singulator and remove and replace dome.</b>";
        }
        if (_hub->IsPlatterStuck() || _hub->IsPlatterError())
        {
            _display_error_msg = "<b> Your hub's platter is jammed. Please remove dome and clear the obstruction, then replace dome.</b>";
        }
        if (_hub->IsSingulatorError())
        {
            _display_error_msg = "<b> Your hub's singulator is jammed. Please remove dome and clear the obstruction, then replace dome.</b>";
        }
        if (_hub->IsDomeRemoved())
        {
            _display_error_msg = "<b> Your hub's dome is removed.</b>";
        }

        // get current game from gameMan
        _game_to_play = _gameMan->get_current_game();
        _new_game_selected = _game_to_play;

        _serve_webinterface();

        if (_new_game_selected >= 0 && _new_game_selected != _game_to_play)
        {
            Log.info("New game selected %i", _new_game_selected);
            _next_game_to_play = _new_game_selected;
            
            // why set it here? edge case: user selects game, then hub resets / turns off before trial completes. still starts on most recently selected game in this case.
            EEPROM.put(_GAME_EEP_ADDRESS, _next_game_to_play);

            _gameMan->set_next_game(_next_game_to_play);

        }
        

    }

    // TODO should _process_hub_mode be outside system_ready? things should function in general even if not on wifi???
    // also set to active by default?
    // TODO it already is by default but should set back to HUB STAY ON mode if can't connect to wifi??? or at least not in HUB STAY OFF mode???
    // also... we might need to put this in its own class at some point

    _process_hub_mode();

    return true;

}


bool ConfigManager::_process_hub_mode()
{
        
    // compare _hub_mode vs _last_hub_mode (set at the end of this function)

    if (_hub_mode != _last_hub_mode)
    {
        // there may not be anything to set here ...
    }

    // determine hub state: active vs. standby, based on mode (and schedule, if mode is scheduler)
    
    int new_hub_state = -1;

    if (_hub_mode == _HUB_MODE_STAY_ON)
    {
        new_hub_state = _HUB_STATE_ACTIVE;
    }
    else if (_hub_mode == _HUB_MODE_STAY_OFF)
    {
        new_hub_state = _HUB_STATE_STANDBY;
    }
    else if (_hub_mode == _HUB_MODE_SCHEDULED)
    {
        // use:

        // _weekday_from: 06:30
        // _weekday_to
        // _weekend_from
        // _weekend_to

        // get:
        //  current time
        //  current day of the week -> weekday or weekend
        
        int hour_now = Time.hour();
        int minute_now = Time.minute();
        int weekday_now = Time.weekday();

        String from_hour = "  ";
        String from_minute = "  ";
        String to_hour = "  ";
        String to_minute = "  ";

        if (weekday_now == 1 || weekday_now == 7)  // it is a weekend
        {
            from_hour[0] = _weekend_from[0];
            from_hour[1] = _weekend_from[1];

            from_minute[0] = _weekend_from[3];
            from_minute[1] = _weekend_from[4];

            to_hour[0] = _weekend_to[0];
            to_hour[1] = _weekend_to[1];

            to_minute[0] = _weekend_to[3];
            to_minute[1] = _weekend_to[4];
        }
        else  // it is a weekday
        {
            from_hour[0] = _weekday_from[0];
            from_hour[1] = _weekday_from[1];

            from_minute[0] = _weekday_from[3];
            from_minute[1] = _weekday_from[4];

            to_hour[0] = _weekday_to[0];
            to_hour[1] = _weekday_to[1];

            to_minute[0] = _weekday_to[3];
            to_minute[1] = _weekday_to[4];
        }


        int day_minutes_now = hour_now * 60 + minute_now;

        int day_minutes_from = from_hour.toInt() * 60 + from_minute.toInt();
        int day_minutes_to = to_hour.toInt() * 60 + to_minute.toInt();

        // figure out hub state based on time and schedule

        if (day_minutes_to > day_minutes_from)
        {
            if (day_minutes_from < day_minutes_now && day_minutes_now < day_minutes_to)
            {
                // hub on
                new_hub_state = _HUB_STATE_ACTIVE;
            }
            else
            {
                // hub off
                new_hub_state = _HUB_STATE_STANDBY;
            }
        }
        else if (day_minutes_from > day_minutes_to)
        {
            if (day_minutes_from < day_minutes_now && day_minutes_now < day_minutes_to)
            {
                // hub off
                new_hub_state = _HUB_STATE_STANDBY;
            }
            else
            {
                // hub on
                new_hub_state = _HUB_STATE_ACTIVE;
            }
        }
        else
        {
            // from and to are equal, keep hub off!
            new_hub_state = _HUB_STATE_STANDBY;
        }

    }
    else
    {
        // invalid hub mode!
        Log.info("ERROR: Invalid hub mode! Keeping hub on!");
        _hub_mode = _HUB_MODE_STAY_ON;
        new_hub_state = _HUB_STATE_ACTIVE;
    }

    if (new_hub_state != _hub_state)
    {
        // hub state has changed!

        _hub_state = new_hub_state;


        if (_hub_state == _HUB_STATE_ACTIVE)
        {   

            // for now, we are going to ignore the indicator light
                
            //     _logger->Log("SI::OnDesiredStatus: setting _activity_state = ACTIVITY_STATE_ACTIVE", Logger::LOG_LEVEL_LIGHT_CONTEXT);
            //     IndicatorState = IL_SI_ACTIVE;


            // this just set _active_mode flag in old firmware and that's all...
            // _dli->SetActiveMode(true);
            
            _hub->SetButtonAudioEnabled(true);
            _hub->SetLightEnabled(true);
            _hub->UpdateButtonAudioEnabled();

            // inform game manager of hub state
            // technically, only need to do this if it changed.
            // should we just have it skip the game loop?
            _gameMan->set_game_enabled(true);

        }
        else if (_hub_state == _HUB_STATE_STANDBY)
        {
            // for now, we are going to ignore the indicator light

        //        _logger->Log("SI::OnDesiredStatus: setting _activity_state = ACTIVITY_STATE_STANDBY", Logger::LOG_LEVEL_LIGHT_CONTEXT);
         //        if (_max_kibbles_light_on)
        //        {
        //            IndicatorState = IL_SI_MAX_KIBBLES_DEPLETED;
        //        }
        //        else
        //        {
        //            IndicatorState = IL_SI_STANDBY;   
        //        }
            // 
            
            // this just set _active_mode flag in old firmware and that's all...
            // _dli->SetActiveMode(false);
            
            _hub->SetButtonAudioEnabled(false);
            _hub->SetLightEnabled(false);
            _hub->UpdateButtonAudioEnabled();

            _gameMan->set_game_enabled(false);
            
        }
        else
        {
            Log.info("ERROR invalid hub state!");
        }


    }

    _last_hub_mode = _hub_mode;

    return true;

}

bool ConfigManager::_serve_webinterface()
{
    //int new_game_selected = -1;
    //int overrideable_next_game = _next_game_to_play;

    _webclient = _webserver.available();
    bool request_finished = false;
    if (_webclient.connected()) 
    {
        String request_string = "";
        _read_from_client(request_finished, request_string);

        if (request_finished)
        {
            _process_request(request_string);
        }
    }

    delay (1); //That is a hack to allow the browser to receive the data
    _webclient.stop();
    return true;
}

bool ConfigManager::_read_from_client(bool & request_finished, String & response_str)
{
    int c = 0, last_c = 0, last_last_c = 0;

    request_finished = false;

    while (_webclient.available())
    {
        c = _webclient.read();
        last_last_c = last_c;
        last_c = c;

        response_str += char(c);

        // Request finished.
        if (c == '\n' && (c == last_c || c == last_last_c) )
        {
            request_finished = true;
        }
    }

    return true;
}


bool ConfigManager::_process_request(String req_str)
{

    // different types of requests
    
    if (req_str.indexOf("local-api") > -1)
    {
        _process_api_req(req_str);
    }
    else
    {
        bool req_get = req_str.substring(0, 3).equalsIgnoreCase("GET");
        bool req_post = req_str.substring(0, 4).equalsIgnoreCase("POST");

        if (req_get)
        {
            Log.info("!!! SERVER IS SERVING GET REQUEST !!!");
            _process_get_req(req_str);
        }
        else if (req_post)
        {
            Log.info("!!! SERVER IS SERVING POST REQUEST !!!");
            _process_post_req(req_str);
        }
        else
        {
            Log.error("!!! SERVER IS SERVING UNKNOWN REQUEST !!!");
        }

        _write_response_html();

    }
    return true;
}

bool ConfigManager::_process_api_req(String req_str)
{
    // this is an API request (asynchronous webpage update)

    bool req_get = req_str.substring(0, 3).equalsIgnoreCase("GET");
    bool req_post = req_str.substring(0, 4).equalsIgnoreCase("POST");
    
    if (req_get)
    {
        Log.info("--- SERVER IS SERVING API GET REQUEST ---");
        //Log.info("API request string:");
        //Log.print(req_str);
        
        String next_game_str = int_to_string(_next_game_to_play);
        String current_game_str = int_to_string(_game_to_play);

        String return_str = "{"
                            "\"status\":\"" + _display_error_msg + "\","
                            "\"game_id_queued\":\"" + next_game_str + "\","
                            "\"game_id_playing\":\"" + current_game_str + "\","
                            "\"time\":\"" + Time.timeStr() + "\""
                            "}";
        _webclient.println(return_str);
    }
    else if (req_post)
    {
        Log.info("!!! SERVER IS SERVING API POST REQUEST !!!");
        Log.info("API request string:");
        Log.print(req_str);
        
        // weekday_from=14:22&weekday_to=14:24&weekend_from=14:23&weekend_to=06:21
        _weekday_from = req_str.substring(req_str.indexOf("weekday_from=") + 13).substring(0, 5);
        _weekday_to = req_str.substring(req_str.indexOf("weekday_to=") + 11).substring(0, 5);
        _weekend_from = req_str.substring(req_str.indexOf("weekend_from=") + 13).substring(0, 5);
        _weekend_to = req_str.substring(req_str.indexOf("weekend_to=") + 11).substring(0, 5);

        char char_tmp[6];
        char_tmp[5] = 0;

        _sched_string_to_char(char_tmp, _weekday_from);
        EEPROM.put(_SCHED_WEEKDAY_FROM_ADDRESS, char_tmp);
        
        _sched_string_to_char(char_tmp, _weekday_to);
        EEPROM.put(_SCHED_WEEKDAY_TO_ADDRESS, char_tmp);

        _sched_string_to_char(char_tmp, _weekend_from);
        EEPROM.put(_SCHED_WEEKEND_FROM_ADDRESS, char_tmp);

        _sched_string_to_char(char_tmp, _weekend_to);
        EEPROM.put(_SCHED_WEEKEND_TO_ADDRESS, char_tmp);

        String return_str = "{}";
        _webclient.println(return_str);
    }
    else
    {
 
        Log.error("!!! SERVER IS SERVING UNKNOWN **API** REQUEST !!!");
    }

    return true;
}

bool ConfigManager::_process_get_req(String req_str)
{
    // currently nothing specific in the request to process for GET
    // assume all GET requests route to cleverpet.local
    return true;
}

bool ConfigManager::_process_post_req(String req_str)
{
    // what kind of request was it?
    // we assume only one of these will be processed on any given call (per request)
    
    int timezone_html_index = req_str.indexOf("select_timezone=");
    int game_html_index = req_str.indexOf("game=");
    int dst_html_index = req_str.indexOf("select_dst=");
    int hub_mode_html_index = req_str.indexOf("hub_mode=");   
    
    if (dst_html_index > -1)
    {
        _process_dst_req(req_str, dst_html_index);
    }
    else if (timezone_html_index > -1)
    {
        _process_timezone_req(req_str, timezone_html_index);
    }
    else if (hub_mode_html_index > -1)
    {
        _process_hub_mode_req(req_str, hub_mode_html_index);
    }
    else if (game_html_index > -1)
    {
        _process_game_select_req(req_str, game_html_index);
    }

    return true;
}

bool ConfigManager::_process_dst_req(String req_str, int dst_html_index)
{
    Log.info("This is a DST post request.");

    String tmp_2 = req_str.substring(dst_html_index + 11);
    //tmp_2 = tmp_2.substring(0, tmp_2.indexOf("&"));
    Log.print(tmp_2 + "\n");
    
    _dst_on = bool(tmp_2.toInt());
    if(_dst_on)
    {
        Time.beginDST();
    }
    else
    {
        Time.endDST();
    }
    EEPROM.put(_DST_EEP_ADDRESS, _dst_on);
    return true;
}

bool ConfigManager::_process_timezone_req(String req_str, int timezone_html_index)
{
    Log.info("This is a TIMEZONE post request.");

    String tmp_2 = req_str.substring(timezone_html_index + 16);
    //tmp_2 = tmp_2.substring(0, tmp_2.indexOf("&"));
    Log.print(tmp_2 + "\n");
    
    float tz_offset = tmp_2.toFloat();
    Time.zone(tz_offset);              
    _time_zone_offset = tz_offset;
    EEPROM.put(_TIME_ZONE_EEP_ADDRESS, _time_zone_offset);
    
    return true;
}


bool ConfigManager::_process_hub_mode_req(String req_str, int hub_mode_html_index)
{
    Log.info("This is a HUB MODE / SCHEDULER post request.");
    //Log.print("*******\n");
    //Log.print(thing + "\n");
    //Log.print("*-------------*\n");
    String tmp_2 = req_str.substring(hub_mode_html_index + 9);
    tmp_2 = tmp_2.substring(0, tmp_2.indexOf("&"));
    Log.print(tmp_2 + "\n");
    _hub_mode = tmp_2.toInt();
    EEPROM.put(_HUB_MODE_EEP_ADDRESS, _hub_mode);

    return true;

}

bool ConfigManager::_process_game_select_req(String req_str, int game_html_index)
{
    // !!!!!
    // TODO make if statements more efficient
    // !!!!!

    Log.info("This is a GAME post request.");

    String new_game_str = req_str.substring(game_html_index + 5, game_html_index + 6);
    String new_game_str_2 = req_str.substring(game_html_index + 5, game_html_index + 7);
    
    if (new_game_str_2.equalsIgnoreCase("10"))
    {
        _new_game_selected = 10;
        Log.info("POST: selected game 10!");
    }
    else if (new_game_str_2.equalsIgnoreCase("11"))
    {
        _new_game_selected = 11;
        Log.info("POST: selected game 11!");
    }
    else if (new_game_str.equalsIgnoreCase("0"))
    {
        _new_game_selected = 0;
        Log.info("POST: selected game 0!");
    }
    else if (new_game_str.equalsIgnoreCase("1"))
    {
        _new_game_selected = 1;
        Log.info("POST: selected game 1!");          
    }
    else if (new_game_str.equalsIgnoreCase("2"))
    {
        _new_game_selected = 2;
        Log.info("POST: selected game 2!");
    }
    else if (new_game_str.equalsIgnoreCase("3"))
    {
        _new_game_selected = 3;
        Log.info("POST: selected game 3!");
    }
    else if (new_game_str.equalsIgnoreCase("4"))
    {
        _new_game_selected = 4;
        Log.info("POST: selected game 4!");
    }
    else if (new_game_str.equalsIgnoreCase("5"))
    {
        _new_game_selected = 5;
        Log.info("POST: selected game 5!");
    }
    else if (new_game_str.equalsIgnoreCase("6"))
    {
        _new_game_selected = 6;
        Log.info("POST: selected game 6!");
    }
    else if (new_game_str.equalsIgnoreCase("7"))
    {
        _new_game_selected = 7;
        Log.info("POST: selected game 7!");
    }
    else if (new_game_str.equalsIgnoreCase("8"))
    {
        _new_game_selected = 8;
        Log.info("POST: selected game 8!");
    }
    else if (new_game_str.equalsIgnoreCase("9"))
    {
        _new_game_selected = 9;
        Log.info("POST: selected game 9!");
    }


    return true;

}




bool ConfigManager::_write_response_html()
{
    String content = "";
    content += "<!DOCTYPE html>\n";
    content += "<html>\n";
    content += "<head>\n<style>\n";
    content += _htmlMan->get_css_string();
    content += "</style>\n";
    content += _htmlMan->get_script_html();
    content += "</head>\n";
    content += "<body>\n";

    // enable for debugging full GET request:
    //content += "<br><br>";
    //content += thing;
    //content += "<br><br>";

    // return the id from this function at the end; or, return what? -1? to indicate no new choice?

    // print list of games and URL to go to
    
    content += "<br>\n";
    content += "select game:<br><br>\n";

    int next_game_to_disp = _next_game_to_play;
    if (_new_game_selected > 0)
    {
        // main loop has not set this yet, if it was just selected; so we need this check
        next_game_to_disp = _new_game_selected;
    }

    content += _htmlMan->get_link_for_game(0, "0", "Eating the Food", _game_to_play, next_game_to_disp);
    content += _htmlMan->get_link_for_game(1, "1", "Exploring the Touchpads", _game_to_play, next_game_to_disp);
    content += _htmlMan->get_link_for_game(2, "2", "Engaging Consistently", _game_to_play, next_game_to_disp);
    content += _htmlMan->get_link_for_game(3, "3", "Avoiding Unlit Touchpads", _game_to_play, next_game_to_disp);
    content += _htmlMan->get_link_for_game(4, "4", "Learning the Lights", _game_to_play, next_game_to_disp);
    content += _htmlMan->get_link_for_game(5, "5", "Mastering the Lights", _game_to_play, next_game_to_disp);
    content += _htmlMan->get_link_for_game(6, "6", "Responding Quickly", _game_to_play, next_game_to_disp);
    content += _htmlMan->get_link_for_game(7, "7", "Learning Brightness", _game_to_play, next_game_to_disp);
    content += _htmlMan->get_link_for_game(8, "8", "Learning Double Sequences", _game_to_play, next_game_to_disp);
    content += _htmlMan->get_link_for_game(9, "9", "Learning Longer Sequences", _game_to_play, next_game_to_disp);
    content += _htmlMan->get_link_for_game(10, "10", "Matching Two Colors", _game_to_play, next_game_to_disp);
    content += _htmlMan->get_link_for_game(11, "11", "Matching More Colors", _game_to_play, next_game_to_disp);

    content += "<br>\n";
    //content += display_error_msg;
    //content += "<br>";
    content += _htmlMan->get_async_html();
    content += "<br>\n";
    
    //    "<form method=\"post\" action=\"http://cleverpet.local\">\n"
    //"Select Timezone: <select name=\"select_timezone\" onchange=\"this.form.submit()\">\n"
    
    String dst_option_1_sel = "";
    String dst_option_2_sel = "";
    if (_dst_on)
    {
        dst_option_1_sel = " selected";
    }
    else
    {
        dst_option_2_sel = " selected";
    }

    content += "<form method=\"post\" action=\"http://cleverpet.local\">\n"
                "Apply Daylight Savings: <select name=\"select_dst\" onchange=\"this.form.submit()\"><option value=\"1\"" + dst_option_1_sel + ">Yes</option><option value=\"0\"" + dst_option_2_sel + ">No</option></select><br>\n"
                "</form>\n";

    String time_zone_str = _htmlMan->get_time_zone_string(_time_zone_offset);
    Log.info("time zone str length: " + int_to_string(time_zone_str.length()));

    String content_2 = "";
    
    content_2 += "<br>\n";
    
    content_2 += "Current Date/Time:<br>\n";
    content_2 += "<b><strong class=\"api-msg\" id=\"api-time\">" + Time.timeStr() + "</strong><br />\n" + "</b>";

    content_2 += "<br>\n";
    
    String content_3 = "";
    
    // add scheduler to content_3

    Log.info("_weekday_from: <" + _weekday_from + ">");
    Log.info("_weekday_to: <" + _weekday_to + ">");
    Log.info("_weekend_from: <" + _weekend_from + ">");
    Log.info("_weekend_to: <" + _weekend_to + ">");

    content_3 += _htmlMan->get_scheduler_html(_hub_mode, _weekday_from, _weekday_to, _weekend_from, _weekend_to);

    content_3 += "</body>\n";
    content_3 += "</html>";
    //Log.info("content length: " + int_to_string(content.length()));
    //Log.info("content_2 length: " + int_to_string(content_2.length()));
    _webclient.println("HTTP/1.0 200 OK");
    _webclient.println("Content-type: text/html");
    _webclient.print("Content-length: ");
    _webclient.println(content.length() + time_zone_str.length() + content_2.length() + content_3.length());
    _webclient.println("");
    _webclient.print(content);
    _webclient.print(time_zone_str);
    _webclient.print(content_2);
    _webclient.print(content_3);
    _webclient.println();

    return true;
}

