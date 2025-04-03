#include "utilities.h"
// 1 variable holding the operating mode
volatile enum mode_t mode;

// Get and set fns:
enum mode_t get_mode() {
    return mode;
}

void set_mode(enum mode_t m) {
    mode = m;
}