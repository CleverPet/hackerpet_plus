

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

#include "games/g_007_LearningBrightness.h"

#include "hackerpet.h"
#include "game_helper_functions.h"

namespace LearningBrightness
{
    /**
     * Challenge settings
     * -------------
     *
     * These constants (capitalized SNAKE_CASE) and variables (camelCase) define the
     * gameplay
     */
    int currentLevel = 1; // starting and current level
    const int MAX_LEVEL=           4;   // Maximum number of levels
    const int HISTORY_LENGTH=      50;   // Number of previous interactions to look at for performance
    const int ENOUGH_SUCCESSES=    40;   // if num successes >= ENOUGH_SUCCESSES level-up
    const unsigned long FOODTREAT_DURATION = 6000; // (ms) how long to present foodtreat
    const unsigned long TIMEOUT_MS = 300000; // (ms) how long to wait until restarting the interaction
    const unsigned long MAX_REACTION_TIME = 20000; // (ms) timeout time for second interaction
    const unsigned long INTER_GAME_DELAY = 6000;
    const unsigned char TARGET_INTENSITY = 80; // touchpad target intensity
    const unsigned char SLEW = 20; // touchpad lights fade time

    /**
     * Global variables and constants
     * ------------------------------
     */
    const unsigned long SOUND_FOODTREAT_DELAY = 1200; // (ms) delay for reward sound
    const unsigned long SOUND_TOUCHPAD_DELAY = 300; // (ms) delay for touchpad sound
    const unsigned long VIEW_WINDOW = 200;
    // make sure you understand the challenge logic before changing
    const unsigned char DISTRACTOR_INTENSITY_MIN[MAX_LEVEL] = {1,1,5,10};
    const unsigned char DISTRACTOR_INTENSITY_MAX[MAX_LEVEL] = {5,10,15,18};
    const unsigned char DISTRACTOR_INTENSITY_TRESHOLD_MIN[MAX_LEVEL] = {0,5,10,10};
    const unsigned char DISTRACTOR_INTENSITY_TRESHOLD_MAX[MAX_LEVEL] = {255,255,255,16};

    bool performance[HISTORY_LENGTH] = {0}; // store the progress in this challenge
    unsigned char perfPos = 0; // to keep our position in the performance array
    unsigned char perfDepth = 0;  // to keep the size of the number of perf numbers
                                // to consider
}


/// The actual LearningBrightness challenge. This function needs to be called in a loop.
bool playLearningBrightness(HubInterface * hub, trial_info *_trial_info){
    using namespace LearningBrightness;
    yield_begin();

    static unsigned char distractor_intensity_probes[8] = { // should not be re-initialized
        17, 21, 26, 33, 41, 51, 64, 80};
    static unsigned long gameStartTime, timestampBefore, activityDuration = 0;
    static unsigned long timestampTouchpad = 0;
    static unsigned char pressed[2] = {0,0};
    static unsigned char foodtreatState = 99;
    static unsigned char touchpads[3]={hub->BUTTON_LEFT,  // should not be re-initialized
                                     hub->BUTTON_MIDDLE,
                                     hub->BUTTON_RIGHT};
    static unsigned char distractor_intensity = 0; // should not be re-initialized
    static bool probe_game = false;
    static bool retryTarget = false; // should not be re-initialized
    static bool accurate = false;
    static bool timeout = false;
    static bool foodtreatWasEaten = false; // store if foodtreat was eaten in last interaction
    static bool challengeComplete = false; // do not re-initialize

  // Static variables and constants are only initialized once, and need to be re-initialized
  // on subsequent calls
    gameStartTime = 0;
    timestampBefore = 0;
    activityDuration = 0;
    timestampTouchpad = 0;
    pressed[0] = 0;
    pressed[1] = 0;
    foodtreatState = 99;
    probe_game = false;
    accurate = false;
    timeout = false;
    foodtreatWasEaten = false; // store if foodtreat was eaten in last interaction

    Log.info("-------------------------------------------");
    // Log.info("Starting new \"Learning Brightness\" challenge");
    Log.info("Current level: %u, successes: %u, number of misses: %u",
        currentLevel, countSuccesses(performance, perfDepth), countMisses(performance, perfDepth));

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

    // Record start timestamp for performance logging
    timestampBefore = millis();

    if (retryTarget){
        Log.info("We're doing a retry interaction");
    } else {
        // randomly shuffle our 3 touchpads in an array
        random_shuffle(&touchpads[0], &touchpads[3]);
        // randomly choose distractor intensity
        distractor_intensity = random(DISTRACTOR_INTENSITY_MIN[currentLevel-1],
                                    DISTRACTOR_INTENSITY_MAX[currentLevel-1]);
        // In level 4 we add random probes of higher distractor intensities
        if ((currentLevel > 3)
            && (distractor_intensity > DISTRACTOR_INTENSITY_TRESHOLD_MAX[currentLevel-1])){
            Log.info("We're doing a probe interaction");
            probe_game = true;
            // randomly shuffle our probe list
            random_shuffle(&distractor_intensity_probes[0], &distractor_intensity_probes[8]);
            // pick first probe in list
            distractor_intensity = distractor_intensity_probes[0];
        }
    }

    Log.info("Distractor intensity: %d",distractor_intensity);

    // set first target
    hub->SetLights(touchpads[0],TARGET_INTENSITY,TARGET_INTENSITY,SLEW);

    // progress to next state
    timestampTouchpad = millis();
    do
    {
        // detect any touchpads currently pressed
        pressed[0] = hub->AnyButtonPressed();
        yield(false); // use yields statements any time the hub is pausing or waiting
    } // Ignore non-target touches
    while (!(pressed[0] == touchpads[0]) //0 if only pressed touchpad match
            && millis()  < timestampTouchpad + TIMEOUT_MS); //0 if timed out

    hub->SetLights(hub->LIGHT_BTNS, 0, 0, 0); // turn off all touchpad lights

    // wait until: no touchpad is currently pressed
    yield_wait_for((!hub->AnyButtonPressed()), false);

    // Check touchpads and accuracy for first interaction
    if (pressed[0] == touchpads[0]){
        Log.info("First interaction: correct touchpad pressed");
        timeout = false;

        // set up second interaction
        hub->SetLights(touchpads[1],TARGET_INTENSITY,TARGET_INTENSITY,SLEW);   // target
        hub->SetLights(touchpads[2],distractor_intensity,distractor_intensity,SLEW); // distractor

        yield_sleep_ms(VIEW_WINDOW, false); // make sure the player has seen the touchpad

        timestampTouchpad = millis();

        do
        {
            // detect any touchpads currently pressed
            pressed[1] = hub->AnyButtonPressed();
            yield(false); // use yields statements any time the hub is pausing or waiting
        }
        while (!(pressed[1] != 0) //0 if any touchpad is touched
                && millis()  < timestampTouchpad + MAX_REACTION_TIME); //0 if timed out

        hub->SetLights(hub->LIGHT_BTNS, 0, 0, 0); // turn off all touchpad lights

        // record time period for performance logging
        activityDuration = millis() - timestampBefore;

        //check touchpads and accuracy for second interaction
        if (pressed[1] == 0){
            Log.info("Second interaction: no touchpad pressed, timeout");
            timeout = true;
            accurate = false;
        } else {
            timeout = false;
            if (pressed[1] == touchpads[1]){
                accurate = true;
            } else {
                accurate = false;
            }
        }
    } else {
        Log.info("First interaction: no touchpad pressed, timeout");
        timeout = true;
        accurate = false;
    }

    if (accurate){
        Log.info("Second interaction: correct touchpad pressed, dispensing foodtreat");
        // give the Hub a moment to finish playing the touchpad sound
        yield_sleep_ms(SOUND_TOUCHPAD_DELAY, false);
        hub->PlayAudio(hub->AUDIO_POSITIVE, 20);
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
        if (!timeout){
            Log.info("Second interaction: wrong touchpad pressed");
            // give the Hub a moment to finish playing the touchpad sound
            yield_sleep_ms(SOUND_TOUCHPAD_DELAY, false);
            hub->PlayAudio(hub->AUDIO_NEGATIVE, 5);
            // give the Hub a moment to finish playing the sound
            yield_sleep_ms(SOUND_FOODTREAT_DELAY, false);
            foodtreatWasEaten = false;
        }
    }

    // don't keep track of performance on retry interactions
    // only keep track on interactions with distractor_intensity higher than level threshold
    if (!timeout){
        if(!retryTarget
            && (distractor_intensity > DISTRACTOR_INTENSITY_TRESHOLD_MIN[currentLevel-1])
            && (distractor_intensity <= DISTRACTOR_INTENSITY_TRESHOLD_MAX[currentLevel-1])){
            addResultToPerformanceHistory(accurate, performance, perfDepth, perfPos, HISTORY_LENGTH);
        } else {
            Log.info("Retry interaction and/or distractor intensity outside treshold: \
discarding performance.");
        }
    }

    _trial_info->food_eaten = foodtreatWasEaten;

    // Check if we're ready for next challenge
    if (currentLevel == MAX_LEVEL){
        if (countSuccesses(performance, perfDepth) >= ENOUGH_SUCCESSES){
            Log.info("At MAX level! %u", currentLevel);
            challengeComplete = true;
            resetPerformanceHistory(performance, perfDepth, perfPos, HISTORY_LENGTH);
        }
    } else {
        // Increase level if foodtreat eaten and good performance in this level
        if (countSuccesses(performance, perfDepth) >= ENOUGH_SUCCESSES){
            if (currentLevel < MAX_LEVEL){
                currentLevel++;
                Log.info("Leveling UP %u", currentLevel);
                resetPerformanceHistory(performance, perfDepth, perfPos, HISTORY_LENGTH);
            }
        }
    }

    if(!timeout){
        // Send report
        Log.info("Sending report");

        String extra = "{\"targetSeq\":\"";
        extra += convertBitfieldToLetter(touchpads[0], hub);
        extra += convertBitfieldToLetter(touchpads[1], hub);
        extra += "\",\"pressedSeq\":\"";
        // Multiple touches possible, but irrelevant for reporting
        extra += convertBitfieldToLetter(touchpads[0], hub);
        // multiple touches possible, only report the wrong one if a miss
        extra += convertBitfieldToSingleLetter(touchpads[1],pressed[1], hub);
        extra += String::format(
            "\",\"distractor_intensity\":%d,\"retryGame\":\"%c\"",
            distractor_intensity, retryTarget ? '1' : '0');
        if (challengeComplete) {extra += ",\"challengeComplete\":1";}
        extra += "}";

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
    }

    // Check if we need to do, or reset a retry interaction
    if (accurate){
        // Reset retry interaction
        retryTarget = false;
    } else if (!timeout && !probe_game) {
        // Set retry interaction, escept on timeout or probe interaction
        retryTarget = true;
    }

    // printPerformanceArray();

    if(retryTarget){
        // between interaction wait time if a miss
        yield_sleep_ms(INTER_GAME_DELAY, false);
    }

    hub->SetDIResetLock(false);  // allow DI board to reset if needed between interactions
    yield_finish();
    return true;
}

bool LearningBrightness_Loop(HubInterface * hub, trial_info *_trial_info)
{
    using namespace LearningBrightness;
    bool gameIsComplete = false;
    gameIsComplete = playLearningBrightness(hub, _trial_info); // Returns true if level is done
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

//     // Play 1 interaction of the Learning Brightness challenge
//     gameIsComplete = playLearningBrightness(); // Returns true if level is done

//     if(gameIsComplete){
//         // Interaction end
//         return;
//     }

// }
