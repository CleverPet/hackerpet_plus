#ifndef G_RESPONDINGQUICKLY_H
#define G_RESPONDINGQUICKLY_H

/**
  Responding Quickly
  ====================

    This is the seventh challenge from the original CleverPet learning
    curriculum.

    In this challenge, immediately after your player presses one illuminated
    touchpad, a different one lights up and must be touched. Your player now has
    to press two touchpads in a row to solve one puzzle. Chaining behaviors
    together requires that your player complete multiple correct actions in
    sequence to earn a reward. Your player must learn to stay attentive for a
    second instruction even after the first press.

    **Challenge logic:** This challenge has 4 levels. With each higher level the
    maximum reaction time decreases. One interaction consists of 2 moves, in
    each move 1 touchpad lights up and has to be touched to advance. The
    reaction time is the time between the first and second moves. If 17
    successful interactions occur over  the previous 20, the player levels up.
    After 17 misses the player levels down. On a miss there will be a
    between-interaction delay. For the player's first move  multiple touchpads
    can be touched, as long as the lit up touchpad was one of them (player has
    unlimited opportunities to try and get it right). The second touch, however,
    allows only the lit up touchpad to be touched, and the interaction will
    result in a miss if any of the lit touchpads is pressed more than once, or
    if the unlit touchpad is pressed at all.

  Authors: CleverPet Inc.
           Jelmer Tiete <jelmer@tiete.be>

  Copyright 2019
  Licensed under the AGPL 3.0
*/

#include "hackerpet/hackerpet.h"
#include "game_helper_functions.h"


/// The actual RespondingQuickly challenge. This function needs to be called in a loop.
bool playRespondingQuickly(HubInterface * hub);

bool RespondingQuickly_Loop(HubInterface * hub);


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

//   // Play 1 interaction of the Responding Quickly challenge
//   gameIsComplete = playRespondingQuickly(); // Will return true if level is done

//   if (gameIsComplete) {
//     // Interaction end
//     return;
//   }
// }

#endif