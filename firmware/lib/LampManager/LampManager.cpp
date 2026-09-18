#include "LampManager.h"
#include <rbdimmerESP32.h>

namespace LampManager {
namespace {

constexpr uint8_t DIM_VI_PIN = 9;
constexpr uint8_t DIM_IR_PIN = 10;
constexpr uint8_t ZC_PIN = 11;
constexpr uint8_t RELAY_PIN = 12;

rbdimmer_channel_t *dimmer_vi = nullptr;
rbdimmer_channel_t *dimmer_ir = nullptr;

uint8_t lampViLevel = 0;
uint8_t lampIrLevel = 0;
uint8_t lampViLastLevel = 100;
uint8_t lampIrLastLevel = 100;
bool daylightLampOn = false;
bool zeroCrossError = false;

} // namespace

void begin() {
  rbdimmer_init();
  rbdimmer_err_t err = rbdimmer_register_zero_cross(ZC_PIN, 0, 0);
  if (err == RBDIMMER_OK) {
    Serial.printf("Zero-cross detector registered on pin %d\n", ZC_PIN);
  } else {
    zeroCrossError = true;
  }

  rbdimmer_config_t config_vi = {.gpio_pin = DIM_VI_PIN,
                                 .phase = 0,
                                 .initial_level = 0,
                                 .curve_type = RBDIMMER_CURVE_RMS};
  rbdimmer_create_channel(&config_vi, &dimmer_vi);

  rbdimmer_config_t config_ir = {.gpio_pin = DIM_IR_PIN,
                                 .phase = 0,
                                 .initial_level = 0,
                                 .curve_type = RBDIMMER_CURVE_RMS};
  rbdimmer_create_channel(&config_ir, &dimmer_ir);

  setLampViBrightness(0);
  setLampIrBrightness(0);

  pinMode(RELAY_PIN, OUTPUT);
  setDaylightLampState(false);
}

void setLampViBrightness(uint8_t value) {
  if (value > 100) {
    value = 100;
  }
  if (value > 0) {
    lampViLastLevel = value;
  }
  lampViLevel = value;
  rbdimmer_set_level(dimmer_vi, lampViLevel);
}

void setLampIrBrightness(uint8_t value) {
  if (value > 100) {
    value = 100;
  }
  if (value > 0) {
    lampIrLastLevel = value;
  }
  lampIrLevel = value;
  rbdimmer_set_level(dimmer_ir, lampIrLevel);
}

void setLampViState(bool state) {
  setLampViBrightness(state ? lampViLastLevel : 0);
}

void setLampIrState(bool state) {
  setLampIrBrightness(state ? lampIrLastLevel : 0);
}

void setDaylightLampState(bool state) {
  daylightLampOn = state;
  digitalWrite(RELAY_PIN, daylightLampOn ? HIGH : LOW);
}

uint8_t getLampViBrightness() { return lampViLevel; }

uint8_t getLampIrBrightness() { return lampIrLevel; }

bool getLampViState() { return lampViLevel > 0; }

bool getLampIrState() { return lampIrLevel > 0; }

bool getDaylightLampState() { return daylightLampOn; }

bool isZeroCrossError() { return zeroCrossError; }

} // namespace LampManager
