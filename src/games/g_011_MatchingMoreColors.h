#ifndef G_MATCHINGMORECOLORS_H
#define G_MATCHINGMORECOLORS_H

/**
  Matching More Colors
  ====================

    This challenge is similar to Matching Two Colors, but now the pads can also
    turn white.

    By adding another color, the number of possible solutions grows
    exponentially! Adding a third color means there are over 24 initial
    combinations to solve, and many different paths to solving a given puzzle.

    **Challenge logic:** This interaction has 4 different levels with 4
    corresponding touches. There are 3 different colors: yellow, white and blue.

    At the start of the interaction a random combination of colors is chosen for
    the touchpads (starting state). Every iteration of the main while loop
    checks if the colors match and advances the color of the touched touchpad to
    the next one. If the player is about to run out of remaining touches, the
    'Do' sound is played with every touch as they are close to running out.

    When all touches are used, the interaction becomes a miss, if a match is
    found, the player gets presented with a foodtreat.

    If 4 successful games are played within the last 5 games the player will
    level up. On 3 missed games in the last 5 games, the player will level down.

  Authors: CleverPet Inc.
           Jelmer Tiete <jelmer@tiete.be>

  Copyright 2019
  Licensed under the AGPL 3.0
*/

#include "hackerpet.h"
#include "game_helper_functions.h"


/// The actual MatchingMoreColors challenge. This function needs to be called in a loop.
bool playMatchingMoreColors(HubInterface * hub, trial_info *_trial_info);


bool MatchingMoreColors_Loop(HubInterface * hub, trial_info *_trial_info);

// /**
//  * Setup function
//  * --------------
//  */
// void setup() {
//   // Initializes the hub and passes the current filename as ID for reporting
//   hub->Initialize(__FILE__);
// }

// /**
//  * Main loop function
//  * ------------------
//  */
// void loop()
// {
//     bool gameIsComplete = false;

//     // Advance the device layer state machine, but with 20 ms max time
//     // spent per loop cycle.
//     hub->Run(20);

//     // Play 1 interaction of the Matching More Colors challenge
//     gameIsComplete = playMatchingMoreColors();

//     if(gameIsComplete){
//         // Interaction end
//         return;
//     }

// }
#endif