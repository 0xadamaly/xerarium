#pragma once
#include_next <time.h>

#ifdef __cplusplus
namespace testdouble {
inline long long g_epoch = 0;
} // namespace testdouble

inline time_t time(time_t *out) {
  time_t v = (time_t)testdouble::g_epoch;
  if (out != nullptr) {
    *out = v;
  }
  return v;
}
#endif