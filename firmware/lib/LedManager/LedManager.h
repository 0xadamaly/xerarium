#pragma once
#include <cstdint>

namespace LedManager {

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

void begin();

void setColor(uint8_t red, uint8_t green, uint8_t blue);
void setState(bool state);

Color getColor();
bool getState();


} // namespace LedManager
