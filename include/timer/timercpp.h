#ifndef TIMERCPP_H
#define TIMERCPP_H

#include <iostream>
#include <thread>
#include <chrono>


class Timer {
    public:
        bool stop = false;
        bool destroy = false;
        void setTimeout(auto function, int delay);
        void setInterval(auto function, int interval);
};

void Timer::setTimeout(auto function, int delay) {
    this->stop = false;
    std::thread t([=]() {
        if(this->stop) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        if(this->stop) return;
        function();
    });
    t.detach();
}

void Timer::setInterval(auto function, int interval) {
    this->stop = false;
    this->destroy = false;
    std::thread t([=]() {
        while(true) {
            if(this->stop || this->destroy) {
                if (this->destroy) break;
                return;
            }
            std::this_thread::sleep_for(std::chrono::nanoseconds(interval));
            //if(this->stop || this->destroy) return;
            while (this->stop || this->destroy) {
                if (this->destroy) break;
            }
            function();
        }
    });
    t.detach();
}



#endif
