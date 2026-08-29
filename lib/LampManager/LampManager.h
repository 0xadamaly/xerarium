#pragma once
#include <Arduino.h>
#include <cstdint>

namespace LampManager {

void begin();

void setLampViBrightness(uint8_t value);
void setLampIrBrightness(uint8_t value);

void setLampViState(bool state);
void setLampIrState(bool state);
void setDaylightLampState(bool state);

uint8_t getLampViBrightness();
uint8_t getLampIrBrightness();
bool getLampViState();
bool getLampIrState();
bool getDaylightLampState();

bool isZeroCrossError();

} // namespace LampManager
