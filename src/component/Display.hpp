#pragma once
#include <avr/io.h>

#include "util/spi.hpp"
#include "util/math.hpp"

class Display {
    public:
        Display(uint8_t pin);
        void show(double number, uint8_t const &decimals);
        void show(int const &number);
        void clear();
        void extra(uint8_t const &data);
        void task();

    protected:
        static const uint8_t DIGITS = 5;
        static const uint8_t CHARACTERS[];

        uint8_t segments[DIGITS];
        uint8_t pin; // RCLK

        uint8_t dotPosition = 0;
        uint8_t lastPosition = 0;
        uint8_t extraData = 0;

        uint8_t getShiftedNumber(uint8_t const &digit, bool const &dot) const;
        void saveDigit(uint8_t const &digit, uint8_t const &position);
        void writeDigit(uint8_t const &port) const;
};
