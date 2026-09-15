#pragma once
#include <Arduino.h>

namespace SensorManager {

void begin();
void update();

float getHotTemp();
float getColdTemp();
float getAmbientTemp();
float getAmbientHumidity();

bool hasValidTemps();

void setDallasState(bool state);
void setDhtState(bool state);

bool isHotProbeError();
bool isColdProbeError();
bool isDhtError();
bool isOverheatError();

} // namespace SensorManager
