
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

#include "games/g_003_AvoidingUnlitTouchpads.h"

#include "hackerpet.h"
#include "game_helper_functions.h"

namespace AvoidingUnlitTouchpads
{
    /**
     * Challenge settings
     * -------------
     *
     * These constants (capitalized SNAKE_CASE) and variables (camelCase) define the
     * gameplay
     */
    int currentLevel = 1;               // starting and current level
    const int MAX_LEVEL = 2;              // Maximum number of levels
    const int HISTORY_LENGTH = 20;        // Number of previous interactions to look at for
                                        // performance
    const int ENOUGH_SUCCESSES = 18;      // if num successes >= ENOUGH_SUCCESSES level-up
    const unsigned long FOODTREAT_DURATION = 5000; // (ms) how long to present foodtreat
    const int FLASHING = 0;
    const int FLASHING_DUTY_CYCLE = 99;
    const unsigned long TIMEOUT_MS = 60000; // (ms) how long to wait until restarting
                                          // the interaction
    const int NUM_PADS = 2; // Choose number of lit-up pads at a time (1, 2 or 3)

    /**
     * Global variables and constants
     * ------------------------------
     */
    const unsigned long SOUND_FOODTREAT_DELAY = 1200; // (ms) delay for reward sound
    const unsigned long SOUND_TOUCHPAD_DELAY = 300; // (ms) delay for touchpad sound

    bool performance[HISTORY_LENGTH] = {0}; // store the progress in this challenge
    int perfPos = 0;   // to keep our position in the performance array
    int perfDepth = 0; // to keep the size of the number of perf numbers to
                    // consider
}


/// The actual AvoidingUnlitTouchpads challenge. This function needs to be called in a loop.
bool playAvoidingUnlitTouchpads(HubInterface * hub, trial_info *_trial_info) {
  using namespace AvoidingUnlitTouchpads;
  yield_begin();

  static unsigned long gameStartTime, timestampBefore, activityDuration = 0;
  static unsigned char target = 0; // bitfield, holds target touchpad
  static unsigned char  retryTarget = 0; // should not be re-initialized
  static int foodfoodtreatState = 99;
  static unsigned char pressed = 0; // bitfield, holds pressed touchpad
  static unsigned long timestampTouchpad = 0;
  static bool foodtreatWasEaten = false; // store if foodtreat was eaten
  static bool accurate = false;
  static bool timeout = false;
  static bool challengeComplete = false; // do not re-initialize
  static int yellow = 60; // touchpad color, is randomized
  static int blue = 60; // touchpad color, is randomized

  // Static variables and constants are only initialized once, and need to be re-initialized
  // on subsequent calls
  gameStartTime = 0;
  timestampBefore = 0;
  activityDuration = 0;
  target = 0; // bitfield, holds target touchpad
  foodfoodtreatState = 99;
  pressed = 0; // bitfield, holds pressed touchpad
  timestampTouchpad = 0;
  foodtreatWasEaten = false; // store if foodtreat was eaten
  accurate = false;
  timeout = false;
  yellow = 60; // touchpad color, is randomized
  blue = 60; // touchpad color, is randomized

  Log.info("-------------------------------------------");
  Log.info("Starting new \"Avoiding Unlit Touchpads\" challenge");
  // Log.info("Current level: %u, successes: %u, number of misses: %u",
  //     currentLevel, countSuccesses(), countMisses());

  gameStartTime = Time.now();

  // before starting interaction, wait until:
  //  1. device layer is ready (in a good state)
  //  2. foodmachine is "idle", meaning it is not spinning or dispensing
  //      and tray is retracted (see FOODMACHINE_... constants)
  //  3. no touchpad is currently pressed
  yield_wait_for((hub->IsReady() &&
                  hub->FoodmachineState() == hub->FOODMACHINE_IDLE &&
                  !hub->AnyButtonPressed()),
                 false);

  // DI reset occurs if, for example, device  layer detects that touchpads need
  // re-calibration
  hub->SetDIResetLock(true);

  // Record start timestamp for performance logging
  timestampBefore = millis();

  yellow = random(20, 90); // pick a yellow for interaction
  blue = random(20, 90);    // pick a blue for interaction

  if (retryTarget != 0) {
    Log.info("We're doing a retry interaction");
    target = retryTarget;
    hub->SetLights(target, yellow, blue, FLASHING, FLASHING_DUTY_CYCLE);
  } else {
    // choose some target lights, and store which targets were randomly chosen
    target = hub->SetRandomButtonLights(NUM_PADS, yellow, blue, FLASHING,
                                       FLASHING_DUTY_CYCLE);
  }

  // progress to next state
  timestampTouchpad = millis();

  do {
    // detect any touchpads currently pressed
    pressed = hub->AnyButtonPressed();
    yield(false); // use yields statements any time the hub is pausing or waiting
  } while (
      (pressed != hub->BUTTON_LEFT // we want it to just be a single touchpad
       && pressed != hub->BUTTON_MIDDLE && pressed != hub->BUTTON_RIGHT) &&
      millis() < timestampTouchpad + TIMEOUT_MS);

  // record time period for performance logging
  activityDuration = millis() - timestampBefore;

  hub->SetLights(hub->LIGHT_BTNS, 0, 0, 0); // turn off lights

  // Check touchpads and accuracy
  if (pressed == 0) {
    Log.info("No touchpad pressed, we have a timeout");
    timeout = true;
    accurate = false;
  } else {
    // Log.info("Button pressed");
    timeout = false;
    accurate = !((pressed & target) == 0); // will be zero if and only if the
  }                                        // wrong touchpad was touched

  foodtreatWasEaten = false;

  // Check result and consequences
  if (accurate) {
    Log.info("Correct touchpad pressed, dispensing foodtreat");
    // give the Hub a moment to finish playing the touchpad sound
    yield_sleep_ms(SOUND_TOUCHPAD_DELAY, false);
    hub->PlayAudio(hub->AUDIO_POSITIVE, 20);
    // give the Hub a moment to finish playing the reward sound
    yield_sleep_ms(SOUND_FOODTREAT_DELAY, false);
    // if successful interaction, present foodtreat using PresentAndCheckFoodtreat
    // state machine
    do {
      foodfoodtreatState =
          hub->PresentAndCheckFoodtreat(FOODTREAT_DURATION); // time pres (ms)
      yield(false);
    } while (foodfoodtreatState != hub->PACT_RESPONSE_FOODTREAT_NOT_TAKEN &&
             foodfoodtreatState != hub->PACT_RESPONSE_FOODTREAT_TAKEN);

    // Check if foodtreat was eaten
    if (foodfoodtreatState == hub->PACT_RESPONSE_FOODTREAT_TAKEN) {
      Log.info("Foodtreat was eaten");
      foodtreatWasEaten = true;
    } else {
      Log.info("Foodtreat was not eaten");
      foodtreatWasEaten = false;
    }
  } else {
    if (!timeout) // don't play any audio if there's a timeout (no response -> no
                  // consequence)
    {
      Log.info("Wrong touchpad pressed");
      // give the Hub a moment to finish playing the touchpad sound
      yield_sleep_ms(SOUND_TOUCHPAD_DELAY, false);
      // if unsuccessful interaction: play negative feedback sound at low volume
      hub->PlayAudio(hub->AUDIO_NEGATIVE, 5);
      yield_sleep_ms(
          SOUND_FOODTREAT_DELAY,
          false); // give the Hub a moment to finish playing the sound
    }
  }

  _trial_info->food_eaten = foodtreatWasEaten;

  // Check if we're ready for next challenge
  if (currentLevel == MAX_LEVEL) {
    addResultToPerformanceHistory(accurate, performance, perfDepth, perfPos, HISTORY_LENGTH);
    if (countSuccesses(performance, perfDepth) >= ENOUGH_SUCCESSES) {
      Log.info("At MAX level! %u", currentLevel);
      challengeComplete = true;
      resetPerformanceHistory(performance, perfDepth, perfPos, HISTORY_LENGTH);
    }
  } else {
    // Increase level if foodtreat eaten and good performance in this level
    addResultToPerformanceHistory(accurate, performance, perfDepth, perfPos, HISTORY_LENGTH);
    if (countSuccesses(performance, perfDepth) >= ENOUGH_SUCCESSES) {
      if (currentLevel < MAX_LEVEL) {
        currentLevel++;
        Log.info("Leveling UP %u", currentLevel);
        resetPerformanceHistory(performance, perfDepth, perfPos, HISTORY_LENGTH);
      }
    }
  }

  if (!timeout) {
    // Send report
    Log.info("Sending report");

    String extra = "{\"targets\":\"";
    extra += convertBitfieldToLetter(target, hub);
    extra += "\",\"pressed\":\"";
    extra += convertBitfieldToLetter(pressed, hub);
    extra += String::format("\",\"retryGame\":\"%c\"", retryTarget ? '1' : '0');
    if (challengeComplete) {extra += ",\"challengeComplete\":1";}
    extra += "}";

    hub->Report(Time.format(gameStartTime,
                           TIME_FORMAT_ISO8601_FULL), // play_start_time
               PlayerName,                           // player
               currentLevel,                         // level
               String(accurate),                      // result
               activityDuration,   // duration -> linked to level and includes
                                    // tray movement
               accurate,            // foodtreat_presented
               foodtreatWasEaten, // foodtreatWasEaten
               extra                // extra field
    );
  }

  // Check if we need to do, or reset a retry interaction
  if (accurate) {
    // Reset retry interaction
    retryTarget = 0;
  } else if (!timeout && (currentLevel > 1)) {
    retryTarget = target;
  }

  // printPerformanceArray();

  // between interaction wait time
  yield_sleep_ms((unsigned long)random(1000, 8000), false);

  hub->SetDIResetLock(false); // allow DI board to reset if needed between interactions
  yield_finish();
  return true;
}


bool AvoidingUnlitTouchpads_Loop(HubInterface * hub, trial_info *_trial_info)
{
  using namespace AvoidingUnlitTouchpads;
  bool gameIsComplete = false;
  gameIsComplete = playAvoidingUnlitTouchpads(hub, _trial_info);// Returns true if level is done
  return gameIsComplete;
}

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
