#include "sr04.h"

/******************************************************************************
 * MAIN FUNCTION   ==SecA.4.2==
 ******************************************************************************/

int main(void) {

    NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
    SR04_Startup();

    char throwaway[100], message[100];
    NU32DIP_ReadUART1(throwaway, 100); // first one to verify user has connected to the port.

    while(1) {   
        float meters = SR04_read_meters();
        sprintf(message, "Distance: %f meters \r\n", meters);
        NU32DIP_WriteUART1(message);
        

        unsigned int init = _CP0_GET_COUNT();
        while(_CP0_GET_COUNT() <= init + 10000000)
        {
            _nop();
        }


    }
    return 0;
} // ***** END main *****