#include "timer/timercpp.h"

void Timer::stopLruTimer() {
    this->stop = true;
}

void Timer::startLruTimer() {
    this->stop = false;
}