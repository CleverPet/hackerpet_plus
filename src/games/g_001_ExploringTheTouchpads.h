#ifndef G_EXPLORINGTHETOUCHPADS_H
#define G_EXPLORINGTHETOUCHPADS_H

/**

*****************************************************
THIS FILE IS COPIED FROM hackerpet-games/ExploringTheTouchpads.cpp
BUT, LEAVING OUT setup() and loop() functions!
*****************************************************


  Exploring The Touchpads
  =======================

    This is the second challenge from the original CleverPet learning
    curriculum.

    A foodtreat is still offered periodically “for free”, but the player will
    also earn a reward when they press a touchpad — even if just "by accident".

    This challenge is designed to have players begin to associate their own
    behavior with getting a foodtreat. Through a combination of classical and
    operant conditioning, players begin to understand that the Hub is a tool
    they can control to get tasty snacks.

    **Challenge logic:** The challenge consists of 4 levels with accompanying
    time-out speeds and foodtreat presentation intervals. If 3 foodtreats were
    eaten in the last 6 interactions, the player levels up. If 4 foodtreats were
    left uneaten within the last 6 interactions, the player levels down. When
    the player reaches level 4 and eats 3 foodtreats, the level resets back to
    level 4.

  Authors:    CleverPet Inc.
              Jelmer Tiete <jelmer@tiete.be>

  Copyright 2019
  Licensed under the AGPL 3.0
*/

#include "hackerpet.h"
#include "game_helper_functions.h"


//// The actual ExploringTheTouchpads function. This function needs to be called in a loop.
bool playExploringTheTouchpads(HubInterface * hub);


// new loop to call; same as original loop() below, but without hub->Run(20)
bool ExploringTheTouchpads_Loop(HubInterface * hub);



// original for reference:

// /**
//  * Setup function
//  * --------------
//  */
// void setup() {
//   // Initializes the hub and passes the current filename as ID for reporting
//   hub->Initialize(__FILE__);
//   Log.info("Starting new \"Exploring The Touchpads\" challenge");
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

//   // Play 1 level of the ExploringTheTouchpads challenge
//   gameIsComplete = playExploringTheTouchpads(); // Will return true if level is done

//   if (gameIsComplete) {
//     return;
//   }
// }

#endif
