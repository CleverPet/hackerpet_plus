#ifndef G_LEARNINGBRIGHTNESS_H
#define G_LEARNINGBRIGHTNESS_H

/**
  Learning Brightness
  ===================

    This is the eight challenge from the original CleverPet learning curriculum.

    After the first press, the other touchpads light up and your player will
    need to choose the brightest one. The touchpads start out with very
    different brightness levels and gradually become more difficult to tell
    apart.

    Your player now has to make a more careful comparison between the target
    (brighter) and distractor (dimmer), and will gradually improve as they learn
    to attend to the relevant feature. This skill will come in handy during the
    next few challenges.

    **Challenge logic:** This challenge has 4 levels with 4 corresponding
    minimum and maximum levels for distractor touchpad intensity. At the start
    of an interaction, the touchpads are randomly shuffled and a distractor
    intensity level is picked randomly. At the 4th level random probe
    interactions are introduced with randomly chosen higher (i.e., more
    challenging) distractor intensities.

    The first target in the interaction is a single target which the player gets
    unlimited retries to try and touch. The second target is a different pad
    from the first target and a distractor is added to the interaction, which is
    also different from the first target. If the player misses, the same targets
    will be presented in the next (redo) interaction, except on probe
    interactions.

    Performance on redo interactions and intensities outside the threshold
    interval aren't counted. There is a between-interaction wait time of 6
    seconds when the player misses. When a player has 40 successful interactions
    within the previous 50, the player will level up. The player cannot level
    down.

  Authors: CleverPet Inc.
           Jelmer Tiete <jelmer@tiete.be>

  Copyright 2019
  Licensed under the AGPL 3.0
*/

#include "hackerpet.h"
#include "game_helper_functions.h"


/// The actual LearningBrightness challenge. This function needs to be called in a loop.
bool playLearningBrightness(HubInterface * hub, trial_info *_trial_info);

bool LearningBrightness_Loop(HubInterface * hub, trial_info *_trial_info);

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

//     // Play 1 interaction of the Learning Brightness challenge
//     gameIsComplete = playLearningBrightness(); // Returns true if level is done

//     if(gameIsComplete){
//         // Interaction end
//         return;
//     }

// }
#endif