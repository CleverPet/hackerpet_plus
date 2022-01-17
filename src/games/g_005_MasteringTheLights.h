#ifndef G_MASTERINGTHELIGHTS_H
#define G_MASTERINGTHELIGHTS_H

/**
  Mastering The Lights
  ====================

    This is the sixth challenge from the original CleverPet learning curriculum.

    This challenge continues with only one lit touchpad, and now your player
    will need to engage even more consistently and accurately. There are some
    sneaky ways for players to get enough rewards without always paying
    attention to the information the lights provide. This challenge improves on
    the progress your player has made by making such strategies much less
    rewarding.

    **Challenge logic:** This challenge has only 1 level. If 40 successful
    interactions were played over the previous 50, the challenge is complete
    (will reset). Every interaction, 1 random target will be picked and compared
    with the pressed touchpad. Performance is tracked, but time outs have no
    effect on the game's estimate of the player's performance. When a the
    touchpad is pushed, the next interaction will be a redo with the same
    target. There is a random between-interaction pause ranging from 1 and 8
    seconds.

  Authors: CleverPet Inc.
           Jelmer Tiete <jelmer@tiete.be>

  Copyright 2019
  Licensed under the AGPL 3.0
 */


#include "hackerpet.h"
#include "game_helper_functions.h"


/// The actual MasteringTheLights challenge. This function needs to be called in a loop.
bool playMasteringTheLights(HubInterface * hub);


bool MasteringTheLights_Loop(HubInterface * hub);


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

//   // Play 1 interaction of the Mastering The Lights challenge
//   gameIsComplete = playMasteringTheLights(); // Will return true if level is done

//   if (gameIsComplete) {
//     // Interaction end
//     return;
//   }
// }

#endif