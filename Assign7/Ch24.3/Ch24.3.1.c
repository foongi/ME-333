#include "nu32dip.h"
#define NUMSAMPS 1000 // number of points in
static volatile int Waveform[NUMSAMPS]; // waveform

void makeWaveForm();

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) {
    static int counter = 0; // init counter

    OC1RS = Waveform[counter];
    counter++;              // add one to counter
    if (counter == NUMSAMPS) {
        counter = 0;        // roll counter over
    }
    // clear interrupt flag
    IFS0bits.T2IF = 0;
}

void makeWaveform() {
    int i = 0;
    int center = 1200;
    int a = 600;

    for (i = 0; i < NUMSAMPS; i++)
    {
        if (i < NUMSAMPS/2) {
            Waveform[i] = center + a;
        } else {
            Waveform[i] = center - a;
        }
    }

}

int main(void) {
    NU32DIP_Startup();

    makeWaveform(); // initialize waveform

    __builtin_disable_interrupts();   // step 2: disable interrupts at CPU
     

    // 1kHz interrupt:
    T2CONbits.TCKPS = 2; // prescaler 1:4
    PR2 = 11999; // 12000 ticks
    TMR2 = 0;

    // step 3: T2 turn on
    T2CONbits.ON = 1;

    // step 4: clear 5 priority and subp bits for INT1
    // step 4: set INT1 to priority 6 subpriority 0  
    IPC2bits.T2IP = 5;
    IPC2bits.T2IS = 0;   

    // step 5: clear INT1 flag status
    // step 6: enable T2 interrupts
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE = 1;


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

    __builtin_enable_interrupts();    // step 7: enable interrupts

    while (1) {
        ;
    }
    return 0;
}