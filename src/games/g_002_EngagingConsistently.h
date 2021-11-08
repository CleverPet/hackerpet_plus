#ifndef G_ENGAGINGCONSISTENTLY_H
#define G_ENGAGINGCONSISTENTLY_H

/**
  Engaging Consistently
  =====================

    This challenge takes off the training wheels and no longer offers your
    player free foodtreats. Now your player will need to press a touchpad to
    earn a reward.

    Through repeated action and feedback from the Hub, players learn that the
    front of the Hub — where the touchpads are — is the most interesting part.
    Before this challenge, a very patient player could hang out and simply wait
    for foodtreats to arrive on their own.

    **Challenge logic:** This challenge uses a combination of performance and a
    timer to progress the game-play. There are 3 levels and 3 corresponding
    timer durations (10 minutes, 10 minutes, and 5 minutes respectively). The
    timer starts running, and the player can play so long as the timer hasn't
    expired. During the challenge the player levels up if 10 foodtreats have
    been eaten. When the timer expires, the level is checked and the
    corresponding timer length is loaded into the timer. The current count of
    foodtreats eaten is cleared and new interactions can continue to be played
    while the timer is running. Once the player reaches level 3 and eats 10
    foodtreats the max level is reached. Leveling down requires many misses,
    since the player would need to fail to consume 99 foodtreats in one level.
    When this happens, the timer is immediately reset and the duration is
    adjusted. Another way to level down is be to ignore the touchpads, which
    will make the current interaction time-out.

  Authors: CleverPet Inc.
           Jelmer Tiete <jelmer@tiete.be>

  Copyright 2019
  Licensed under the AGPL 3.0
*/

#include <hackerpet.h>
#include "game_helper_functions.h"



/// The actual EngagingConsistently challenge. This function needs to be called in a loop.
bool playEngagingConsistently(HubInterface * hub);


// new loop to call; same as original loop() below, but without hub->Run(20)
bool EngagingConsistently_Loop(HubInterface * hub);

#endif