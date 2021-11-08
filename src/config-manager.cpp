
#include "config-manager.h"

// TODO ADD THIS FILE TO GIT

ConfigManager::ConfigManager(HubInterface * hub, GameManager * gameMan)
{
    _hub = hub;
    _gameMan = gameMan;
    
    
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
        
        String display_error_msg = "<b> Your hub is functioning normally.</b>";
        if (_hub->IsHubOutOfFood())
        {
            display_error_msg = "<b> Your hub is out of food. Please place food in the singulator and remove and replace dome.</b>";
        }
        if (_hub->IsPlatterStuck() || _hub->IsPlatterError())
        {
            display_error_msg = "<b> Your hub's platter is jammed. Please remove dome and clear the obstruction, then replace dome.</b>";
        }
        if (_hub->IsSingulatorError())
        {
            display_error_msg = "<b> Your hub's singulator is jammed. Please remove dome and clear the obstruction, then replace dome.</b>";
        }
        if (_hub->IsDomeRemoved())
        {
            display_error_msg = "<b> Your hub's dome is removed.</b>";
        }

        // get current game from gameMan
        _game_to_play = _gameMan->get_current_game();

        // TODO clean up this function!!!
        int new_game_selected = mgschwan_serve_webinterface(_game_to_play, _next_game_to_play, display_error_msg,
                                                            _time_zone_offset, _TIME_ZONE_EEP_ADDRESS, _dst_on, _DST_EEP_ADDRESS, 
                                                            _hub_mode, _HUB_MODE_EEP_ADDRESS);

        if (new_game_selected >= 0 && new_game_selected != _game_to_play)
        {
            Log.info("New game selected %i", new_game_selected);
            _next_game_to_play = new_game_selected;
            
            // why set it here? edge case: user selects game, then hub resets / turns off before trial completes. still starts on most recently selected game in this case.
            EEPROM.put(_GAME_EEP_ADDRESS, _next_game_to_play);

            _gameMan->set_next_game(_next_game_to_play);

        }

    }

    return true;

}
