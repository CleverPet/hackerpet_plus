#ifndef G_AVOIDINGUNLITTOUCHPADS_H
#define G_AVOIDINGUNLITTOUCHPADS_H

/**
  Avoiding Unlit Touchpads
  ========================

    This is the fourth challenge from the original CleverPet learning
    curriculum.

    In this challenge only two of the three touchpads are lit, and your player
    must learn that only pressing illuminated touchpads will result in a reward.
    This is the first time the Hub’s lights begin to serve as meaningful cues.
    Using trial and error, your player will gather evidence and figure out how
    the Hub works, and what to do to earn a reward. All attempts to get kibble
    (whether successful or not) are part of the learning process. As the player
    learns, they will hit the unlit touchpads less and less often.

    **Challenge logic:** This challenge has 2 levels. The first level chooses 2
    targets at random. If 1 of the targets is pressed, the accurate flag is set
    and a foodtreat is dispensed. If accurate, a positive interaction is added
    to the performance history. When 18 of the previous 20 interactions are
    accurate, the player levels up. At level 2 the interaction is identical save
    that if the player has gotten it wrong they will have to redo the
    interaction: if the player pushes the wrong touchpad, the player will be
    presented with the same targets on the next interaction. This prevents the
    player from developing and keeping a favorite touchpad. If the interaction
    times out, a miss wil be added to the performance array. Between
    interactions there is a random wait of between 1 and 8 seconds.

  Authors: CleverPet Inc.
           Jelmer Tiete <jelmer@tiete.be>

  Copyright 2019
  Licensed under the AGPL 3.0
*/

#include "hackerpet/hackerpet.h"
#include "game_helper_functions.h"


/// The actual AvoidingUnlitTouchpads challenge. This function needs to be called in a loop.
bool playAvoidingUnlitTouchpads(HubInterface * hub);


bool AvoidingUnlitTouchpads_Loop(HubInterface * hub);


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

//   // Play 1 interaction of the playAvoidingUnlitTouchpads challenge
//   gameIsComplete = playAvoidingUnlitTouchpads();// Returns true if level is done

//   if (gameIsComplete) {
//     // Interaction end
//     return;
//   }
// }

#endif