#pragma once
#include <cstdint>

#include "OneWire.h"
#include "testdouble.h"

inline const float DEVICE_DISCONNECTED_C = -127.0f;

class DallasTemperature {
public:
  explicit DallasTemperature(OneWire *) {}

  void begin() {}

  void setWaitForConversion(bool) {}

  void requestTemperatures() { testdouble::g_dallasRequests++; }

  float getTempCByIndex(uint8_t) {
    if (testdouble::g_dallasReads.empty()) {
      return DEVICE_DISCONNECTED_C;
    }
    float value = testdouble::g_dallasReads.front();
    testdouble::g_dallasReads.erase(testdouble::g_dallasReads.begin());
    return value;
  }
};