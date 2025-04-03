#ifndef helper__H__
#define helper__H__

/******************************************************************************
 * PREPROCESSOR COMMANDS   ==SecA.4.3==
 ******************************************************************************/

#include <stdio.h>      // input/output library
#define MAX_YEARS 100   // constant indicating max number of years to track
#include "nu32dip.h"



/******************************************************************************
 * DATA TYPE DEFINITIONS (HERE, A STRUCT)  ==SecA.4.4==
 ******************************************************************************/

typedef struct {
  double inv0;                    // initial investment
  double growth;                  // growth rate, where 1.0 = zero growth
  int years;                      // number of years to track
  double invarray[MAX_YEARS+1];   // investment array   ==SecA.4.9==
} Investment;                     // the new data type is called Investment


/******************************************************************************
 * GLOBAL VARIABLES   ==SecA.4.2, A.4.5==
 ******************************************************************************/

// no global variables in this program

/******************************************************************************
 * HELPER FUNCTION PROTOTYPES  ==SecA.4.2==
 ******************************************************************************/

int getUserInput(Investment *invp);     // invp is a pointer to type ...
void calculateGrowth(Investment *invp); // ... Investment ==SecA.4.6, A.4.8==
void sendOutput(double *arr, int years);

#endif