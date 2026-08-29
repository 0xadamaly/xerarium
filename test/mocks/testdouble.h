#pragma once
#include <time.h>

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace testdouble {

struct GpioWrite {
  int pin;
  int value;
};

struct RgbWrite {
  int pin;
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

inline unsigned long g_millis = 0;

inline long long g_sunrise = 0;
inline long long g_sunset = 0;
inline int g_sunCalculations = 0;

inline bool g_configTimeCalled = false;

inline std::vector<GpioWrite> g_pinModes;
inline std::vector<GpioWrite> g_digitalWrites;
inline std::vector<RgbWrite> g_neoWrites;
inline std::string g_serial;

inline int g_zeroCrossStatus = 0;
inline int g_dimmerChannelCount = 0;
inline std::vector<uint32_t> g_dimmerLevels;
inline std::vector<std::pair<int, uint32_t>> g_dimmerLevelWrites;

inline int g_dallasRequests = 0;
inline std::vector<float> g_dallasReads;

inline std::vector<float> g_dhtHumidity;
inline std::vector<float> g_dhtTemp;

inline void resetAll() {
  g_epoch = 0;
  g_sunrise = 0;
  g_sunset = 0;
  g_sunCalculations = 0;
  g_configTimeCalled = false;
  g_pinModes.clear();
  g_digitalWrites.clear();
  g_neoWrites.clear();
  g_serial.clear();
  g_zeroCrossStatus = 0;
  g_dimmerChannelCount = 0;
  g_dimmerLevels.clear();
  g_dimmerLevelWrites.clear();
  g_dallasRequests = 0;
  g_dallasReads.clear();
  g_dhtHumidity.clear();
  g_dhtTemp.clear();
}

inline void advanceMillis(unsigned long ms) { g_millis += ms; }

inline void queueDallasReads(float hot, float cold) {
  g_dallasReads.push_back(hot);
  g_dallasReads.push_back(cold);
}

inline void queueDhtRead(float humidity, float temperature) {
  g_dhtHumidity.push_back(humidity);
  g_dhtTemp.push_back(temperature);
}

} // namespace testdouble