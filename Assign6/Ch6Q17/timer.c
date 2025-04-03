#include "nu32dip.h"          // constants, funcs for startup and UART
#define TICKSPERSECOND 24000000

static volatile int timing;

void __ISR(_EXTERNAL_0_VECTOR, IPL6SOFT) Ext0ISR(void) { // step 1: the ISR
  
  int start = _CP0_GET_COUNT();
  while(_CP0_GET_COUNT() - start < 240000) { ;} // delay for 10ms

  if(!PORTBbits.RB7)
  {
    return; // if the button is not pressed after 10ms, it is not real
  }

  char msg[100];

  if(!timing){ // if timing == 0, we haven't started timing
    timing = 1;
    _CP0_SET_COUNT(0);
    sprintf(msg, "Press the USER button again to stop the timer.\r\n");  
    NU32DIP_WriteUART1(msg);
  }
  else // else, we are already timing
  {
    int final = _CP0_GET_COUNT();
    sprintf(msg, "%lf seconds elapsed\r\n\r\n", (final * 1.0)/TICKSPERSECOND);
    NU32DIP_WriteUART1(msg);
    timing = 0;

  }
  
  IFS0bits.INT0IF = 0;            // clear interrupt flag IFS0<3>
}

int main(void) {
  NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  __builtin_disable_interrupts(); // step 2: disable interrupts
  INTCONbits.INT0EP = 0;          // step 3: INT0 triggers on falling edge
  IPC0bits.INT0IP = 6;            // step 4: interrupt priority 2
  IPC0bits.INT0IS = 1;            // step 4: interrupt priority 1
  IFS0bits.INT0IF = 0;            // step 5: clear the int flag
  IEC0bits.INT0IE = 1;            // step 6: enable INT0 by setting IEC0<3>
  __builtin_enable_interrupts();  // step 7: enable interrupts
                                  // Connect RD7 (USER button) to INT0 (RD0)
  
  char start[100];
  sprintf(start, "Press the USER button to start the timer.\r\n");  
  NU32DIP_WriteUART1(start);
  timing = 0;

  while(1) {
      ; // do nothing, loop forever
  }

  return 0;
}
