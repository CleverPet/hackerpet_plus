
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

        // TODO: move this to a function when cleaning up
        
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

        // TODO clean up this function!!!
        //int new_game_selected = mgschwan_serve_webinterface(_game_to_play, _next_game_to_play, display_error_msg,
        //                                                    _time_zone_offset, _TIME_ZONE_EEP_ADDRESS, _dst_on, _DST_EEP_ADDRESS, 
        //                                                    _hub_mode, _HUB_MODE_EEP_ADDRESS);

        if (_new_game_selected >= 0 && _new_game_selected != _game_to_play)
        {
            Log.info("New game selected %i", _new_game_selected);
            _next_game_to_play = _new_game_selected;
            
            // why set it here? edge case: user selects game, then hub resets / turns off before trial completes. still starts on most recently selected game in this case.
            EEPROM.put(_GAME_EEP_ADDRESS, _next_game_to_play);

            _gameMan->set_next_game(_next_game_to_play);

        }

    }

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
        
        // !!!!!!!!!!!!!!
        // TODO must also do the webclient.print part!!
        // !!!!!!!!!!!!!!

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

        // !!!!!!!!!!!!!!
        // TODO must also do the webclient.print part!!
        // !!!!!!!!!!!!!!
        
        _write_response_html();

    }
    return true;
}

bool ConfigManager::_process_api_req(String req_str)
{
    // this is an API request (asynchronous webpage update)
    Log.info("!!! SERVER IS SERVING API REQUEST !!!");
    Log.info("API request string:");
    Log.print(req_str);

    String next_game_str = int_to_string(_next_game_to_play);
    String current_game_str = int_to_string(_game_to_play);

    String return_str = "{"
                        "\"status\":\"" + _display_error_msg + "\","
                        "\"game_id_queued\":\"" + next_game_str + "\","
                        "\"game_id_playing\":\"" + current_game_str + "\","
                        "\"time\":\"" + Time.timeStr() + "\""
                        "}";
    _webclient.println(return_str);  // println?
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
    
    // TODO NEED TO return TIME_ZONE_OFFSET
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
    
    // TODO NEED TO return TIME_ZONE_OFFSET
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
        // TODO make if statements more efficient than in http-server-util.cpp
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

    // TODO DEBUG THIS FURTHER:
    String content_2 = "";
    
    content_2 += "<br>\n";
    
    content_2 += "Current Date/Time:<br>\n";
    content_2 += "<b><strong class=\"api-msg\" id=\"api-time\">" + Time.timeStr() + "</strong><br />\n" + "</b>";

    content_2 += "<br>\n";
    
    String content_3 = "";
    
    // add scheduler to content_3

    content_3 += _htmlMan->get_scheduler_html(_hub_mode);

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
