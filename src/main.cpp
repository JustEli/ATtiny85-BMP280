#include <avr/io.h>

#include "util/boot.hpp"
#include "util/spi.hpp"

#include "util/interrupt/timer0.hpp"
#include "util/interrupt/timer1.hpp"

#include "tool/Timer.hpp"

#include "component/Display.hpp"
#include "component/BMP280.hpp"

Timer timer(150);
Display display(PB3);
BMP280 sensor(PB4);

void buttonHandler();
void displayHandler();

bool buttonStatus = false;
bool temperatureElsePressure = true;
bool recalculate = false;

void setup() {
    spiInit();

    display.clear();
    sensor.init();

    setupTimer0Interrupt(1 << CS01); // prescaler 8
    setupTimer1Interrupt(8); // 1 Hz (voor 1MHz)
}

void repeat() {
    // verwerk sensor data en zet op display
    displayHandler();

    // button interactie
    buttonHandler();
}

// voor scherm en timer; >240Hz
void interruptTimer0() {
    display.task();
    timer.task();
}

// voor sensor
void interruptTimer1() {
    sensor.readTemperature();
    sensor.readPressure();
    recalculate = true;
}

void buttonHandler() {
    if (sensor.isUsingInput()) {
        return; // de input pin van SPI is in gebruik, dus niet uitlezen
    }

    bool current = PINB & (1 << PB0); // digitalRead(PB0)
    if (current != buttonStatus && current && timer.check()) { // als de knop wordt ingedrukt
        temperatureElsePressure = !temperatureElsePressure;
        recalculate = true;
    }

    buttonStatus = current;
}

void displayHandler() {
    if (!recalculate) { // als er geen nieuwe waarde is hoeft er ook niks nieuws
        return;         //  op het display gezet te worden
    }

    sensor.calculateTemperature(); // temperatuur moet altijd berekend worden

    if (temperatureElsePressure) {
        float temperature = sensor.getTemperature();
        if (temperature == -999) {
            return;
        }

        display.show(temperature, 2);
        display.extra(0b10);
    }
    else {
        sensor.calculatePressure();
        float pressure = sensor.getPressure();
        if (pressure == -999) {
            return;
        }

        display.show(pressure, 1);
        display.extra(0b01);
    }

    recalculate = false;
}

// ---------------
// -- BEDRADING --
// ---------------

// ---------------- SPI Shift Register ----------------
// Shift Reg.	ATtiny		  TM (Display) Shift Reg.

// SRCLK (11)   SCK (7/PB2)   SCLK
// RCLK (12)    PB3 (2/PB3)   RCLK
// SER (14)	    DO (6/PB1)    DIO

// OE (13) -> GND
// SRCLR (10) -> VCC
// ----------------------------------------------------


// -------------------- SPI BMP280 --------------------
// BMP280       ATtiny

// SDO          DI (5/PB0)
// SDA          DO (6/PB1)
// SCL          SCK (7/PB2)
// CSB          PB4 (3/PB4)
// ----------------------------------------------------
