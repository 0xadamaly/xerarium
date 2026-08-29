#pragma once
#include <cstdint>
#include <vector>

#include "testdouble.h"

typedef int rbdimmer_err_t;

#define RBDIMMER_OK 0
#define RBDIMMER_CURVE_RMS 1

typedef struct rbdimmer_channel_s rbdimmer_channel_t;

struct rbdimmer_config_t {
  uint8_t gpio_pin;
  uint32_t phase;
  uint32_t initial_level;
  int curve_type;
};

inline rbdimmer_err_t rbdimmer_init() { return RBDIMMER_OK; }

inline rbdimmer_err_t
rbdimmer_register_zero_cross(uint8_t, uint32_t, uint8_t) {
  return testdouble::g_zeroCrossStatus;
}

inline int rbdimmer_channel_index(rbdimmer_channel_t *channel) {
  return (int)((intptr_t)channel - 1);
}

inline rbdimmer_err_t rbdimmer_create_channel(rbdimmer_config_t *config,
                                              rbdimmer_channel_t **out) {
  int idx = testdouble::g_dimmerChannelCount++;
  testdouble::g_dimmerLevels.push_back(config->initial_level);
  *out = (rbdimmer_channel_t *)(intptr_t)(idx + 1);
  return RBDIMMER_OK;
}

inline rbdimmer_err_t rbdimmer_set_level(rbdimmer_channel_t *channel,
                                         uint32_t level) {
  int idx = rbdimmer_channel_index(channel);
  if (idx < 0 || idx >= (int)testdouble::g_dimmerLevels.size()) {
    return -1;
  }
  testdouble::g_dimmerLevels[idx] = level;
  testdouble::g_dimmerLevelWrites.push_back({idx, level});
  return RBDIMMER_OK;
}

inline uint32_t rbdimmer_get_level(rbdimmer_channel_t *channel) {
  int idx = rbdimmer_channel_index(channel);
  if (idx < 0 || idx >= (int)testdouble::g_dimmerLevels.size()) {
    return 0;
  }
  return testdouble::g_dimmerLevels[idx];
}