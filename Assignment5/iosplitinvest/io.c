
#include "io.h"

/* getUserInput
 * 
 * This reads the user's input into the struct pointed at by invp, 
 * and returns TRUE (1) if the input is valid, FALSE (0) if not.
 */
int getUserInput(Investment *invp) {

  int valid;       // int used as a boolean ==SecA.4.10==

  char prompt[100], input[100], validstr[100], invalidstr[100];

  // I/O functions in stdio.h ==SecA.4.14==
  sprintf(prompt, "Enter investment, growth rate, number of yrs (up to %d): ",MAX_YEARS);
  NU32DIP_WriteUART1(prompt);

  NU32DIP_ReadUART1(input, 100);
  sscanf(input, "%lf %lf %d", &(invp->inv0), &(invp->growth), &(invp->years));

  // logical operators ==SecA.4.11==
  valid = (invp->inv0 > 0) && (invp->growth > 0) &&
    (invp->years > 0) && (invp->years <= MAX_YEARS);

  sprintf(validstr, "Valid input?  %d\r\n",valid);
  NU32DIP_WriteUART1(validstr);

  // if-else ==SecA.4.12==
  if (!valid) { // ! is logical NOT ==SecA.4.11== 
    
    sprintf(invalidstr, "Invalid input; exiting.\r\n");
    NU32DIP_WriteUART1(invalidstr);
  }
  return(valid);
} // ***** END getUserInput *****


/* sendOutput
 *
 * This function takes the array of investment values (a pointer to the first
 * element, which is a double) and the number of years (an int).  We could
 * have just passed a pointer to the entire investment record, but we decided 
 * to demonstrate some different syntax.
 */
void sendOutput(double *arr, int yrs) {

  int i;
  char results[50], newline[50], outstring[100];      // defining a string ==SecA.4.9==

  sprintf(results, "\nRESULTS:\n\r\n");
  NU32DIP_WriteUART1(results);

  for (i=0; i<=yrs; i++) {  // ++, +=, math in ==SecA.4.7==
    sprintf(outstring,"Year %3d:  %10.2f\r\n",i,arr[i]); 
    NU32DIP_WriteUART1(outstring);
  }
  sprintf(newline, "\r\n");
  NU32DIP_WriteUART1(newline);
} // ***** END sendOutput *****
