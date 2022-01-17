#ifndef G_LEARNINGLONGERSEQUENCES_H
#define G_LEARNINGLONGERSEQUENCES_H

/**
  Learning Longer Sequences
  =========================

    This is the tenth challenge from the original CleverPet learning curriculum.

    The sequences get longer! Now, your player is challenged to complete
    patterns of up to nine moves in a row. Beginning with move sequences of
    length three, the sequences get progressively longer with the player's
    success, and shorter when the difficulty is too high.

    To master this challenge, players need to stay attentive and accurate over
    progressively longer periods of time. They are given a few "lives" so that
    one missed touchpad doesn’t always send them back to the beginning.

    **Challenge logic:** The sequence increases if 12 of the last 15
    interactions were successes, it also decreases if 12 of the last 15
    interactions were misses.

    The first interaction in the challenge is a "stimulator round" where all 3
    touchpads light up and any combination of pads can be touched to advance the
    interaction. The second interaction starts the sequence. At every step of
    the sequence the current target will light up, along with following target
    at lower intensity. The target must to be touched exactly, with no
    combinations or double touches allowed. On a wrong touchpad touch the player
    loses a "life" and is allowed to continue the interaction. If all 3 of the
    player's lives run out the interaction becomes a miss.

  Authors: CleverPet Inc.
           Jelmer Tiete <jelmer@tiete.be>

  Copyright 2019
  Licensed under the AGPL 3.0
*/

#include "hackerpet/hackerpet.h"
#include "game_helper_functions.h"


/// The actual LearningLongerSequences function. This function needs to be called in a loop.
bool playLearningLongerSequences(HubInterface * hub);

bool LearningLongerSequences_Loop(HubInterface * hub);

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

//     // Play 1 interaction of the Learning Longer Sequences challenge
//     // Will return true if level is done
//     gameIsComplete = playLearningLongerSequences();

//     if(gameIsComplete){
//         // Interaction end
//         return;
//     }
// }

#endif