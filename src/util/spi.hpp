#pragma once
#include <util/atomic.h>

void spiInit();
uint8_t spiTransfer(uint8_t const &data);
int32_t spiRead();
