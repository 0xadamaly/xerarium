#include "ClimateManager.h"

#include <Arduino.h>
#include <PID_v1.h>
#include <cstdint>

#include "LampManager.h"
#include "SensorManager.h"
#include "TimeManager.h"

namespace ClimateManager {
namespace {
constexpr int sampleTime = 500;
constexpr double lowerLimit = 0.0, upperLimit = 100.0;
double targetTemp = 32.0;
double inputTemp = 25.0;
double Kp = 7.5, Ki = 0.1, Kd = 2.5; // gains
double outputLampLevel = 0.0;
PID pid(&inputTemp, &outputLampLevel, &targetTemp, Kp, Ki, Kd, DIRECT);
unsigned long lastUpdate = 0;

bool isManualMode = false;

void handleDaylightLamp() {
  if (isManualMode) {
    return;
  }
  bool lightOn = TimeManager::isLightOn();
  if (LampManager::getDaylightLampState() != lightOn) {
    LampManager::setDaylightLampState(lightOn);
  }
}

} // namespace

void begin() {
  pid.SetMode(AUTOMATIC);
  // limits based on dimmer
  pid.SetOutputLimits(lowerLimit, upperLimit);
  pid.SetSampleTime(sampleTime);
}

void setTargetTemp(double temp) { targetTemp = temp; }
double getTargetTemp() { return targetTemp; }

void setManualModeState(bool state) { isManualMode = state; }
bool getManualModeState() { return isManualMode; }

void update() {
  unsigned long now = millis();

  if (isManualMode || (now - lastUpdate) < sampleTime) {
    return;
  }

  handleDaylightLamp();
  
  if (!TimeManager::isHeatOn() || SensorManager::isHotProbeError() ||
      SensorManager::isOverheatError()) {
    outputLampLevel = 0.0;
    if (LampManager::getLampViState() || LampManager::getLampIrState()) {
      LampManager::setLampViState(false);
      LampManager::setLampIrState(false);
    }
    return;
  }
  if (!SensorManager::hasValidTemps()) {
    return;
  }

  lastUpdate = now;
  inputTemp = SensorManager::getHotTemp();
  pid.Compute();
  LampManager::setLampViBrightness((uint8_t) outputLampLevel);
  LampManager::setLampIrBrightness((uint8_t) outputLampLevel);
}
} // namespace ClimateManager
