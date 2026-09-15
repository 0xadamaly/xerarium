#pragma once
#include <cstdint>

namespace ClimateManager {
void begin();
void setTargetTemp(double temp);
double getTargetTemp(); 

void setManualModeState(bool state);
bool getManualModeState();

void update();
}

