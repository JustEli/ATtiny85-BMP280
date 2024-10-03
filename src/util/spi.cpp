#include "spi.hpp"

void spiInit() {
    DDRB &= ~(1 << PB0); // pinMode(PB0, INPUT); MISO (master in, slave out; MOSI in datasheet)
    DDRB |= (1 << PB1); // pinMode(PB1, OUTPUT); MOSI (master out, slave in; MISO in datasheet)
    DDRB |= (1 << PB2); // pinMode(PB2, OUTPUT); SCL (clock)

    USICR |= (1 << USIWM0) | (1 << USICS1); // USIWM0 op 1; enable 3-wire mode
    USICR |= (1 << USICLK); // stel spi 4-bit (= 16 keer) counter klok in
}

uint8_t spiTransfer(uint8_t const &data) {
    USIDR = data; // zet de gegeven data in het dataregister
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { // niet laten beinvloeden door interrupts
        USISR = 1 << USIOIF; // klok resetten, zodat ie daarna 16 keer kan gaan voordat 'ie weer hoog (1) wordt
        while ((USISR & (1 << USIOIF)) == 0) { // zolang de klok doorgaat, dus het bit op de positie van USIOIF 0 is
            USICR |= (1 << USITC); // toggle de klok
        }

        // // andere manier: de klok moet een aantal keer hoog en laag gezet worden
        // for (uint8_t i = 0; i < sizeof(data) * 16; i++) {
        //     USICR |= (1 << USITC); // toggle de klok
        // }
    }

    return USIDR;
}

int32_t spiRead() {
    return spiTransfer(0);
}
