#include <avr/interrupt.h>

void setupTimer1Interrupt(float const &hz) { // klopt bij 8MHz
    cli();

    TCCR1 = 0; // reset timer register
    TCNT1 = 0; // reset counter

    TCCR1 |= (1 << CTC1); // zet CTC mode aan
    TCCR1 |= (1 << CS10) | (1 << CS11) | (1 << CS12) | (1 << CS13); // zet 16384 prescaler

    OCR1A = F_CPU / 16384 / hz - 1;
    OCR1C = OCR1A;

    TIMSK |= (1 << OCIE1A); // activeer interrupt

    sei();
}

void interruptTimer1();

ISR(TIM1_COMPA_vect) {
    interruptTimer1();
}
