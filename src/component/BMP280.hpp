#pragma once
#include <avr/io.h>

#include "util/spi.hpp"

class BMP280 {
    public:
        BMP280(uint8_t pin);
        void init();
        bool isUsingInput() const;
        bool isReady();
        void calibrateTemperature();
        void readTemperature();
        void calculateTemperature();
        float getTemperature() const;
        void calibratePressure();
        void readPressure();
        void calculatePressure();
        float getPressure() const;

    protected:
        uint8_t pin; // CSB
        bool usingInput = false;

        // temperature
        uint16_t calT1;
        int16_t calT[2]; // calT2, calT3, maar in een array
        int32_t adcT = 0;
        int32_t fineT;
        float temperature = -999;

        // pressure
        uint16_t calP1;
        int16_t calP[8]; // calP2, calP3, calP4, .., maar in een array
        int32_t adcP = 0;
        float pressure = -999;

        void beginMeasure();
        void endMeasure();
};
