#pragma once
#include <time.h>

#include "testdouble.h"

class SunRise {
public:
  time_t riseTime = 0;
  time_t setTime = 0;

  void calculate(double, double, time_t) {
    testdouble::g_sunCalculations++;
    riseTime = (time_t)testdouble::g_sunrise;
    setTime = (time_t)testdouble::g_sunset;
  }
};