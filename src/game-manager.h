#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H


// games
#include "games/g_000_EatingTheFood.h"
#include "games/g_001_ExploringTheTouchpads.h"
#include "games/g_002_EngagingConsistently.h"
#include "games/g_003_AvoidingUnlitTouchpads.h"
#include "games/g_004_LearningTheLights.h"
#include "games/g_005_MasteringTheLights.h"
#include "games/g_006_RespondingQuickly.h"
#include "games/g_007_LearningBrightness.h"
#include "games/g_008_LearningDoubleSequences.h"
#include "games/g_009_LearningLongerSequences.h"
#include "games/g_010_MatchingTwoColors.h"
#include "games/g_011_MatchingMoreColors.h"


class GameManager
{
    public:
        GameManager(HubInterface * hub);
        bool Initialize(int init_game);
        bool Run();
        int get_current_game();
        bool set_next_game(int next_game);  // TODO should also set _current_game at init ! i.e. if it is not set yet!
        bool set_game_enabled(bool enable_game);
        int get_kibbles_eaten();
        bool reset_kibbles_eaten();
    private:
        HubInterface * _hub;
        int _current_game;
        int _next_game;
        bool _game_enabled;
        int _kibbles_eaten;
        trial_info *_trial_info;
};


#endif
