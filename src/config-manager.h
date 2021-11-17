#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "Particle.h"
#include "http-server-util.h"
#include <hackerpet.h>
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
        bool _serve_webinterface();
        bool _read_from_client(bool & request_finished, String & response_str);
        bool _process_request(String req_str);
        bool _process_api_req(String req_str);
        bool _process_get_req(String req_str);
        bool _process_post_req(String req_str);
        
        bool _process_dst_req(String req_str, int dst_index);
        bool _process_timezone_req(String req_str, int dst_index);
        bool _process_hub_mode_req(String req_str, int dst_index);
        bool _process_game_select_req(String req_str, int dst_index);

        bool _write_response_html();

        HubInterface * _hub;
        GameManager * _gameMan;
        HtmlManager * _htmlMan;

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
        
        // ***************** const other *****************

        // we can change this number to force an eeprom "reset" to defaults; and to avoid undefined state when adding new variables
        const int _EVER_STORED_CHECK_VALUE = 12347;  // EVER_STORED_CHECK_VALUE = 12346; 
        
        const int _HUB_MODE_STAY_OFF = 0;
        const int _HUB_MODE_STAY_ON = 1;
        const int _HUB_MODE_SCHEDULED = 2;
        
        // ***************** config vars *****************
        
        int _game_to_play;  // GAME_TO_PLAY;
        int _next_game_to_play;  // NEXT_GAME_TO_PLAY;
        int _new_game_selected; // selected (clicked) via web interface

        float _time_zone_offset;  // TIME_ZONE_OFFSET = 0.0;
        bool _dst_on;  // DST_ON = false;
        
        int _hub_mode;  // HUB_MODE = HUB_MODE_STAY_ON;

        String _display_error_msg;

};





#endif