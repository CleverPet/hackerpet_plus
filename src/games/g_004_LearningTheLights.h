#ifndef G_LEARNINGTHELIGHTS_H
#define G_LEARNINGTHELIGHTS_H

/**
  Learning The Lights
  ===================

    This is the fifth challenge from the original CleverPet learning curriculum.

    Only one touchpad is lit. This is the challenge where most players will
    start to "see the lights". If your player wasn’t using the information
    provided by the lights on the previous challenge, they could expect to get
    rewarded about two-thirds of the time, just by chance. Now, chance is only 1
    in 3 guesses. The lights are twice as important as before!

    **Challenge logic:** This challenge has only 1 level. if 30 successful
    interactions were played in the last 50 games, the challenge is done (will
    reset). Every interaction, 1 random random target will be picked and
    compared with the pressed touchpad. The player's performance is recorded,
    and only correct presses count.  When an incorrect touchpad is pressed, the
    next interaction will be a redo interaction with the same target. There is a
    random between interactions pause ranging from 1 to 8 seconds

  Authors: CleverPet Inc.
           Jelmer Tiete <jelmer@tiete.be>

  Copyright 2019
  Licensed under the AGPL 3.0
*/

#include "hackerpet.h"
#include "game_helper_functions.h"

/// The actual LearningTheLights challenge. This function needs to be called in a loop.
bool playLearningTheLights(HubInterface * hub);

bool LearningTheLights_Loop(HubInterface * hub);


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
// void loop() {
//   bool gameIsComplete = false;

//   // Advance the device layer state machine, but with 20 ms max time
//   // spent per loop cycle.
//   hub->Run(20);

//   // Play 1 interaction of the Learning The Lights challenge
//   gameIsComplete = playLearningTheLights(); // Will return true if level is done

//   if (gameIsComplete) {
//     // Interaction end
//     return;
//   }
// }

#endif