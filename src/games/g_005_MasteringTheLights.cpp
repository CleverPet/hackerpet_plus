

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

#include "games/g_005_MasteringTheLights.h"

#include "hackerpet.h"
#include "game_helper_functions.h"

namespace MasteringTheLights
{
    /**
     * Challenge settings
     * -------------
     *
     * These constants (capitalized SNAKE_CASE) and variables (camelCase) define the
     * gameplay
     */
    int currentLevel = 1;
    const int MAX_LEVEL = 1;              // Maximum number of levels
    const int HISTORY_LENGTH = 50;        // Number of successes to look at for
                                        // performance
    const int ENOUGH_SUCCESSES = 40;      // if successes >= ENOUGH_SUCCESSES level-up
    const unsigned long FOODTREAT_DURATION = 5000; // (ms) how long to present foodtreat
    const int FLASHING = 0;
    const int FLASHING_DUTY_CYCLE = 99;
    const unsigned long TIMEOUT_MS = 60000; // (ms) how long to wait until restarting
                                          // the interaction
    const char NUM_PADS = 1; // Choose number of lit-up pads at a time (1, 2 or 3)

    /**
     * Global variables and constants
     * ------------------------------
     */
    const unsigned long SOUND_FOODTREAT_DELAY = 1200; // (ms) delay for reward sound
    const unsigned long SOUND_TOUCHPAD_DELAY = 300; // (ms) delay for touchpad sound

    bool performance[HISTORY_LENGTH] = {0}; // store the progress in this challenge
    unsigned char perfPos = 0;   // to keep our position in the performance array
    unsigned char perfDepth = 0; // to keep the size of the number of perf numbers
                                  // to consider
}


/// The actual MasteringTheLights challenge. This function needs to be called in a loop.
bool playMasteringTheLights(HubInterface * hub, trial_info *_trial_info) {
  using namespace MasteringTheLights;
  yield_begin();

  static unsigned long timestampBefore, activityDuration = 0;
  static unsigned long gameStartTime = 0;
  static unsigned char target = 0;
  static unsigned char retryTarget = 0; // should not be re-initialized
  static int foodtreatState = 99;
  static bool accurate = false;
  static bool timeout = false;
  static unsigned char pressed = 0;
  static unsigned long timestampTouchpad = 0;
  static int yellow = 60;
  static int blue = 60;
  static bool foodtreatWasEaten = false; // store if foodtreat was eaten in last interaction
  static bool challengeComplete = false; // do not re-initialize

  // Static variable and constants are only initialized once, and need to be re-initialized
  // on subsequent calls
  timestampBefore = 0;
  activityDuration = 0;
  gameStartTime = 0;
  target = 0;
  foodtreatState = 99;
  accurate = false;
  timeout = false;
  pressed = 0;
  timestampTouchpad = 0;
  yellow = 60;
  blue = 60;
  foodtreatWasEaten = false; // store if foodtreat was eaten in last interaction

  Log.info("-------------------------------------------");
  Log.info("Starting new \"Mastering The Lights\" challenge");
  // Log.info("Current level: %u, successes: %u, misses: %u",
  //     currentLevel, countSuccesses(), countMisses());

  gameStartTime = Time.now();

  // before starting interaction, wait until:
  //  1. device layer is ready (in a good state)
  //  2. foodmachine is "idle", meaning it is not spinning or dispensing foodtreat
  //      and tray is retracted (see FOODMACHINE_... constants)
  //  3. no button is currently pressed
  yield_wait_for((hub->IsReady() &&
                  hub->FoodmachineState() == hub->FOODMACHINE_IDLE &&
                  not hub->AnyButtonPressed()),
                 false);

  // DI reset occurs if, for example, device  layer detects that touchpads need
  // re-calibration
  hub->SetDIResetLock(true);

  // Record start timestamp for performance logging
  timestampBefore = millis();

  yellow = random(20, 90); // pick a yellow for interaction
  blue = random(20, 90);   // pick a blue for interaction

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
    // detect any buttons currently pressed
    pressed = hub->AnyButtonPressed();
    yield(false);
  } while (
      (pressed != hub->BUTTON_LEFT // we want it to just be a single touchpad
       && pressed != hub->BUTTON_MIDDLE && pressed != hub->BUTTON_RIGHT) &&
      millis() < timestampTouchpad + TIMEOUT_MS);

  // record time period for performance logging
  activityDuration = millis() - timestampBefore;

  hub->SetLights(hub->LIGHT_BTNS, 0, 0, 0); // turn off lights

  // Check buttons and accuracy
  if (pressed == 0) {
    Log.info("No button pressed, we have a timeout");
    timeout = true;
    accurate = false;
  } else {
    // Log.info("Button pressed");
    timeout = false;
    // will be zero if and only if the wrong touchpad was touched
    accurate = !((pressed & target) == 0);
  }

  foodtreatWasEaten = false;

  // Check result and consequences
  if (accurate) {
    Log.info("Correct button pressed, dispensing foodtreat");
    // give the Hub a moment to finish playing the touchpad sound
    yield_sleep_ms(SOUND_TOUCHPAD_DELAY, false);
    hub->PlayAudio(hub->AUDIO_POSITIVE, 20);
    // give the Hub a moment to finish playing the reward sound
    yield_sleep_ms(SOUND_FOODTREAT_DELAY, false);
    // if successful interaction, present foodtreat using PresentAndCheckFoodtreat
    // state machine
    do {
      foodtreatState =
          hub->PresentAndCheckFoodtreat(FOODTREAT_DURATION); // time pres (ms)
      yield(false);
    } while (foodtreatState != hub->PACT_RESPONSE_FOODTREAT_NOT_TAKEN &&
             foodtreatState != hub->PACT_RESPONSE_FOODTREAT_TAKEN);

    // Check if foodtreat was eaten
    if (foodtreatState == hub->PACT_RESPONSE_FOODTREAT_TAKEN) {
      Log.info("Treat was eaten");
      foodtreatWasEaten = true;
    } else {
      Log.info("Treat was not eaten");
      foodtreatWasEaten = false;
    }
  } else {
    if (!timeout) // don't play any audio if time out (no response -> no
                  // consequence)
    {
      Log.info("Wrong button pressed");
      // give the Hub a moment to finish playing the touchpad sound
      yield_sleep_ms(SOUND_TOUCHPAD_DELAY, false);
      // if unsuccessful interaction: play negative feedback sound at low volume
      hub->PlayAudio(hub->AUDIO_NEGATIVE, 5);
      // give the Hub a moment to finish playing the sound
      yield_sleep_ms(SOUND_FOODTREAT_DELAY,false);
    }
  }

  _trial_info->food_eaten = foodtreatWasEaten;

  // Don't update performance if we have a timeout
  if (!timeout) {
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
  } else if (!timeout) {
    // Set retry interaction
    retryTarget = target;
  }

  // printPerformanceArray();

  // between interaction wait time
  yield_sleep_ms((unsigned long)random(1000, 8000), false);

  hub->SetDIResetLock(false); // allow DI board to reset if needed between interactions
  yield_finish();
  return true;
}



bool MasteringTheLights_Loop(HubInterface * hub, trial_info *_trial_info)
{
  using namespace MasteringTheLights;
  bool gameIsComplete = false;
  gameIsComplete = playMasteringTheLights(hub, _trial_info);// Returns true if level is done
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

//   // Play 1 interaction of the Mastering The Lights challenge
//   gameIsComplete = playMasteringTheLights(); // Will return true if level is done

//   if (gameIsComplete) {
//     // Interaction end
//     return;
//   }
// }
