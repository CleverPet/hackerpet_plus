#include "game-manager.h"



GameManager::GameManager(HubInterface * hub)
{
    _hub = hub;

    // TODO should this be saved / loaded from eeprom?
    // TODO must be reset at midnight! But this class has no concept of time!
    //_kibbles_eaten_today = 0;

}

bool GameManager::Initialize(int init_game)
{
    // will be overwritten by config manager Initialize:
    _current_game = init_game;
    _next_game = init_game;

    _game_enabled = false;
    
    _kibbles_eaten = 0;

    _trial_info = new trial_info;
    _trial_info->food_eaten = false;
    
    _trial_just_done = true;

    return true;
}

int GameManager::get_kibbles_eaten()
{
    return _kibbles_eaten;
}

bool GameManager::reset_kibbles_eaten()
{
    _kibbles_eaten = 0;
    return true;
}

bool GameManager::set_next_game(int next_game)
{
    _next_game = next_game;
    return true;
}

int GameManager::get_current_game()
{
    return _current_game;
}

bool GameManager::set_game_enabled(bool enable_game)
{
    _game_enabled = enable_game;
    return true;
}

bool GameManager::trial_just_done()
{
    return _trial_just_done;
}

bool GameManager::Run()
{
    if (_game_enabled)
    {
        Serial.println("    [[GameManager::Run()]]: 1!");
        bool trial_done = false;

        // TODO simplify this / move to a function when cleaning up

        // NOTE: only info set AFTER any yield, i.e. from latest return, will be returned!

        if (_current_game == 0)
        {
            trial_done = EatingTheFood_Loop(_hub, _trial_info);
        }
        else if (_current_game == 1)
        {
            trial_done = ExploringTheTouchpads_Loop(_hub, _trial_info);
        }
        else if (_current_game == 2)
        {
            trial_done = EngagingConsistently_Loop(_hub, _trial_info);
        }
        else if (_current_game == 3)
        {
            trial_done = AvoidingUnlitTouchpads_Loop(_hub, _trial_info);
        }
        else if (_current_game == 4)
        {
            trial_done = LearningTheLights_Loop(_hub, _trial_info);
        }
        else if (_current_game == 5)
        {
            trial_done = MasteringTheLights_Loop(_hub, _trial_info);
        }
        else if (_current_game == 6)
        {
            trial_done = RespondingQuickly_Loop(_hub, _trial_info);
        }
        else if (_current_game == 7)
        {
            trial_done = LearningBrightness_Loop(_hub, _trial_info);
        }
        else if (_current_game == 8)
        {
            trial_done = LearningDoubleSequences_Loop(_hub, _trial_info);
        }
        else if (_current_game == 9)
        {
            trial_done = LearningLongerSequences_Loop(_hub, _trial_info);
        }
        else if (_current_game == 10)
        {
            trial_done = MatchingTwoColors_Loop(_hub, _trial_info);
        }
        else if (_current_game == 11)
        {
            trial_done = MatchingMoreColors_Loop(_hub, _trial_info);
        }
        else
        {
            Log.error("Invalid game selected!");
        }
        
        if (trial_done)
        {
           if(_trial_info->food_eaten)
           {
               _kibbles_eaten++;
           }
           _trial_info->food_eaten = false;
        }

        if (trial_done && (_next_game != _current_game))
        {
            _current_game = _next_game;
        }

        _trial_just_done = trial_done;
        Serial.println("    [[GameManager::Run()]]: 2!");
    }
    else
    {
        _trial_just_done = true;  // if no game enabled; say trial done true
    }
    
    return true;
}