#ifndef SR04__H__
#define SR04__H__

#include "nu32dip.h"
#define TIMEOUT 4000000000


void SR04_Startup();
unsigned int SR04_read_raw(unsigned int);
float SR04_read_meters();

#endif // SR04__H__