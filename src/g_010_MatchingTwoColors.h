/**
  Matching Two Colors
  ===================

    This is the eleventh challenge from the original CleverPet learning
    curriculum.  For the first time, the Hub introduces colors (blue and yellow
    - yes, dogs and cats can see color!) as a meaningful signal. Each time a
    touchpad is pressed, its color will change. Your player’s job is to make all
    the touchpads match within a limited number of overall presses.

    Up until now, there was always one correct answer about which pad to touch.
    This is the first challenge where each puzzle has more than one correct
    solution, and it showcases your player’s abilities solve problems creatively
    and in their own way.

    **Challenge logic:** This challenge has 4 levels. Every level has a
    corresponding maximum number of touches. The challenge uses two different
    colors, yellow and blue.

    At the start of an interaction every pad randomly gets one of the 2 colors.
    Every touch, the color of the touched touchpad advances to the next color.
    This all happens in a while loop, and with every iteration the state of the
    lights gets checked for a match of all 3 pads. If a match is found the while
    loop is exited and the player gets a foodtreat. Every touch decreases the
    maximum touches counter, if the touches are about to run out, the 'Do' sound
    will be played. When the remaining touches become zero, the interaction
    becomes a miss. On a miss the player's next interaction will be a retry
    interaction with the same starting state.

    If 4 successful games are played within the last 5 games the player will
    level up. On 3 missed games in the last 5 games, the player will level down.

  Authors: CleverPet Inc.
           Jelmer Tiete <jelmer@tiete.be>

  Copyright 2019
  Licensed under the AGPL 3.0
*/

#include <hackerpet.h>
#include "game_helper_functions.h"

namespace MatchingTwoColors
{
    /**
     * Challenge settings
     * -------------
     *
     * These constants (capitalized SNAKE_CASE) and variables (camelCase) define the
     * gameplay
     */
    const int STARTING_LEVEL = 1;
    const int MAX_LEVEL =           4;   // Maximum number of levels
    const int HISTORY_LENGTH =      5;   // Number of previous interactions to look at for performance
    const int ENOUGH_SUCCESSES =    4;   // if num successes >= ENOUGH_SUCCESSES level-up
    const int TOO_MANY_MISSES =     3;   // if number of misses >= TOO_MANY_MISSES level-down
    const int PADS_PRESSED_MAX[MAX_LEVEL] = {100,10,6,4};
    const unsigned long FOODTREAT_DURATION = 6000; // (ms) how long to present foodtreat
    const unsigned long TIMEOUT_MS = 300002; // (ms) how long to wait until restarting the interaction
    const unsigned long WRONG_INTERACTION_DELAY = 6000;
    const unsigned char TOUCHPADS[3][2][2] = { //[pad][color][Y,B] //TODO make this easier
                        {{90, 00},{00, 90}},
                        {{90, 00},{00, 90}},
                        {{90, 00},{00, 90}},
                        };
    const unsigned char REPORT_COLORS[2] = {'Y','B'};

    /**
     * Global variables and constants
     * ------------------------------
     */
    const unsigned long SOUND_DO_DELAY = 600; // (ms) delay between reward sound and foodtreat
    const unsigned long SOUND_FOODTREAT_DELAY = 600; // (ms) delay for reward sound
    const unsigned long SOUND_TOUCHPAD_DELAY = 300; // (ms) delay for touchpad sound

    bool performance[HISTORY_LENGTH] = {0}; // store the progress in this challenge
    unsigned char perfPos = 0; // to keep our position in the performance array
    unsigned char perfDepth = 0; // to keep the size of the number of perf numbers to consider
    unsigned char touchpadsColor[3] = {};
}





/// The actual MatchingTwoColors challenge. This function needs to be called in a loop.
bool playMatchingTwoColors(){
    using namespace MatchingTwoColors;
    yield_begin();

    static unsigned long timestampBefore, timestampTouchpad, gameStartTime, activityDuration = 0;
    static unsigned char foodtreatState = 99;
    static unsigned char touchpadsColorStart[3] = {};
    static unsigned char pressed = 0;
    static int currentLevel = STARTING_LEVEL;
    static int pads_pressed = 0;
    static bool match = false;
    static bool retryGame = false;  // should not be re-initialized
    static bool accurate = false;
    static bool timeout = false;
    static bool foodtreatWasEaten = false; // store if foodtreat was eaten in last interaction
    static bool challengeComplete = false; // do not re-initialize
    static String pressedSeq = "";

    // Static variables and constants are only initialized once, and need to be re-initialized
    // on subsequent calls
    timestampBefore = 0;
    gameStartTime = 0;
    activityDuration = 0;
    foodtreatState = 99;
    match = false;
    pads_pressed = 0;
    fill(touchpadsColorStart, touchpadsColorStart+3, 0);
    accurate = false;
    timeout = false;
    foodtreatWasEaten = false; // store if foodtreat was eaten in last interaction
    pressed = 0;
    timestampTouchpad = 0;
    pressedSeq = "";

    Log.info("-------------------------------------------");
    // Log.info("Starting new \"Matching Two Colors\" challenge");
    // Log.info("Current level: %u, successes: %u, number of misses: %u",
        // currentLevel, countSuccesses(), countMisses());

    gameStartTime = Time.now();

    // before starting interaction, wait until:
    //  1. device layer is ready (in a good state)
    //  2. foodmachine is "idle", meaning it is not spinning or dispensing
    //      and tray is retracted (see FOODMACHINE_... constants)
    //  3. no touchpad is currently pressed
    yield_wait_for((hub.IsReady()
                    && hub.FoodmachineState() == hub.FOODMACHINE_IDLE
                    && not hub.AnyButtonPressed()), false);

    // DI reset occurs if, for example, device  layer detects that touchpads need re-calibration
    hub.SetDIResetLock(true);

    // Rndomly pick start state, except on retry interaction
    if (!retryGame){
        do{
            touchpadsColor[0] = random(0,2);
            touchpadsColor[1] = random(0,2);
            touchpadsColor[2] = random(0,2);
        } while (checkMatch(touchpadsColor));
    } else {
        Log.info("Doing a retry interaction");
    }

    // save start state for report
    copy(begin(touchpadsColor), end(touchpadsColor), begin(touchpadsColorStart));

    Log.info("Start state: %c%c%c", REPORT_COLORS[touchpadsColor[0]],
                                    REPORT_COLORS[touchpadsColor[1]],
                                    REPORT_COLORS[touchpadsColor[2]]);

    updateTouchpadLights(TOUCHPADS, touchpadsColor);

    hub.SetButtonAudioEnabled(true);

    // Record start timestamp for performance logging
    timestampBefore = millis();

    // main while loop
    while (match == false){
        timestampTouchpad = millis();
        do
        {
            yield(false); // use yields statements any time the hub is pausing or waiting
            // detect any touchpads currently pressed
            pressed = hub.AnyButtonPressed();
        }
        while (!(pressed != 0) //0 if any touchpad is touched
                && millis()  < timestampTouchpad + TIMEOUT_MS); //0 if timed out

        activityDuration = millis() - timestampBefore;

        if (pressed == hub.BUTTON_LEFT){
            Log.info("Left touchpad pressed");
            advanceTouchpad_MatchingTwoColors(0, touchpadsColor);
        } else if (pressed == hub.BUTTON_MIDDLE){
            Log.info("Middle touchpad pressed");
            advanceTouchpad_MatchingTwoColors(1, touchpadsColor);
        } else if (pressed == hub.BUTTON_RIGHT){
            Log.info("Right touchpad pressed");
            advanceTouchpad_MatchingTwoColors(2, touchpadsColor);
        } else if (pressed == 0) {
            timeout = true;
            accurate = false;
        } else {
            // double pad press
        }

        // add our press to the reporting sequence
        pressedSeq += convertBitfieldToLetter_ColorMatch(pressed);

        // update lights
        updateTouchpadLights(TOUCHPADS, touchpadsColor);

        // increase pressed counter
        pads_pressed++;
        Log.info("Remaining presses: %u", PADS_PRESSED_MAX[currentLevel-1]-pads_pressed);
        // check for timeout
        if (activityDuration > TIMEOUT_MS ){
            Log.info("Timeout");
            timeout = true;
            break;
        }
        // check for match
        if (checkMatch(touchpadsColor)){
            Log.info("We have a match");
            match = true;
            break;
        }
        // check for last tries
        if (pads_pressed >= PADS_PRESSED_MAX[currentLevel-1]-2){
            // Log.info("almost out");
            // give the Hub a moment to finish playing the touchpad sound
            yield_sleep_ms(SOUND_TOUCHPAD_DELAY, false);
            hub.PlayAudio(hub.AUDIO_DO, 60);
            // give the Hub a moment to finish playing the sound
            yield_sleep_ms(SOUND_DO_DELAY, false);
        }
        //check for max tries
        if (pads_pressed == PADS_PRESSED_MAX[currentLevel-1]){
            Log.info("Max presses");
            break;
        }

        // wait until: no touchpad is currently pressed
        yield_wait_for((!hub.AnyButtonPressed()), false);
    }

    // check if we have a match on all touchpads
    accurate = checkMatch(touchpadsColor);

    if (accurate){
        timeout = false; // rare case
        retryGame = false;
        Log.info("Match, dispensing foodtreat");
        // give the Hub a moment to finish playing the touchpad sound
        yield_sleep_ms(SOUND_TOUCHPAD_DELAY, false);
        hub.PlayAudio(hub.AUDIO_POSITIVE, 80);
        // give the Hub a moment to finish playing the reward sound
        yield_sleep_ms(SOUND_FOODTREAT_DELAY, false);
        do {
            foodtreatState=hub.PresentAndCheckFoodtreat(FOODTREAT_DURATION);
            yield(false);
        } while (foodtreatState!=hub.PACT_RESPONSE_FOODTREAT_NOT_TAKEN &&
             foodtreatState!=hub.PACT_RESPONSE_FOODTREAT_TAKEN);

        // Check if foodtreat was eaten
        if (foodtreatState == hub.PACT_RESPONSE_FOODTREAT_TAKEN){
            Log.info("Foodtreat was eaten");
            foodtreatWasEaten = true;
        } else {
            Log.info("Foodtreat was not eaten");
            foodtreatWasEaten = false;
        }
    } else {
        retryGame = true;
        if (!timeout){
            // give the Hub a moment to finish playing the touchpad sound
            yield_sleep_ms(SOUND_TOUCHPAD_DELAY, false);
            hub.PlayAudio(hub.AUDIO_NEGATIVE, 80);
            // give the Hub a moment to finish playing the reward sound
            yield_sleep_ms(SOUND_FOODTREAT_DELAY, false);
            // turn off touchpads sound and light during time-out
            hub.SetLights(hub.LIGHT_BTNS, 0, 0, 0);
            hub.SetButtonAudioEnabled(false);
            yield_sleep_ms(WRONG_INTERACTION_DELAY, false);
        }
    }

    hub.SetLights(hub.LIGHT_BTNS, 0, 0, 0); // turn off all touchpad lights

    // keep track of performance
    if (!timeout){
        addResultToPerformanceHistory(accurate, performance, perfDepth, perfPos, HISTORY_LENGTH);
    }

    // Check if we're ready for next challenge
    if (currentLevel == MAX_LEVEL){
        if (countSuccesses(performance, perfDepth) >= ENOUGH_SUCCESSES){
            Log.info("At MAX level! %u", currentLevel);
            challengeComplete = true;
            resetPerformanceHistory(performance, perfDepth, perfPos, HISTORY_LENGTH);
        }
    }

    if (currentLevel < MAX_LEVEL){
        if (countSuccesses(performance, perfDepth) >= ENOUGH_SUCCESSES){
            currentLevel++;
            Log.info("Leveling UP %u", currentLevel);
            retryGame = false;
            resetPerformanceHistory(performance, perfDepth, perfPos, HISTORY_LENGTH);
        }
    }

    if (countMisses(performance, perfDepth) >= TOO_MANY_MISSES){
        if (currentLevel > 1){
            currentLevel--;
            Log.info("Leveling DOWN %u", currentLevel);
            retryGame = false;
            resetPerformanceHistory(performance, perfDepth, perfPos, HISTORY_LENGTH);
        }
    }

    // Send report
    if(!timeout){
        Log.info("Sending report");

        String extra = String::format(
            "{\"startState\":\"%c%c%c\",\"pressedSeq\":\"%s\",\"presses\":%u,"
            "\"retryGame\":%c",
            REPORT_COLORS[touchpadsColorStart[0]],
            REPORT_COLORS[touchpadsColorStart[1]],
            REPORT_COLORS[touchpadsColorStart[2]], pressedSeq.c_str(),
            pads_pressed,
            retryGame ? '1' : '0');  // TODO this is the new value
        if (challengeComplete) {extra += ",\"challengeComplete\":1";}
        extra += "}";

        // Log.info(extra);

        hub.Report(Time.format(gameStartTime,
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

    // printPerformanceArray();

    hub.SetDIResetLock(false);  // allow DI board to reset if needed between interactions
    yield_finish();
    return true;
}


bool MatchingTwoColors_Loop()
{
  using namespace MatchingTwoColors;
  bool gameIsComplete = false;
  gameIsComplete = playMatchingTwoColors();// Returns true if level is done
  return gameIsComplete;
}

// /**
//  * Setup function
//  * --------------
//  */
// void setup() {
//   // Initializes the hub and passes the current filename as ID for reporting
//   hub.Initialize(__FILE__);
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
//     hub.Run(20);

//     // Play 1 interaction of the Matching Two Colors challenge
//     gameIsComplete = playMatchingTwoColors(); // Returns true if level is done

//     if(gameIsComplete){
//         // Interaction end
//         return;
//     }

// }
