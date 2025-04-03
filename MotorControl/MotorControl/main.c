#include "nu32dip.h"
#include "encoder.h"
#include "ina219.h"
#include "utilities.h"
#include "currentcontrol.h"
#include "positioncontrol.h"

// other header files

#define BUF_SIZE 200
static volatile int trajectory_ref[5000]; // in encoder counts


int main(void) {
    // Initialize other models, initialize PIC 32
    __builtin_disable_interrupts();

    // Complete startup for different controllers
    char buffer[BUF_SIZE];
    NU32DIP_Startup();
    UART2_Startup();
    INA219_Startup();
    
    current_startup();
    position_startup();

    // Set inital gains
    set_gains(2500, 1000);
    set_pos_gains(3500, 0, 3500);

    set_ref_array(trajectory_ref); // set the reference array in position control to the one we have stored.

    // Set mode to IDLE
    set_mode(IDLE);

    NU32DIP_GREEN = 1;
    NU32DIP_YELLOW = 1;

    __builtin_enable_interrupts();

    // Infininite loop for commands with client
    while(1)
    {
        NU32DIP_ReadUART1(buffer, BUF_SIZE);    // expect next char 
                                                // to be menu cmd
        NU32DIP_YELLOW = 1;                     // Clearing error LED
        switch (buffer[0]) {
            case 'a': // Read current
            {
                char m[50];
                float current = INA219_read_current(); // read current

                sprintf(m, "%lf\r\n", current); 
                NU32DIP_WriteUART1(m); // print it
                break;
            }
            case 'c': // Get encoder count
            {
                WriteUART2("a");                // write a request for encoder count
                while(!get_encoder_flag()){}    // wait for reply
                set_encoder_flag(0);            // set flag back to 0
                char m[50];
                int p = get_encoder_count();    // get encoder count
                sprintf(m, "%d\r\n", p);
                NU32DIP_WriteUART1(m); // send to client
                break;
            }
            case 'd': // read encoder angle
            {
                WriteUART2("a");                // write a request for encoder count
                while(!get_encoder_flag()){}    // wait for reply
                set_encoder_flag(0);            // set flag back to 0
                char m[50];
                int p = get_encoder_count();
                double angle = p/1336.0 * 360.0;// convert from encoder count to angle

                sprintf(m, "%lf\r\n", angle);
                NU32DIP_WriteUART1(m);          // send back to client
                break;
            }
            case 'e': // reset encoder
            {
                WriteUART2("b"); // send rpi "b" to reset encoder
                break;
            }
            case 'f': // set PWM
            {
                int pwm = 0;
                NU32DIP_ReadUART1(buffer, BUF_SIZE); // read pwm request
                sscanf(buffer, "%d", &pwm);

                set_duty_cycle(pwm); // set duty cycle in current controller
                set_mode(PWM);
                break;
            }
            case 'g': // set current gains
            {
                int p_gain = 0;
                int i_gain = 0;
                NU32DIP_ReadUART1(buffer, BUF_SIZE); 
                sscanf(buffer, "%d %d", &p_gain, &i_gain); // read gains on uart
                
                set_gains(p_gain, i_gain); // update gains
                break;
            }
            case 'h': // get current gains
            {
                char m[75];

                // retrieve gains from current control
                int p_gain = get_p_gain();
                int i_gain = get_i_gain();
                sprintf(m, "Proportional Gain: %d Integral Gain: %d\r\n", p_gain, i_gain);
                NU32DIP_WriteUART1(m); // send gains to client
                break;
            }
            case 'i': // set position gains
            {
                int p_gain = 0;
                int i_gain = 0;
                int d_gain = 0;
                NU32DIP_ReadUART1(buffer, BUF_SIZE);
                sscanf(buffer, "%d %d %d", &p_gain, &i_gain, &d_gain); // recieve gain requests from client
                set_pos_gains(p_gain, i_gain, d_gain); // set position gains in position control
                break;

            }
            case 'j': // get position gains
            {
                char m[100];

                // request gains from position controller
                int p_gain = get_pos_p_gain();
                int i_gain = get_pos_i_gain();
                int d_gain = get_pos_d_gain();

                sprintf(m, "Proportional Gain: %d, Integral Gain: %d, Derivative Gain: %d\r\n", p_gain, i_gain, d_gain);
                NU32DIP_WriteUART1(m); // create message of gains, send back to client
                break;
            }
            case 'k': // test current gains
            {
                set_current_ref(200); // set current reference to 200mA initially

                set_mode(ITEST);                // start current test
                while(get_mode() == ITEST) {    // wait for current test to end
                    ;
                }
                send_data(); // send data back to client 
                break;

            }
            case 'l': // set angle to go to
            {

                float angle = 0;
                NU32DIP_ReadUART1(buffer, BUF_SIZE);
                sscanf(buffer, "%f", &angle);           // read requst from client
                
                int encoder_count = angle * 1336.0/360.0;   // convert rerquest to encoder counts
                set_ref_count(encoder_count);               // set encoder counts as reference angle
                set_mode(HOLD);                             // set mode of position controller to HOLD

                break;

            }
            case 'm': // read step trajectory
            {
                set_mode(IDLE); // Stop motor
                int num_samps = 0;

                NU32DIP_ReadUART1(buffer, BUF_SIZE);
                sscanf(buffer, "%d", &num_samps);               // read amount of data points
                set_ref_elems(num_samps);                       // set amount of data points
                int i = 0;
                for (i = 0; i < num_samps; i++) {
                    NU32DIP_ReadUART1(buffer, BUF_SIZE);
                    sscanf(buffer, "%d", &trajectory_ref[i]);   // scan in data to trajectory reference array
                }
                break;
            }
            case 'n': // read cubic trajectory
            {
                set_mode(IDLE); // Stop motor

                int num_samps = 0;

                NU32DIP_ReadUART1(buffer, BUF_SIZE);
                sscanf(buffer, "%d", &num_samps);               // read amount of data points
                set_ref_elems(num_samps);                       // set amount of data points
                int i = 0;
                for (i = 0; i < num_samps; i++) {
                    NU32DIP_ReadUART1(buffer, BUF_SIZE);
                    sscanf(buffer, "%d", &trajectory_ref[i]);   // scan in data to trajectory reference array
                }
                break;

            }
            case 'o': // execute trajectory
            {
                WriteUART2("b");    // tell rpi to reset encoder counts
                set_mode(TRACK);    // start tracking reference trajectory
                while(get_mode() == TRACK) { // wait for finish
                    ;
                }
                send_pos_data();    // send position data back for plotting
                break;



            }
            case 'p':
            {
                set_mode(IDLE); // power motor off
                break;
            }
            case 'r': // get mode
            {
                char m[50];

                enum mode_t mode = get_mode();
                sprintf(m, "%d\r\n", mode);
                NU32DIP_WriteUART1(m); // get mode, send it as an int back
                break;
            }
            case 'q':
            {
                // handle q for quit, return to idle mode
                set_mode(IDLE);
                break;
            }
            default:
            {
                NU32DIP_YELLOW = 0; // Turn on LED2 to indicate error
                break;
            }

        }
    }
    return 0;

}