#ifndef GAME_HELPER_FUNCTIONS_H
#define GAME_HELPER_FUNCTIONS_H

#include <hackerpet.h>

// Set this to the name of your player (dog, cat, etc.)
const char playerName[] = "Pet, Clever";

// Use primary serial over USB interface for logging output (9600)
// Choose logging level here (ERROR, WARN, INFO)
SerialLogHandler logHandler(LOG_LEVEL_INFO, { // Logging level for all messages
    { "app.hackerpet", LOG_LEVEL_ERROR }, // Logging level for library messages
    { "app", LOG_LEVEL_INFO } // Logging level for application messages
});


// access to hub functionality (lights, foodtreats, etc.)
extern HubInterface hub;




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


#endif