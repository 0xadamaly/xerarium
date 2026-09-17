#include "LedManager.h"
#include <Arduino.h>

namespace LedManager {
namespace {

constexpr uint8_t RGB_LED_PIN = 48;
Color color = {};
Color lastColor = {255, 255, 255};
bool isOn = false;

} // namespace

void begin() { pinMode(RGB_LED_PIN, OUTPUT); }

void setColor(uint8_t red, uint8_t green, uint8_t blue) {
  color = {red, green, blue};
  isOn = (color.r > 0 || color.b > 0 || color.g > 0);
  if (isOn) {
    lastColor = color;
  }
  neopixelWrite(RGB_LED_PIN, color.r, color.g, color.b);
}
void setState(bool state) {
  if (state) {
    color = lastColor;
    neopixelWrite(RGB_LED_PIN, color.r, color.g, color.b);
  } else {
    color = {0, 0, 0};
    neopixelWrite(RGB_LED_PIN, color.r, color.g, color.b);
  }
  isOn = state;
}

Color getColor() { return color; }
bool getState() { return isOn; }

} // namespace LedManager
