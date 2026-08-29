#include "TimeManager.h"

#include <secrets.h>
#include <Arduino.h>
#include <SunRise.h>

namespace TimeManager {
namespace {

constexpr double LATITUDE = MY_LAT;
constexpr double LONGITUDE = MY_LONG;

constexpr const char *NTP_SERVER = "pool.ntp.org";
constexpr const char *TIMEZONE = "EST5EDT,M3.2.0,M11.1.0";

constexpr time_t heatSunOffset = 0;
constexpr time_t lightSunOffset = 30 * 60;

SunRise sr;
time_t sunriseTime = 0;
time_t sunsetTime = 0;
int lastCalculatedDay = -1;

} // namespace

void begin() {
  Serial.println("Setting up time");
  configTime(0, 0, NTP_SERVER);

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
  } else {
    Serial.println("Got the time from NTP");
  }

  Serial.printf("Setting Timezone to %s\n", TIMEZONE);
  setenv("TZ", TIMEZONE, 1);
  tzset();
}

void update() {
  time_t now = time(NULL);
  struct tm local;
  localtime_r(&now, &local);

  if (local.tm_yday == lastCalculatedDay) {
    return;
  }

  sr.calculate(LATITUDE, LONGITUDE, now);
  sunriseTime = sr.riseTime;
  sunsetTime = sr.setTime;
  lastCalculatedDay = local.tm_yday;

  struct tm tRise;
  struct tm tSet;
  localtime_r(&sunriseTime, &tRise);
  localtime_r(&sunsetTime, &tSet);

  Serial.printf("Schedule updated for Day %d:\n", local.tm_yday);
  Serial.println(&tRise, "sunrise: %H:%M:%S");
  Serial.println(&tSet, "sunset: %H:%M:%S");
}

bool isHeatOn() {
  time_t now = time(NULL);
  return ((now - sunriseTime) >= heatSunOffset) &&
         ((sunsetTime - now) >= heatSunOffset);
}

bool isLightOn() {
  time_t now = time(NULL);
  return ((now - sunriseTime) >= lightSunOffset) &&
         ((sunsetTime - now) >= lightSunOffset);
}

} // namespace TimeManager
