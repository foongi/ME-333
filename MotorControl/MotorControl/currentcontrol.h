#ifndef CURRENTCONTROL__H__
#define CURRENTCONTROL__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro

#include "NU32DIP.h"
#include "ina219.h"
#include "utilities.h"

#define PHASE_BIT LATBbits.LATB14
#define PLOTPTS 100
#define EINTMAX 2400000


// send/recieve fixed pwm command in range [-100, 100]
int get_duty_cycle();
void set_duty_cycle(int d); 

// Recieve current control gains
int get_p_gain();
int get_i_gain();

// Provide current control gains.
void set_gains(int p, int i);
void send_data();

// startup fn
void current_startup();

#endif // CURRENTCONTROL__H__


