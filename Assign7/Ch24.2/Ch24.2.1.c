#include "nu32dip.h"

int main(void) {
    NU32DIP_Startup();

    RPB15Rbits.RPB15R = 0b0101;    // set B15 to be OC1
    T3CONbits.TCKPS = 0;        // Timer3 prescaler 1:1
    PR3 = 2399;                 // Period = (PR3 +1) * N * 20.83ns 
    TMR3 = 0;                   // Initial timer 3 count is 0
    OC1CONbits.OCTSEL = 1;      // Use timer 3
    OC1CONbits.OCM = 0b110;     // PWM mode without fault pin;
    OC1RS = 1800;               // duty cycle = OC1RS/PR3
    OC1R = 1800;                // 
    T3CONbits.ON = 1;           // turn on Timer3
    OC1CONbits.ON = 1;          // turn on OC1

    while (1) {
        ;
    }
    return 0;
}