
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

#include "games/g_008_LearningDoubleSequences.h"

#include "hackerpet.h"
#include "game_helper_functions.h"

namespace LearningDoubleSequences
{
    /**
     * Challenge settings
     * -------------
     *
     * These constants (capitalized SNAKE_CASE) and variables (camelCase) define the
     * gameplay
     */
    int currentLevel = 1;
    const int MAX_LEVEL=           1;   // Maximum number of levels
    const int HISTORY_LENGTH=      50;   // Number of previous interactions to look at for performance
    const int ENOUGH_SUCCESSES=    40;   // if num successes >= ENOUGH_SUCCESSES level-up
    const int SEQUENCE_LENGTH = 2;
    const int TARGET_INTENSITY = 75;
    const int NEXT_TARGET_INTENSITY = 10;
    const int SLEW = 90;
    const unsigned long FOODTREAT_DURATION = 6000; // (ms) how long to present foodtreat
    const unsigned long TIMEOUT_MS = 300002; // (ms) how long to wait until restarting the interaction
    const unsigned long INTER_GAME_DELAY = 10000;

    /**
     * Global variables and constants
     * ------------------------------
     */
    const unsigned long SOUND_FOODTREAT_DELAY = 1200; // (ms) delay for reward sound
    const unsigned long SOUND_TOUCHPAD_DELAY = 300; // (ms) delay for touchpad sound

    bool performance[HISTORY_LENGTH] = {0}; // store the progress in this challenge
    unsigned char perfPos = 0; // to keep our position in the performance array
    unsigned char perfDepth = 0; // to keep the size of the number of perf numbers to consider
}


/// The actual LearningDoubleSequences challenge. This function needs to be called in a loop.
bool playLearningDoubleSequences(HubInterface * hub, trial_info *_trial_info){
    using namespace LearningDoubleSequences;
    yield_begin();

    static unsigned long gameStartTime, timestampTouchpad, timestampBefore, activityDuration = 0;
    static unsigned char foodtreatState = 99;
    static unsigned char touchpads[3]={hub->BUTTON_LEFT,
                                     hub->BUTTON_MIDDLE,
                                     hub->BUTTON_RIGHT};
    static unsigned char sequence_pos = 0;
    static unsigned char touchpad_sequence[SEQUENCE_LENGTH]={};
    static unsigned char pressed[SEQUENCE_LENGTH+1] = {};
    static bool accurate = false;
    static bool timeout = false;
    static bool foodtreatWasEaten = false; // store if foodtreat was eaten in last interaction
    static bool challengeComplete = false; // do not re-initialize

  // Static variables and constants are only initialized once, and need to be re-initialized
  // on subsequent calls
    gameStartTime=0;
    timestampTouchpad=0;
    timestampBefore =0;
    activityDuration = 0;
    foodtreatState = 99;
    sequence_pos = 0;
    touchpads[0]=hub->BUTTON_LEFT;
    touchpads[1]=hub->BUTTON_MIDDLE;
    touchpads[2]=hub->BUTTON_RIGHT;
    fill(touchpad_sequence, touchpad_sequence+SEQUENCE_LENGTH, 0);
    fill(pressed, pressed+SEQUENCE_LENGTH+1, 0);
    accurate = false;
    timeout = false;
    foodtreatWasEaten = false; // store if foodtreat was eaten in last interaction

    Log.info("-------------------------------------------");
    // Log.info("Starting new \"Learning Double Sequences\" challenge");
    // Log.info("Current level: %u, successes: %u, number of misses: %u",
        // currentLevel, countSuccesses(), countMisses());

    gameStartTime = Time.now();

    // before starting interaction, wait until:
    //  1. device layer is ready (in a good state)
    //  2. foodmachine is "idle", meaning it is not spinning or dispensing
    //      and tray is retracted (see FOODMACHINE_... constants)
    //  3. no touchpad is currently pressed
    yield_wait_for((hub->IsReady()
                    && hub->FoodmachineState() == hub->FOODMACHINE_IDLE
                    && not hub->AnyButtonPressed()), false);

    // DI reset occurs if, for example, device  layer detects that touchpads need re-calibration
    hub->SetDIResetLock(true);

    // fill touchpad_sequence
    for (int i = 0; i < SEQUENCE_LENGTH; ++i)
    {
        random_shuffle(&touchpads[0], &touchpads[3]);
        touchpad_sequence[i] = touchpads[0];
    }

    // turn on all touchpads
    hub->SetLights(hub->LIGHT_BTNS,TARGET_INTENSITY,TARGET_INTENSITY,SLEW);

    // progress to next state
    timestampTouchpad = millis();

    do
    {
        // detect any touchpads currently pressed
        pressed[0] = hub->AnyButtonPressed();
        yield(false); // use yields statements any time the hub is pausing or waiting
    }
    while (!(pressed[0] != 0) //0 if any touchpad is touched
            && millis()  < timestampTouchpad + TIMEOUT_MS); //0 if timed out

    hub->SetLights(hub->LIGHT_BTNS, 0, 0, 0); // turn off all touchpad lights

    // wait until: no touchpad is currently pressed
    yield_wait_for((!hub->AnyButtonPressed()), false);

    if (pressed[0] != 0)
    {
        Log.info("Stimulator touchpad touched, starting interactions");
        sequence_pos = 0;
        timeout = false;
        accurate = true;
    } else {
        Log.info("No touchpad pressed, timeout");
        sequence_pos = SEQUENCE_LENGTH;
        accurate = false;
        timeout = true;
    }

    // Record start timestamp for performance logging
    timestampBefore = millis();

    // Start main interactions loop
    while (sequence_pos < SEQUENCE_LENGTH) {
        Log.info("Interaction %d. Target touchpad: %c%c%c", (sequence_pos+1),
            (touchpad_sequence[sequence_pos]&hub->BUTTON_LEFT)?'1':'0',
            (touchpad_sequence[sequence_pos]&hub->BUTTON_MIDDLE)?'1':'0',
            (touchpad_sequence[sequence_pos]&hub->BUTTON_RIGHT)?'1':'0');

        // wait until: no touchpad is currently pressed
        yield_wait_for((!hub->AnyButtonPressed()), false);
        // TODO fix slobber

        // set next touchpad and this touchpad
        if (sequence_pos < SEQUENCE_LENGTH-1)
          hub->SetLights(touchpad_sequence[sequence_pos + 1],
                        NEXT_TARGET_INTENSITY, NEXT_TARGET_INTENSITY, SLEW);
        hub->SetLights(touchpad_sequence[sequence_pos],TARGET_INTENSITY,TARGET_INTENSITY,SLEW);

        // progress to next state
        timestampTouchpad = millis();

        do
        {
            // detect any touchpads currently pressed
            pressed[sequence_pos+1] = hub->AnyButtonPressed();
            yield(false); // use yields statements any time the hub is pausing or waiting
        }
        while (!(pressed[sequence_pos+1] != 0) //0 if any touchpad is touched
                && millis()  < timestampTouchpad + TIMEOUT_MS); //0 if timed out


        if (pressed[sequence_pos+1] == 0) {
            Log.info("No touchpad pressed, timeout");
            timeout = true;
            accurate = false;
            sequence_pos = SEQUENCE_LENGTH;
        } else if (pressed[sequence_pos+1] == touchpad_sequence[sequence_pos]) {
             Log.info("Correct touchpad pressed");
            // turn off lights on last touchpad
            hub->SetLights(touchpad_sequence[sequence_pos],0,0,0);
            sequence_pos++;
            accurate = true;
            timeout = false;
        } else { // asuming wrong
            Log.info("Wrong touchpad pressed");
            accurate = false;
            timeout = false;
            sequence_pos = SEQUENCE_LENGTH;
        }
    }

    // record time period for performance logging
    activityDuration = millis() - timestampBefore;

    hub->SetLights(hub->LIGHT_BTNS, 0, 0, 0); // turn off all touchpad lights


    if (accurate) {
        Log.info("All interactions passed, dispensing foodtreat");
        // give the Hub a moment to finish playing the touchpad sound
        yield_sleep_ms(SOUND_TOUCHPAD_DELAY, false);
        hub->PlayAudio(hub->AUDIO_POSITIVE, 60);
        // give the Hub a moment to finish playing the reward sound
        yield_sleep_ms(SOUND_FOODTREAT_DELAY, false);
        do {
            foodtreatState=hub->PresentAndCheckFoodtreat(FOODTREAT_DURATION);
            yield(false);
        } while (foodtreatState!=hub->PACT_RESPONSE_FOODTREAT_NOT_TAKEN &&
             foodtreatState!=hub->PACT_RESPONSE_FOODTREAT_TAKEN);

        // Check if foodtreat was eaten
        if (foodtreatState == hub->PACT_RESPONSE_FOODTREAT_TAKEN){
            Log.info("Foodtreat was eaten");
            foodtreatWasEaten = true;
        } else {
            Log.info("Foodtreat was not eaten");
            foodtreatWasEaten = false;
        }
    } else {
        if (!timeout) {
            // give the Hub a moment to finish playing the touchpad sound
            yield_sleep_ms(SOUND_TOUCHPAD_DELAY, false);
            hub->PlayAudio(hub->AUDIO_NEGATIVE, 60);
            // give the Hub a moment to finish playing the sound
            yield_sleep_ms(SOUND_FOODTREAT_DELAY, false);
            foodtreatWasEaten = false;
        }
    }

    // keep track of performance
    if (!timeout){
        addResultToPerformanceHistory(accurate, performance, perfDepth, perfPos, HISTORY_LENGTH);
    }

    _trial_info->food_eaten = foodtreatWasEaten;

    // Check if we're ready for next challenge
    if (currentLevel == MAX_LEVEL){
        if (countSuccesses(performance, perfDepth) >= ENOUGH_SUCCESSES){
            Log.info("At MAX level! %u", currentLevel);
            challengeComplete = true;
            resetPerformanceHistory(performance, perfDepth, perfPos, HISTORY_LENGTH);
        }
    }

    // Send report
    // if(!timeout){
        Log.info("Sending report");

        String extra ="{";
        extra += "\"targetSeq\":\"";
        for (int i = 0; i < SEQUENCE_LENGTH; ++i){
            extra += convertBitfieldToLetter(touchpad_sequence[i], hub);
        }
        extra += "\",\"pressedSeq\":\"";
        for (int i = 0; i < SEQUENCE_LENGTH; ++i){
            extra += convertBitfieldToSingleLetter(touchpad_sequence[i],pressed[i+1], hub);
        }
        extra += "\"";
        if (challengeComplete) {extra += ",\"challengeComplete\":1";}
        extra += "}";

        // Log.info(extra);

        hub->Report(Time.format(gameStartTime,
                               TIME_FORMAT_ISO8601_FULL),  // play_start_time
                   PlayerName,                             // player
                   currentLevel,                           // level
                   String(accurate),                       // result
                   activityDuration,                       // duration
                   accurate,           // foodtreat_presented
                   foodtreatWasEaten,  // foodtreatWasEaten
                   extra               // extra field
        );
        // }

        // printPerformanceArray();

        if (!accurate) {
          // between interaction wait time if a miss
          yield_sleep_ms(INTER_GAME_DELAY, false);
    }

    hub->SetDIResetLock(false);  // allow DI board to reset if needed between interactions
    yield_finish();
    return true;
}



bool LearningDoubleSequences_Loop(HubInterface * hub, trial_info *_trial_info)
{
  using namespace LearningDoubleSequences;
  bool gameIsComplete = false;
  gameIsComplete = playLearningDoubleSequences(hub, _trial_info);// Returns true if level is done
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
