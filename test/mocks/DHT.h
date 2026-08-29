#pragma once
#include <cmath>
#include <cstdint>

#include "testdouble.h"

#define DHT22 22

class DHT {
public:
  DHT(uint8_t, uint8_t) {}

  bool begin() { return true; }

  bool read(bool = false) { return true; }

  float readHumidity() {
    if (testdouble::g_dhtHumidity.empty()) {
      return NAN;
    }
    float value = testdouble::g_dhtHumidity.front();
    testdouble::g_dhtHumidity.erase(testdouble::g_dhtHumidity.begin());
    return value;
  }

  float readTemperature() {
    if (testdouble::g_dhtTemp.empty()) {
      return NAN;
    }
    float value = testdouble::g_dhtTemp.front();
    testdouble::g_dhtTemp.erase(testdouble::g_dhtTemp.begin());
    return value;
  }
};