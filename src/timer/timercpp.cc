#include "timer/timercpp.h"

void Timer::stopLruTimer() {
    this->clear = true;
}

void Timer::startLruTimer() {
    this->clear = false;
}