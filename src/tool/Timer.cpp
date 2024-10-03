#include "Timer.hpp"

// geef de duratie van de timer
Timer::Timer(uint16_t duration): duration(duration) {
}

void Timer::task() {
    time++;
}

// check of de timer verlopen is, en als dat het geval is, wordt de timer ook gereset
bool Timer::check() {
    if (time > duration) {
        time = 0; // reset
        return true;
    }

    return false;
}
