#include "game-manager.h"



GameManager::GameManager(HubInterface * hub)
{
    
}

bool GameManager::Initialize(int init_game)
{
    // will be overwritten by config manager Initialize:
    _current_game = init_game;
    _next_game = init_game;

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

bool GameManager::Run()
{

    bool trial_done = false;
    
    // TODO simplify this / move to a function when cleaning up

    if (_current_game == 0)
    {
        trial_done = EatingTheFood_Loop(_hub);  // HubInterface * hub
    }
    else if (_current_game == 1)
    {
        trial_done = ExploringTheTouchpads_Loop(_hub);
    }
    else if (_current_game == 2)
    {
        trial_done = EngagingConsistently_Loop(_hub);
    }
    else if (_current_game == 3)
    {
        trial_done = AvoidingUnlitTouchpads_Loop(_hub);
    }
    else if (_current_game == 4)
    {
        trial_done = LearningTheLights_Loop(_hub);
    }
    else if (_current_game == 5)
    {
        trial_done = MasteringTheLights_Loop(_hub);
    }
    else if (_current_game == 6)
    {
        trial_done = RespondingQuickly_Loop(_hub);
    }
    else if (_current_game == 7)
    {
        trial_done = LearningBrightness_Loop(_hub);
    }
    else if (_current_game == 8)
    {
        trial_done = LearningDoubleSequences_Loop(_hub);
    }
    else if (_current_game == 9)
    {
        trial_done = LearningLongerSequences_Loop(_hub);
    }
    else if (_current_game == 10)
    {
        trial_done = MatchingTwoColors_Loop(_hub);
    }
    else if (_current_game == 11)
    {
        trial_done = MatchingMoreColors_Loop(_hub);
    }
    else
    {
        Log.error("Invalid game selected!");
    }
    
    if (trial_done && (_next_game != _current_game))
    {
        _current_game = _next_game;
    }

    return true;
}