#include <avr/interrupt.h>

void setupTimer0Interrupt(uint8_t const &scale) {
    cli();

    TCCR0A = 0; // reset timer register
    TCCR0B = 0;
    TCNT0 = 0; // reset counter

    TCCR0B |= scale; // zet prescaler
    TIMSK |= (1 << TOIE0); // activeer interrupt

    sei();
}

void interruptTimer0();

ISR(TIMER0_OVF_vect) {
    interruptTimer0();
}
