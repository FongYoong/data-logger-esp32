#ifndef UNIX_TIME_H
#define UNIX_TIME_H

#include "time.h" // Unix time from NTPServer
#include "Arduino.h"

void timeSetup();
unsigned long getUnixTime();

#endif