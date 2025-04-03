#include "calculate.h"

/******************************************************************************
 * HELPER FUNCTIONS   ==SecA.4.2==
 ******************************************************************************/

/* calculateGrowth
 *
 * This optimistically-named function fills the array with the investment 
 * value over the years, given the parameters in *invp.  
 */
void calculateGrowth(Investment *invp) {

  int i;

  // for loop ==SecA.4.13==
  for (i = 1; i <= invp->years; i= i + 1) {   // relational operators ==SecA.4.10==
                                              // struct access ==SecA.4.4==
    invp->invarray[i] = invp->growth * invp->invarray[i-1]; 
  }
} // ***** END calculateGrowth *****
