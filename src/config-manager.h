#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "Particle.h"
#include <MDNS.h>
using namespace mdns;

#include "http-server-util.h"
#include "hackerpet.h"
#include "game-manager.h"
#include "html-manager.h"


class ConfigManager
{
    public:
        ConfigManager(HubInterface * hub, GameManager * gameMan);
        bool Initialize();
        bool Run();
    private:
        bool _sched_char_to_string(char * char_tmp, String & str);  // utility to convert char to string, for scheduler
        bool _sched_string_to_char(char * char_tmp, String & str);  // utility to convert string to char, for scheduler
        bool _process_hub_mode();  // set new hub mode (on/off/scheduler), if needed, and then set hub state (on/standby) based on scheduler if applicable
        bool _serve_webinterface();  // serves local http server
        bool _read_from_client(bool & request_finished, String & response_str);  // read a request
        bool _process_request(String req_str);  // process an http request
        bool _process_api_get_req(String req_str);  // process an API request from cleverpet.local
        bool _process_api_post_req(String req_str);  // post request
        bool _process_set_game_req(String req_str);  // a request to set a game to play
        bool _process_set_max_kibbles_req(String req_str);  // set max kibbles
        bool _process_set_kibbles_thresh_req(String req_str);  // set kibbles detection threshold
        bool _process_set_dst_req(String req_str);  // set daylight saving time on/off
        bool _process_set_timezone_req(String req_str);  // set the timezone
        bool _process_set_hub_mode_req(String req_str);  // set a new hub mode
        bool _process_set_schedule_req(String req_str); // set scheduler settings
        bool _process_get_req(String req_str);  // http get request for showing cleverpet.local webpage

        HubInterface * _hub;
        GameManager * _gameMan;

        // ***************** http server stuff *****************
        
        bool _system_ready;
        IPAddress _broadcastAddress;

        TCPServer _webserver = TCPServer(80);
        TCPClient _webclient;
        
        // ***************** const EEPROM addresses *****************
        
        const int _EVER_STORED_EEP_ADDRESS = 10;  
        const int _GAME_EEP_ADDRESS = 20;  
        const int _TIME_ZONE_EEP_ADDRESS = 30;  
        const int _DST_EEP_ADDRESS = 40; 
        const int _HUB_MODE_EEP_ADDRESS = 50; 
        const int _SCHED_WEEKDAY_FROM_ADDRESS = 60;
        const int _SCHED_WEEKDAY_TO_ADDRESS = 160;
        const int _SCHED_WEEKEND_FROM_ADDRESS = 260;
        const int _SCHED_WEEKEND_TO_ADDRESS = 360;
        const int _KIBBLES_LIMIT_ADDRESS = 460;
        
        // ***************** const other *****************

        // if this number does not match the number stored at EEPROM address _EVER_STORED_EEP_ADDRESS, eeprom will be "reset" to defaults
        // this number should be changed to avoid an undefined value at any new addresses added above
        const int _EVER_STORED_CHECK_VALUE = 12357;
        
        const int _HUB_MODE_STAY_OFF = 0;
        const int _HUB_MODE_STAY_ON = 1;
        const int _HUB_MODE_SCHEDULED = 2;
        
        const int _HUB_STATE_STANDBY = 0;
        const int _HUB_STATE_ACTIVE = 1;
        const int _HUB_STATE_INIT = 2;  // to force initialization

        // ***************** config vars *****************
    
        // these are variables set by the user via the cleverpet.local web page and local http server
    
        int _hub_mode;  // _HUB_MODE_STAY_OFF, _HUB_MODE_STAY_ON, _HUB_MODE_SCHEDULED
        int _last_hub_mode;

        int _hub_state; // _HUB_STATE_STANDBY, _HUB_STATE_ACTIVE, _HUB_STATE_INIT


        int _game_to_play;
        int _next_game_to_play; 
        int _new_game_selected; 

        float _time_zone_offset;  
        bool _dst_on; 
        
        String _weekday_from;
        String _weekday_to;
        String _weekend_from;
        String _weekend_to;

        String _display_error_msg;

        int _kibbles_limit;
        int _kibbles_eaten_today;

        int _last_day;
        
        // mdns reconnect variables
        unsigned long _last_mdns_reconnect_attempt;
        unsigned long _last_request_time;
        MDNS * mgschwan_mdns;
        unsigned long _last_mdns_loop_time;
};





#endif
