#include "math.hpp"

uint16_t unsignedPowerOfTen(uint8_t const &y) {
    uint16_t result = 1;
    for (uint8_t i = 0; i < y; i++) {
        result *= 10;
    }

    return result;
}
