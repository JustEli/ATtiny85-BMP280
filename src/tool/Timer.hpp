#pragma once
#include <avr/io.h>

class Timer {
    public:
        Timer(uint16_t duration);
        void task();
        bool check();

    protected:
        uint32_t time = 0;
        uint16_t duration;
};
