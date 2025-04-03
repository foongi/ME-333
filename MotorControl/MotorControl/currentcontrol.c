// Owns timer to implement fixed-freq control loop

#include "currentcontrol.h"

volatile int duty_cycle;
volatile int p_gain;
volatile int i_gain;
volatile int cur_ref;

static volatile int Eint;
static int u = 0;
static int unew = 0;
static int e = 0;

static volatile int ADCarray[PLOTPTS];  // measured values to plot
static volatile int REFarray[PLOTPTS];  // reference values to plot

// Get and set fns:
int get_duty_cycle() {
    return duty_cycle;
}

void set_duty_cycle(int d) {
    
    if(d > 100)
    {
        d = 100;
    } else if (d < -100) {
        d = -100;
    }

    duty_cycle = d;
}

int get_p_gain() {
    return p_gain;
}

int get_i_gain() {
    return i_gain;
}

void set_gains(int p, int i) {
    p_gain = p;
    i_gain = i;
}

void set_current_ref(int ref) {
    cur_ref = ref;
}

void send_data() {
    
    char message[100];

    sprintf(message, "%d\r\n", PLOTPTS); // send initial message of how many points to plot
    NU32DIP_WriteUART1(message);

    for (int i = 0; i<PLOTPTS; i++) {
        sprintf(message, "%d %d\r\n", REFarray[i], ADCarray[i]); // send reference and actual data
        NU32DIP_WriteUART1(message);
    }
}


// Timer 4: 5kHz current control ISR
// Timer 3: PWM OC1RS on pin B15 - 20kHz - prio 5


void __ISR(_TIMER_4_VECTOR, IPL5SOFT) Current_Controller(void) {

    static int count = 0;
    

    // OC1RS = 600; // PWM duty cycle 25% 

    // LATBINV = 0b100000000000000; // Invert B14
    
    switch (get_mode()) 
    {
        case IDLE:
        {
            OC1RS = 0; //Brake
            LATBbits.LATB14 = 0; // set direction to 0 (doesn't matter).

            break;
        }
        case PWM:
        {
            if (duty_cycle >= 0)
            {
                OC1RS = duty_cycle * 24;
                LATBbits.LATB14 = 0; // set direction to 0 (fwd)
            }
            else 
            {
                OC1RS = duty_cycle * -24;
                LATBbits.LATB14 = 1; // set direction to 1 (back)
            }
            break;
        }
        case ITEST:
        {
            

            if((count != 0) && (count % 25 == 0)) // every 25 cycles, switch from 200mA to -200mA
            {
                cur_ref = -cur_ref;
            }

            float current = INA219_read_current();

            

            e = cur_ref - (int) current; 
            Eint = Eint + e; // discrete integral is (running sum)
            u = p_gain * e + i_gain * Eint; // multiply kp by error, ki by its integral

            

            unew = u /1000; // centered on 0, divide by 1000 to add precision
            if (unew > 2399) {
                unew = 2399;
            } else if (unew < -2399) {
                unew = -2399;
            } // caps at 2400 and -2400

            if(unew >= 0) {
                OC1RS = unew;
                LATBbits.LATB14 = 0; // set direction to 0 (fwd)
            } else {
                OC1RS = -unew;
                LATBbits.LATB14 = 1; // set direction to 1 (back)
            }

            if (Eint * i_gain > EINTMAX) { // max for Eint
                Eint = EINTMAX/i_gain;
            } else if (Eint * i_gain < -EINTMAX) {
                Eint = -EINTMAX/i_gain;
            }

            ADCarray[count] = (int) current; // set actual current
            REFarray[count] = cur_ref; // set reference

            count++;

            if (count == PLOTPTS) // when count is 100, we reset 
            {
                count = 0;
                Eint = 0;
                cur_ref = 200;
                set_mode(IDLE);
            }

            
        }
        case HOLD:
        {
            float current = INA219_read_current();

            e = cur_ref - (int) current; 
            Eint = Eint + e; // discrete integral is (running sum)
            u = p_gain * e + i_gain * Eint; // multiply kp by error, ki by its integral

            unew = u /1000; // centered on 0, divide by 1000 to add precision
            if (unew > 2399) {
                unew = 2399;
            } else if (unew < -2399) {
                unew = -2399;
            } // caps at 2400 and -2400

            if(unew >= 0) {
                OC1RS = unew;
                LATBbits.LATB14 = 0; // set direction to 0 (fwd)
            } else {
                OC1RS = -unew;
                LATBbits.LATB14 = 1; // set direction to 1 (back)
            }

            if (Eint * i_gain > EINTMAX) {
                Eint = EINTMAX/i_gain;
            } else if (Eint * i_gain < -EINTMAX) {
                Eint = -EINTMAX/i_gain;
            }


        }
        case TRACK:
        {
            float current = INA219_read_current();

            e = cur_ref - (int) current; 
            Eint = Eint + e; // discrete integral is (running sum)

            if (Eint * i_gain > EINTMAX) {
                Eint = EINTMAX/i_gain;
            } else if (Eint * i_gain < -EINTMAX) {
                Eint = -EINTMAX/i_gain;
            }

            
            u = p_gain * e + i_gain * Eint; // multiply kp by error, ki by its integral

            unew = u /1000; // centered on 0, divide by 1000 to add precision
            if (unew > 2399) {
                unew = 2399;
            } else if (unew < -2399) {
                unew = -2399;
            } // caps at 2400 and -2400

            if(unew >= 0) {
                OC1RS = unew;
                LATBbits.LATB14 = 0; // set direction to 0 (fwd)
            } else {
                OC1RS = -unew;
                LATBbits.LATB14 = 1; // set direction to 1 (back)
            }

            


        }
    }

   
    // clear interrupt flag
    IFS0bits.T4IF = 0;
}


void current_startup()
{
    __builtin_disable_interrupts();

    // Set pin B14 for Output 
    TRISBbits.TRISB14 = 0; // B14 is phase output
    PHASE_BIT = 0;

    // 5kHz interrupt:
    T4CONbits.TCKPS = 0; // prescaler 1:1
    PR4 = 9599; // 9600 ticks
    TMR4 = 0;

    // step 3: T4 turn on
    T4CONbits.ON = 1;
    
    // step 4: set INT1 to priority 5 subpriority 0  
    IPC4bits.T4IP = 5; 
    IPC4bits.T4IS = 0; 

    // step 5: clear INT1 flag status
    // step 6: enable T4 interrupts
    IFS0bits.T4IF = 0;
    IEC0bits.T4IE = 1;


    // 20kHz
    RPB15Rbits.RPB15R = 0b0101; // set B15 to be OC1
    T3CONbits.TCKPS = 0;        // Timer3 prescaler 1:1
    PR3 = 2399;                 // Period = (PR3 +1) * N * 20.83ns 
    TMR3 = 0;                   // Initial timer 3 count is 0
    OC1CONbits.OCTSEL = 1;      // Use timer 3
    OC1CONbits.OCM = 0b110;     // PWM mode without fault pin;
    OC1RS = 0;                  // Initialize duty cycle to 0%
    OC1R = 0;                   // 
    T3CONbits.ON = 1;           // turn on Timer3
    OC1CONbits.ON = 1;          // turn on OC1


    __builtin_enable_interrupts();
}


// Output compare

// Timer to implement PWM to h bridge

// Digital output controlling motor direction

// Controls PIC32 mode IDLE, PWM, Hold, Track, or ITEST