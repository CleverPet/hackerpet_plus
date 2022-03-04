#ifndef GAME_HELPER_FUNCTIONS_H
#define GAME_HELPER_FUNCTIONS_H

#include "hackerpet.h"
#include <algorithm>  // random_shuffle

// Set this to the name of your player (dog, cat, etc.)
const char playerName[] = "Pet, Clever";  // EatingTheFood
const char PlayerName[] = "Pet, Clever";  // other challenges


/**
 * Helper functions
 * ----------------
 */

//// return the number of positive interactions in performance history for current level
unsigned int countSuccesses(bool performance[], unsigned char perfDepth);

/// return the number of negative interactions in performance history for current level
unsigned int countMisses(bool performance[], unsigned char perfDepth);

/// reset performance history to 0
void resetPerformanceHistory(bool performance[], unsigned char perfDepth, unsigned char perfPos, const int HISTORY_LENGTH);

/// add an interaction result to the performance history
void addResultToPerformanceHistory(bool entry, bool performance[], unsigned char perfDepth, unsigned char perfPos, const int HISTORY_LENGTH);

/// print the performance history for debugging
void printPerformanceArray(bool performance[], const int HISTORY_LENGTH);

/* 004 version:
void printPerformanceArray();
*/



/// converts a bitfield of pressed touchpads to letters
/// multiple consecutive touches are possible and will be reported L -> M - > R
/// @returns String
String convertBitfieldToLetter(unsigned char pad, HubInterface * hub);

/// converts a bitfield of pressed touchpads to letters
/// multiple consecutive touches will be reported as X
/// this version ! !! has return X option !!! this version was used only in colormatch
/// @returns String
String convertBitfieldToLetter_ColorMatch(unsigned char pad, HubInterface * hub);



/// converts requested touchpad bitfield and pressed touchpad bitfield to a
/// letter. Requested bitfield should have only one bit set. Touched pad bitfield
/// can have multiple bits set. If correct, the correct pad will be returned, if
/// wrong only the wrong pad will be returned, if multiple wrong pads pressed,
/// only one wrong pad will be returned in order L -> M -> R
/// @returns String
String convertBitfieldToSingleLetter(unsigned char targetPad, unsigned char pad, HubInterface * hub);

// For ColorMatch challenges:

/// update the touchpad lights on the hub
void updateTouchpadLights(const unsigned char TOUCHPADS[][2][2], unsigned char touchpadsColor[], HubInterface * hub);
void updateTouchpadLights(const unsigned char TOUCHPADS[][3][2], unsigned char touchpadsColor[], HubInterface * hub);
/// check if all touchpad colors match
bool checkMatch(unsigned char touchpadsColor[]);

/// advance a touchpad to the next color
void advanceTouchpad_MatchingTwoColors(unsigned char pad, unsigned char touchpadsColor[]);


/// advance a touchpad to the next color
void advanceTouchpad_MatchingMoreColors(unsigned char pad, unsigned char touchpadsColor[], int numberOfColors);



struct trial_info {
    bool food_eaten;
};


#endif