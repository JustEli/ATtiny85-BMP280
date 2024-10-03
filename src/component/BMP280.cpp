#include "BMP280.hpp"

BMP280::BMP280(uint8_t pin): pin(pin) {
    DDRB |= (1 << pin); // pinMode(pin, OUTPUT)
    PORTB |= (1 << pin); // digitalWrite(pin, HIGH); nog niks sturen over SPI
}

void BMP280::init() {
    while (!isReady()); // wachten op sensor

    // zet temperatuurmetingen aan
    beginMeasure();
    spiTransfer(0xF4 & 0b01111111); // 0xF4 -> 0x74, om te schrijven i.p.v. lezen (0b11110100 -> 0b01110100)
    spiTransfer(0b11111111); // temperatuurmeting 16x, luchtdrukmeting 16x, normal mode
    endMeasure();

    calibrateTemperature();
    calibratePressure();
}

bool BMP280::isUsingInput() const {
    return usingInput;
}

bool BMP280::isReady() {
    beginMeasure();
    spiTransfer(0xD0);
    uint8_t ready = spiRead();
    endMeasure();

    return ready == 0x58;
}

void BMP280::calibrateTemperature() {
    // kalibratie
    beginMeasure();
    spiTransfer(0x88);

    //      LSB          MSB
    calT1 = spiRead() | (spiRead() << 8);
    for (int16_t &cal : calT) {
        cal = spiRead() | (spiRead() << 8);
    }

    endMeasure();
}

void BMP280::readTemperature() {
    // data opvragen
    beginMeasure();
    spiTransfer(0xFA);

    adcT = spiRead() << 12; // MSB (8-bit)
    adcT |= spiRead() << 4; // LSB (8-bit)
    adcT |= spiRead() >> 4; // XLSB (4-bit)
    endMeasure();
}

// in *C
void BMP280::calculateTemperature() {
    if (adcT == 0) {
        temperature = -999;
        return;
    }

    int32_t var1 = ((((adcT >> 3) - ((int32_t)calT1 << 1))) * ((int32_t)calT[0])) >> 11;
    int32_t var2 = (((((adcT >> 4) - ((int32_t)calT1)) * ((adcT >> 4) - ((int32_t)calT1))) >> 12) * ((int32_t)calT[1])) >> 14;

    fineT = var1 + var2;
    temperature = fineT / 5120.0;
}

float BMP280::getTemperature() const {
    return temperature;
}

void BMP280::calibratePressure() {
    beginMeasure();
    spiTransfer(0x8E);

    //      LSB            MSB
    calP1 = spiRead() | (spiRead() << 8);
    for (int16_t &cal : calP) {
        cal = spiRead() | (spiRead() << 8);
    }

    endMeasure();
}

void BMP280::readPressure() {
    beginMeasure();
    spiTransfer(0xF7);

    adcP = spiRead() << 12; // MSB (8-bit)
    adcP |= spiRead() << 4; // LSB (8-bit)
    adcP |= spiRead() >> 4; // XLSB (4-bit)
    endMeasure();
}

// in hPa
void BMP280::calculatePressure() {
    if (adcT == 0 || adcP == 0) {
        pressure = -999;
        return;
    }

    int64_t var1 = ((int64_t)fineT) - 128000;
    int64_t var2 = var1 * var1 * (int64_t)calP[4];
    var2 = var2 + ((var1*(int64_t)calP[3])<<17);
    var2 = var2 + (((int64_t)calP[2])<<35);
    var1 = ((var1 * var1 * (int64_t)calP[1])>>8) + ((var1 * (int64_t)calP[0])<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)calP1)>>33;
    if (var1 == 0) return;
    int64_t p = 1048576 - adcP;
    p = (((p<<31)-var2)*3125)/var1;
    var1 = (((int64_t)calP[7]) * (p>>13) * (p>>13)) >> 25;
    var2 = (((int64_t)calP[6]) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)calP[5])<<4);
    pressure = ((uint32_t)p) / 25600.0;
}

float BMP280::getPressure() const {
    return pressure;
}

// communicatie
void BMP280::beginMeasure() {
    usingInput = true;
    PORTB &= ~(1 << pin); // digitalWrite(pin, LOW)
}

void BMP280::endMeasure() {
    PORTB |= (1 << pin); // digitalWrite(pin, HIGH)
    usingInput = false;
}
