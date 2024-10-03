#include "Display.hpp"

Display::Display(uint8_t pin): pin(pin) {
    DDRB |= (1 << pin); // pinMode(pin, OUTPUT)
    PORTB |= (1 << pin); // digitalWrite(pin, HIGH); nog niks sturen over SPI
}

void Display::show(double number, uint8_t const &decimals) {
    bool negative = number < 0;
    if (negative) {
        number *= -1; // positief maken
    }

    uint16_t rounded = number * unsignedPowerOfTen(decimals);
    dotPosition = decimals;

    for (uint8_t i = 0; i < DIGITS; i++) {
        if (rounded <= 0 /*nothing left to display*/ && i > dotPosition /*show first 0*/) {
            saveDigit(negative? 11 : 10, i); // 11 = streepje, of 10 = niks
            if (negative) {
                negative = !negative; // max. 1 streepje
            }
            continue;
        }

        saveDigit(rounded % 10, i);
        rounded /= 10;
    }
}

void Display::show(int const &number) {
    show(number, 0);
}

void Display::clear() {
    for (uint8_t i = 0; i < DIGITS; i++) {
        saveDigit(10, i); // 10 = niks
    }
}

// de overige outputs van de shift register gebruiken
void Display::extra(uint8_t const &data) {
    extraData = ~(data << DIGITS);
}

// run dit in een frequentie om het scherm te updaten (60 * DIGITS Hz)
void Display::task() {
    writeDigit(lastPosition);

    if (lastPosition == DIGITS - 1) {
        lastPosition = 0;
        return;
    }

    lastPosition++;
}

const uint8_t Display::CHARACTERS[] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b00000000, // 10   niks
    0b01000000, // 11   streepje
    0b10000000  // 12   punt
};

uint8_t Display::getShiftedNumber(uint8_t const &digit, bool const &dot) const {
    uint8_t result = CHARACTERS[digit];
    return dot? result | CHARACTERS[12] : result;
}

void Display::saveDigit(uint8_t const &digit, uint8_t const &position) {
    segments[position] = getShiftedNumber(
        digit,
        dotPosition != 0 && position == dotPosition
    );
}

void Display::writeDigit(uint8_t const &port) const {
    PORTB &= ~(1 << pin); // digitalWrite(pin, LOW)
    spiTransfer(segments[port]); // digit
    spiTransfer(~(1 << port) & extraData); // positie, incl. extra data
    PORTB |= (1 << pin); // digitalWrite(pin, HIGH)
}
