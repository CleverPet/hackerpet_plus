#include "game_helper_functions.h"

#include "hackerpet.h"
#include <algorithm>  // random_shuffle


/**
 * Helper functions
 * ----------------
 */

//// return the number of positive interactions in performance history for current level
unsigned int countSuccesses(bool performance[], unsigned char perfDepth) {
  unsigned int total = 0;
  for (unsigned char i = 0; i <= perfDepth - 1; i++)
    if (performance[i] == 1)
      total++;
  return total;
}

/// return the number of negative interactions in performance history for current level
unsigned int countMisses(bool performance[], unsigned char perfDepth) {
  unsigned int total = 0;
  for (unsigned char i = 0; i <= perfDepth - 1; i++)
    if (performance[i] == 0)
      total++;
  return total;
}

/// reset performance history to 0
void resetPerformanceHistory(bool performance[], unsigned char perfDepth, unsigned char perfPos, const int HISTORY_LENGTH) {
  for (unsigned char i = 0; i < HISTORY_LENGTH; i++)
    performance[i] = 0;
  perfPos = 0;
  perfDepth = 0;
}

/// add an interaction result to the performance history
void addResultToPerformanceHistory(bool entry, bool performance[], unsigned char perfDepth, unsigned char perfPos, const int HISTORY_LENGTH) {
  // Log.info("Adding %u", entry);
  performance[perfPos] = entry;
  perfPos++;
  if (perfDepth < HISTORY_LENGTH)
    perfDepth++;
  if (perfPos > (HISTORY_LENGTH - 1)) { // make our performance array circular
    perfPos = 0;
  }
  // Log.info("perfPos %u, perfDepth %u", perfPos, perfDepth);
  Log.info("New successes: %u, misses: %u", countSuccesses(performance, perfDepth),
           countMisses(performance, perfDepth));
}

/// print the performance history for debugging
void printPerformanceArray(bool performance[], const int HISTORY_LENGTH) {
  Serial.printf("performance: ");
  for (unsigned char i = 0; i < HISTORY_LENGTH; i++)
    Serial.printf("%u", performance[i]);
  Serial.printf("\n");
}

/* 004 version:
void printPerformanceArray() {
  Serial.printf("performance: {");
  for (unsigned char i = 0; i < perfDepth; i++) {
    Serial.printf("%u", performance[i]);
    if ((i + 1) == perfPos)
      Serial.printf("|");
  }
  Serial.printf("}\n");
}
*/



/// converts a bitfield of pressed touchpads to letters
/// multiple consecutive touches are possible and will be reported L -> M - > R
/// @returns String
String convertBitfieldToLetter(unsigned char pad, HubInterface * hub){
  String letters = "";
  if (pad & hub->BUTTON_LEFT)
    letters += 'L';
  if (pad & hub->BUTTON_MIDDLE)
    letters += 'M';
  if (pad & hub->BUTTON_RIGHT)
    letters += 'R';
  return letters;
}

/// converts a bitfield of pressed touchpads to letters
/// multiple consecutive touches will be reported as X
/// this version ! !! has return X option !!! this version was used only in colormatch
/// @returns String
String convertBitfieldToLetter_ColorMatch(unsigned char pad, HubInterface * hub){
  if ((pad & (pad-1)) != 0) // targetPad has multiple pads set
    return "X";

  String letters = "";
  if (pad & hub->BUTTON_LEFT)
    letters += 'L';
  if (pad & hub->BUTTON_MIDDLE)
    letters += 'M';
  if (pad & hub->BUTTON_RIGHT)
    letters += 'R';
  return letters;
}



/// converts requested touchpad bitfield and pressed touchpad bitfield to a
/// letter. Requested bitfield should have only one bit set. Touched pad bitfield
/// can have multiple bits set. If correct, the correct pad will be returned, if
/// wrong only the wrong pad will be returned, if multiple wrong pads pressed,
/// only one wrong pad will be returned in order L -> M -> R
/// @returns String
String convertBitfieldToSingleLetter(unsigned char targetPad, unsigned char pad, HubInterface * hub){
  if ((targetPad & (targetPad-1)) != 0) // error targetPad has multiple pads set
    return "X";

  String letter = "";
  if (targetPad == pad){ // did we get it right?
    letter += convertBitfieldToLetter(targetPad, hub); // report right pad
  }
  else { // we have a wrong pad touched or multiple pads touched (of which one is wrong)
    unsigned char maskedPressed = ~targetPad & pad; // mask out the correct pad
    if ((maskedPressed & (maskedPressed-1)) != 0) // check if multiple pads touched
                                                  //(except for correct one)
    {
      // multiple wrong pads touched
      //just pick one to report, L -> M -> R
      if (maskedPressed & hub->BUTTON_LEFT)
        letter += 'L';
      else if (maskedPressed & hub->BUTTON_MIDDLE)
        letter += 'M';
      else if (maskedPressed & hub->BUTTON_RIGHT)
        letter += 'R';
    } else {
      //only one wrong pad touched
      letter += convertBitfieldToLetter(maskedPressed, hub);
    }
  }
  return letter;
}

// For ColorMatch challenges:

/// update the touchpad lights on the hub
void updateTouchpadLights(const unsigned char TOUCHPADS[][2][2], unsigned char touchpadsColor[], HubInterface * hub){
  hub->SetLights(hub->LIGHT_LEFT, TOUCHPADS[0][touchpadsColor[0]][0],
                TOUCHPADS[0][touchpadsColor[0]][1], 0);
  hub->SetLights(hub->LIGHT_MIDDLE, TOUCHPADS[1][touchpadsColor[1]][0],
                TOUCHPADS[1][touchpadsColor[1]][1], 0);
  hub->SetLights(hub->LIGHT_RIGHT, TOUCHPADS[2][touchpadsColor[2]][0],
                TOUCHPADS[2][touchpadsColor[2]][1], 0);
};
void updateTouchpadLights(const unsigned char TOUCHPADS[][3][2], unsigned char touchpadsColor[], HubInterface * hub){
  hub->SetLights(hub->LIGHT_LEFT, TOUCHPADS[0][touchpadsColor[0]][0],
                TOUCHPADS[0][touchpadsColor[0]][1], 0);
  hub->SetLights(hub->LIGHT_MIDDLE, TOUCHPADS[1][touchpadsColor[1]][0],
                TOUCHPADS[1][touchpadsColor[1]][1], 0);
  hub->SetLights(hub->LIGHT_RIGHT, TOUCHPADS[2][touchpadsColor[2]][0],
                TOUCHPADS[2][touchpadsColor[2]][1], 0);
};

/// check if all touchpad colors match
bool checkMatch(unsigned char touchpadsColor[]){
    if ((touchpadsColor[0]==touchpadsColor[1]) && (touchpadsColor[1]==touchpadsColor[2]))
        return true;
    return false;
}

/// advance a touchpad to the next color
void advanceTouchpad_MatchingTwoColors(unsigned char pad, unsigned char touchpadsColor[]){
    touchpadsColor[pad]++;
    if (touchpadsColor[pad]>1)
        touchpadsColor[pad]=0;
}


/// advance a touchpad to the next color
void advanceTouchpad_MatchingMoreColors(unsigned char pad, unsigned char touchpadsColor[], int numberOfColors){
    touchpadsColor[pad]++;
    if (touchpadsColor[pad]>numberOfColors-1)
        touchpadsColor[pad]=0;
}

