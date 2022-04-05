#ifndef G_MATCHINGTWOCOLORS_H
#define G_MATCHINGTWOCOLORS_H

/**
  Matching Two Colors
  ===================

    This is the eleventh challenge from the original CleverPet learning
    curriculum.  For the first time, the Hub introduces colors (blue and yellow
    - yes, dogs and cats can see color!) as a meaningful signal. Each time a
    touchpad is pressed, its color will change. Your player’s job is to make all
    the touchpads match within a limited number of overall presses.

    Up until now, there was always one correct answer about which pad to touch.
    This is the first challenge where each puzzle has more than one correct
    solution, and it showcases your player’s abilities solve problems creatively
    and in their own way.

    **Challenge logic:** This challenge has 4 levels. Every level has a
    corresponding maximum number of touches. The challenge uses two different
    colors, yellow and blue.

    At the start of an interaction every pad randomly gets one of the 2 colors.
    Every touch, the color of the touched touchpad advances to the next color.
    This all happens in a while loop, and with every iteration the state of the
    lights gets checked for a match of all 3 pads. If a match is found the while
    loop is exited and the player gets a foodtreat. Every touch decreases the
    maximum touches counter, if the touches are about to run out, the 'Do' sound
    will be played. When the remaining touches become zero, the interaction
    becomes a miss. On a miss the player's next interaction will be a retry
    interaction with the same starting state.

    If 4 successful games are played within the last 5 games the player will
    level up. On 3 missed games in the last 5 games, the player will level down.

  Authors: CleverPet Inc.
           Jelmer Tiete <jelmer@tiete.be>

  Copyright 2019
  Licensed under the AGPL 3.0
*/

#include "hackerpet.h"
#include "game_helper_functions.h"



/// The actual MatchingTwoColors challenge. This function needs to be called in a loop.
bool playMatchingTwoColors(HubInterface * hub, trial_info *_trial_info);


bool MatchingTwoColors_Loop(HubInterface * hub, trial_info *_trial_info);

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

//     // Play 1 interaction of the Matching Two Colors challenge
//     gameIsComplete = playMatchingTwoColors(); // Returns true if level is done

//     if(gameIsComplete){
//         // Interaction end
//         return;
//     }

// }
#endif