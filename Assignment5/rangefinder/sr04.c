#include "sr04.h"
#include <xc.h>

// macros for the pins
#define TRIG LATBbits.LATB15
#define ECHO PORTBbits.RB14

// initialize the pins used by the SR04
void SR04_Startup(){
    ANSELA = 0; // turn off the analog input functionality that overrides everything else
    ANSELB = 0;
    TRISBbits.TRISB15 = 0; // B15 is TRIG, output from the PIC
    TRISBbits.TRISB14 = 1; // B14 is ECHO, input to the PIC
    TRIG = 0; // initialize TRIG to LOW
}

// trigger the SR04 and return the value in core timer ticks
unsigned int SR04_read_raw(unsigned int timeout){
    // turn on TRIG 
    TRIG = 1;


    // wait at least 10us
    unsigned int init = _CP0_GET_COUNT();
    while(_CP0_GET_COUNT() <= init + 1000)
    {
        _nop();
    }
    // turn off TRIG
    TRIG = 0;

    // wait until ECHO is on
    while(ECHO == 0)
    {
        _nop();
    }

    // note the value of the core timer
    unsigned int initial = _CP0_GET_COUNT();
    
    // wait until ECHO is off or timeout core ticks has passed
    while(ECHO != 0 && _CP0_GET_COUNT() <= timeout)
    {
        _nop();
    }

    // note the core timer
    unsigned int final = _CP0_GET_COUNT();
    
    // return the difference in core times
    return final - initial;
}

float SR04_read_meters(){
    // read the raw rs04 value
    unsigned int diff_ticks = SR04_read_raw(TIMEOUT);

    // convert the time to meters, the velocity of sound in air is 346 m/s
    float meters = diff_ticks * 25.0/2 * 346.0/1000000000; // Divide by 2 because sound has to go there and back.

    // return the distance in meters
    return meters;
}
