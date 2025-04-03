// Owns timer to implement fixed-freq control loop

#include "positioncontrol.h"

// PID gains
volatile int pos_p_gain;
volatile int pos_i_gain;
volatile int pos_d_gain;

// variables for holding at an angle
volatile int ref_angle_count;
volatile int encoder_count;

// variables for position control PID
static volatile int eint;
static volatile int edot;
static volatile int eprev;
static int u = 0;
static int unew = 0;
static int e = 0;

static volatile int count;

// variables for position control plotting
static volatile int *ref_array;
static volatile int actual_array[5000];  
static volatile int num_ref_elems;

// Get and set fns:

void set_ref_array(volatile int *ref) {
    ref_array = ref; // setting reference array
}

void set_ref_elems(int elems) {
    num_ref_elems = elems; // set amount of reference elements 
}

void set_ref_count(int count) {
    ref_angle_count = count; // set reference angle
}

// PID get and set fns
int get_pos_p_gain() {
    return pos_p_gain;
}

int get_pos_i_gain() {
    return pos_i_gain;
}

int get_pos_d_gain() {
    return pos_d_gain;
}
void set_pos_gains(int p, int i, int d) {
    pos_p_gain = p;
    pos_i_gain = i;
    pos_d_gain = d;
}


// Send position data for plotting
void send_pos_data() {
    
    char message[100];

    sprintf(message, "%d\r\n", num_ref_elems); // send amount of data points to plot
    NU32DIP_WriteUART1(message);

    for (int i = 0; i<num_ref_elems; i++) {
        sprintf(message, "%d %d\r\n", ref_array[i], actual_array[i]); // send reference data and actual position data
        NU32DIP_WriteUART1(message);
    }
}


// Timer 4: 5kHz current control ISR
// Timer 3: PWM OC1RS on pin B15 - 20kHz - prio 5


void __ISR(_TIMER_2_VECTOR, IPL6SOFT) Position_Controller(void) {

    int mode = get_mode();

    if(mode == HOLD)
    {

        // Read actual encoder value
        WriteUART2("a");
        while(!get_encoder_flag()){}
        set_encoder_flag(0);
        encoder_count = get_encoder_count();

        // PID control
        e = ref_angle_count - encoder_count; // calculate error
        edot = e - eprev; // error difference (derivative)
        eint = eint + e; // error running sum

        u = pos_p_gain * e + pos_i_gain * eint + pos_d_gain * edot; 

       

        u = u/100; // for extra precision
        

        if (eint > EINTPOSMAX) { // cap eint with EINTPOSMAX
            eint = EINTPOSMAX;
        } else if (eint < -EINTPOSMAX) {
            eint = -EINTPOSMAX;
        }
        
        // Set the reference current for current control
        set_current_ref(u); 
        eprev = e;

    } else if (mode == TRACK)
    {

        // Read actual encoder value
        WriteUART2("a");
        while(!get_encoder_flag()){}
        set_encoder_flag(0);
        encoder_count = get_encoder_count();
        actual_array[count] = (int) (encoder_count * 360.0/1336); // store actual array data as angle
        

        e = (int) ((ref_array[count]* 1336.0/360) - encoder_count); // calculate error in encoder counts

        // PID control
        edot = e - eprev; // error difference (derivative)
        eint = eint + e; // error running sum

        if (eint > EINTPOSMAX) {
            eint = EINTPOSMAX;
        } else if (eint < -EINTPOSMAX) {
            eint = -EINTPOSMAX;
        }

        u = pos_p_gain * e + pos_i_gain * eint + pos_d_gain * edot; 

        u = u/100; // for extra precision
        
        set_current_ref(u); 
        eprev = e;

        count++;

        if (count == num_ref_elems) // When we hit the number of reference data points, stop
        {
            set_ref_count(ref_array[count - 1]);
            count = 0;
            eint = 0;
            set_mode(HOLD);
            
        }

        

    }

    


    // clear interrupt flag
    IFS0bits.T2IF = 0;
}


void position_startup()
{
    __builtin_disable_interrupts();

    count = 0;

    // Timer to implement position PID

    // 200 Hz interrupt:
    T2CONbits.TCKPS = 2; // prescaler 1:4
    PR2 = 59999; // 60000 ticks
    TMR2 = 0;

    // step 3: T2 turn on
    T2CONbits.ON = 1;
    
    // step 4: set INT1 to priority 5 subpriority 0  
    IPC2bits.T2IP = 6;
    IPC2bits.T2IS = 0; 

    // step 5: clear INT1 flag status
    // step 6: enable T2 interrupts
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE = 1;

    eprev = 0;
    __builtin_enable_interrupts();
}
