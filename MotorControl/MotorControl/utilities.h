#ifndef utilities__H__
#define utilities__H__

enum mode_t {
    IDLE,
    PWM,
    ITEST,
    HOLD,
    TRACK
};

// get operating mode
enum mode_t get_mode();

// set operating mode
void set_mode(enum mode_t m);

#endif