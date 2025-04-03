#include "nu32dip.h"
#define NUMSAMPS 1000 // number of points in
#define PLOTPTS 200
#define DECIMATION 10


static volatile int Waveform[NUMSAMPS]; // waveform
static volatile int ADCarray[PLOTPTS];  // measured values to plot
static volatile int REFarray[PLOTPTS];  // reference values to plot
static volatile int StoringData = 0;    // if this flag = 1, currently storing
                                        // plot data
static volatile float Kp = 0, Ki = 0;   // control gains

void makeWaveForm();

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) {
    static int counter = 0; // init counter
    static int plotind = 0; // index for data arrays
    static int decctr = 0;  // count to store data one every decimation
    static int adcval = 0;

    OC1RS = Waveform[counter];
    
    if (StoringData) {
        decctr++;
        if (decctr == DECIMATION){
            decctr = 0;
            ADCarray[plotind] = adcval;
            REFarray[plotind] = Waveform[counter];
            plotind++;
        }
        if (plotind == PLOTPTS) {
            plotind = 0;
            StoringData = 0;
        }
    }


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

    char message[100]; // message to and from MATLAB
    float kptemp = 0, kitemp = 0; // temporary local gains
    int i = 0; // plot data loop counter

    while (1) {
        NU32DIP_ReadUART1(message, sizeof(message));
        sscanf(message, "%f %f", &kptemp, &kitemp);
        __builtin_disable_interrupts();
        Kp = kptemp;
        Ki = kitemp;
        __builtin_enable_interrupts(); 
        StoringData = 1;
        while(StoringData) {
            ;
        }
        for (i = 0; i<PLOTPTS; i++) {
            sprintf(message, "%d %d %d\r\n", PLOTPTS-i, ADCarray[i], REFarray[i]);
            NU32DIP_WriteUART1(message);
        }
    }
    return 0;
}