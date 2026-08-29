#pragma once
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>

#include "testdouble.h"

#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1

inline unsigned long millis() { return testdouble::g_millis; }

inline unsigned long micros() { return testdouble::g_millis * 1000UL; }

inline void delay(unsigned long) {}

inline void pinMode(uint8_t pin, uint8_t mode) {
  testdouble::g_pinModes.push_back({pin, mode});
}

inline void digitalWrite(uint8_t pin, uint8_t value) {
  testdouble::g_digitalWrites.push_back({pin, value});
}

inline void neopixelWrite(uint8_t pin, uint8_t r, uint8_t g, uint8_t b) {
  testdouble::g_neoWrites.push_back({pin, r, g, b});
}

class FakeSerial {
public:
  void begin(unsigned long) {}

  size_t printf(const char *fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    if (n > 0) {
      testdouble::g_serial += buf;
    }
    return (size_t)(n > 0 ? n : 0);
  }

  void print(const char *s) { testdouble::g_serial += s; }

  void println(const char *s) {
    testdouble::g_serial += s;
    testdouble::g_serial += "\n";
  }

  void println() { testdouble::g_serial += "\n"; }

  void println(struct tm *t, const char *fmt) {
    char buf[128];
    strftime(buf, sizeof(buf), fmt, t);
    testdouble::g_serial += buf;
    testdouble::g_serial += "\n";
  }
};

inline FakeSerial Serial;

inline void configTime(long, int, const char *) {
  testdouble::g_configTimeCalled = true;
}

inline bool getLocalTime(struct tm *out, uint32_t ms = 5000) {
  time_t v = (time_t)testdouble::g_epoch;
  localtime_r(&v, out);
  return true;
}