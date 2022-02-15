#ifndef G_LEARNINGDOUBLESEQUENCES_H
#define G_LEARNINGDOUBLESEQUENCES_H

/**
  Learning Double Sequences
  =========================

    This is the ninth challenge from the original CleverPet learning curriculum.

    This challenge engages your player with sequences of actions. For a
    successful interaction, your player must press the brightest touchpad
    accurately twice in a row.

    During this challenge, the Hub gives your player information that will help
    them plan their next move. The dim touchpad during the first sequence
    becomes the the player's next target. Clever players will catch onto this
    and use this “hint” to prepare their next press.

    **Challenge logic:** This challenge has only one level. The challenge has 3
    moves per interaction: 1 stimulator round and 2 moves that are part of a
    sequence. The stimulator round lights up all 3 touchpads and waits for the
    player to touch any of them in any combination.

    For the player's second move the touchpads light up 1 random target and also
    light up the next target at a lower intensity. The target, and only the
    target pad, needs to be touched for the interaction to continue to the next
    round in the sequence.

    When the player has had 40 successful games in the last 50 games, the player
    has reached the maximum level.

  Authors: CleverPet Inc.
           Jelmer Tiete <jelmer@tiete.be>

  Copyright 2019
  Licensed under the AGPL 3.0
*/

#include "hackerpet.h"
#include "game_helper_functions.h"

/// The actual LearningDoubleSequences challenge. This function needs to be called in a loop.
bool playLearningDoubleSequences(HubInterface * hub);


bool LearningDoubleSequences_Loop(HubInterface * hub);


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

//     // Play 1 interaction of the Learning Double Sequences challenge
//     // Will return true if level is done
//     gameIsComplete = playLearningDoubleSequences();

//     if(gameIsComplete){
//         // Interaction end
//         return;
//     }

// }
#endif