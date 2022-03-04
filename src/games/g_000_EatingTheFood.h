#ifndef G_EATINGTHEFOOD_H
#define G_EATINGTHEFOOD_H

/**
  Eating The Food
  ===============

    The goal of this challenge is to help your player get comfortable with the
    Hub’s sounds and movements.

    The Hub's dish will rotate out and offer a free foodtreat to your player at
    varying intervals. If your player does not take the foodtreat, the dish will
    stay out for progressively longer periods of time.

    **Challenge logic**: This challenge has 6 levels with 6 corresponding
    foodtreat offer durations. A foodtreat is automatically offered to the
    player for the corresponding time. If the foodtreat isn't eaten the level is
    increased and the offer duration will be extended. If the foodtreat was
    eaten the level will go down and presentation time will be shortened. If 3
    foodtreats were eaten in the previous 5 interactions, the challenge is
    completed and the performance array will be reset.

  Authors: CleverPet Inc. Jelmer Tiete <jelmer@tiete.be>

  Copyright 2019
  Licensed under the AGPL 3.0
 */

#include "hackerpet.h"
#include "game_helper_functions.h"



/**
 * Helper functions
 * ----------------
 */

/// The actual Eating The Food challenge. Function must be be called in a loop.
bool playEatingTheFood(HubInterface * hub, trial_info *_trial_info);


// new loop to call; same as original loop() below, but without hub->Run(20)
bool EatingTheFood_Loop(HubInterface * hub, trial_info *_trial_info);




// /**
//  * Setup function
//  * --------------
//  */
// void setup() {
//   // Initializes the hub and passes the current filename as ID for reporting
//   hub->Initialize(__FILE__);
//   // You can also pass your own ID like so
//   // hub->Initialize("MyAwesomeGame");
// }

// /**
//  * Main loop function
//  * ------------------
//  */
// void loop() {
//   unsigned int perf_total = 0;  // sum of performance of the
//                                 // HISTORY_LENGTH last interactions
//   bool gameIsComplete = false;

//   // Advance the device layer state machine, but with 20 ms max time
//   // spent per loop cycle.
//   hub->Run(20);

//   // Play 1 level of the Eating The Food challenge
//   gameIsComplete = playEatingTheFood();  // Will return true if level is done

//   // Store level result in performance array
//   if (gameIsComplete) {
//     performance[perfPos] = foodtreatWasEaten;  // store the interaction result
//                                                // in the performance array
//     perfPos++;
//     if (perfPos > (HISTORY_LENGTH - 1)) {  // make our performance array circular
//       perfPos = 0;
//     }
//   }

//   // Check performance array if we're ready to pass to next challenge
//   for (int i = 0; i < HISTORY_LENGTH; ++i) {
//     perf_total += performance[i];
//     if (perf_total >= ENOUGH_SUCCESSES) {
//       Log.info("Challenge completed!");
//       challengeComplete = true;
//       // reset performance
//       perf_total = 0;
//       for (unsigned char i = 0; i < HISTORY_LENGTH; ++i) performance[i] = 0;
//     }
//   }
// }

#endif