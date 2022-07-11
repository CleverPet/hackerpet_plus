#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "Particle.h"
#include <MDNS.h>
using namespace mdns;

#include "http-server-util.h"
#include "hackerpet.h"
#include "game-manager.h"
#include "html-manager.h"


// TODO ADD THIS FILE TO GIT

// TODO also need to commit hackerpet changes separately!!!


class ConfigManager
{
    public:
        ConfigManager(HubInterface * hub, GameManager * gameMan);
        bool Initialize();
        bool Run();
    private:
        bool _sched_char_to_string(char * char_tmp, String & str);
        bool _sched_string_to_char(char * char_tmp, String & str);
        bool _process_hub_mode();
        bool _serve_webinterface();
        bool _read_from_client(bool & request_finished, String & response_str);
        bool _process_request(String req_str);
        bool _process_api_get_req(String req_str);
        bool _process_api_post_req(String req_str);
        bool _process_set_game_req(String req_str);  //, int dst_index);
        bool _process_set_max_kibbles_req(String req_str);
        bool _process_set_kibbles_thresh_req(String req_str);
        bool _process_set_dst_req(String req_str);  //, int dst_index);
        bool _process_set_timezone_req(String req_str);  // , int dst_index);
        bool _process_set_hub_mode_req(String req_str);  //, int dst_index);
        bool _process_set_schedule_req(String req_str); 
        bool _process_get_req(String req_str);

        HubInterface * _hub;
        GameManager * _gameMan;

        // ***************** http server stuff *****************
        
        bool _system_ready;
        IPAddress _broadcastAddress;

        TCPServer _webserver = TCPServer(80);
        TCPClient _webclient;
        
        // ***************** const EEPROM addresses *****************
        
        const int _EVER_STORED_EEP_ADDRESS = 10;  // EVER_STORED_ADDRESS = 10;
        const int _GAME_EEP_ADDRESS = 20;  // GAME_ADDRESS = 20;
        const int _TIME_ZONE_EEP_ADDRESS = 30;  // TIME_ZONE_ADDRESS = 30;
        const int _DST_EEP_ADDRESS = 40;  // DST_ADDRESS = 40;
        const int _HUB_MODE_EEP_ADDRESS = 50;  // HUB_MODE_ADDRESS = 50;
        const int _SCHED_WEEKDAY_FROM_ADDRESS = 60;
        const int _SCHED_WEEKDAY_TO_ADDRESS = 160;
        const int _SCHED_WEEKEND_FROM_ADDRESS = 260;
        const int _SCHED_WEEKEND_TO_ADDRESS = 360;
        const int _KIBBLES_LIMIT_ADDRESS = 460;
        
        // ***************** const other *****************

        // we can change this number to force an eeprom "reset" to defaults; and to avoid undefined state when adding new variables
        const int _EVER_STORED_CHECK_VALUE = 12357;  // EVER_STORED_CHECK_VALUE = 12346; 
        
        const int _HUB_MODE_STAY_OFF = 0;
        const int _HUB_MODE_STAY_ON = 1;
        const int _HUB_MODE_SCHEDULED = 2;
        
        const int _HUB_STATE_STANDBY = 0;
        const int _HUB_STATE_ACTIVE = 1;
        const int _HUB_STATE_INIT = 2;  // to force initialization

        // ***************** config vars *****************
        
        int _hub_mode;  // _HUB_MODE_STAY_OFF, _HUB_MODE_STAY_ON, _HUB_MODE_SCHEDULED
        int _last_hub_mode;

        int _hub_state; // _HUB_STATE_STANDBY, _HUB_STATE_ACTIVE


        int _game_to_play;  // GAME_TO_PLAY;
        int _next_game_to_play;  // NEXT_GAME_TO_PLAY;
        int _new_game_selected; // selected (clicked) via web interface

        float _time_zone_offset;  // TIME_ZONE_OFFSET = 0.0;
        bool _dst_on;  // DST_ON = false;
        
        String _weekday_from;
        String _weekday_to;
        String _weekend_from;
        String _weekend_to;

        String _display_error_msg;

        int _kibbles_limit;
        int _kibbles_eaten_today;

        int _last_day;

        unsigned long _last_mdns_reconnect_attempt;
        unsigned long _last_request_time;
        MDNS * mgschwan_mdns;
        unsigned long _last_mdns_loop_time;
};





#endif
