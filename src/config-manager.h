#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "Particle.h"
#include "http-server-util.h"
#include <hackerpet.h>
#include "game-manager.h"


// TODO ADD THIS FILE TO GIT

// TODO also need to commit hackerpet changes separately!!!


class ConfigManager
{
    public:
        ConfigManager(HubInterface * hub, GameManager * gameMan);
        bool Initialize();
        bool Run();
    private:
        HubInterface * _hub;
        GameManager * _gameMan;

        // ***************** http server stuff *****************
        
        bool _system_ready;
        IPAddress _broadcastAddress;

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

        float _time_zone_offset;  // TIME_ZONE_OFFSET = 0.0;
        bool _dst_on;  // DST_ON = false;
        
        int _hub_mode;  // HUB_MODE = HUB_MODE_STAY_ON;

};





#endif
