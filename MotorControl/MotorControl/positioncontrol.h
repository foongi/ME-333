#ifndef POSITIONCONTROL__H__
#define POSITIONCONTROL__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro

#include "NU32DIP.h"
#include "ina219.h"
#include "utilities.h"
#include "encoder.h"
#include "currentcontrol.h"
#include <stdio.h>
#include <stdlib.h>

#define EINTPOSMAX 250


// Set reference array values for tracking and plotting
void set_ref_array(volatile int *ref);
void set_ref_elems(int elems);

void set_ref_count(int count); // Set reference angle for HOLD

// PID control gains set and get
int get_pos_p_gain();
int get_pos_i_gain();
int get_pos_d_gain();
void set_pos_gains(int p, int i, int d);

// Send position data back to client
void send_pos_data();

// Set current reference (in current control.h)
void set_current_ref(int ref);

// Startup for position control
void position_startup();

#endif // POSITIONCONTROL__H__
