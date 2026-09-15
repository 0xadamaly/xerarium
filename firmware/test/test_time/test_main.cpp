#include <cstdlib>
#include <unity.h>

#include "TimeManager.h"
#include "testdouble.h"

namespace td = testdouble;

static constexpr long long BASE_EPOCH = 1767225600LL;

void setUp(void) {
  td::resetAll();
  setenv("TZ", "UTC0", 1);
  tzset();
}
void tearDown(void) {}

void test_begin_configures_ntp(void) {
  TimeManager::begin();
  TEST_ASSERT_TRUE(td::g_configTimeCalled);
}

void test_update_calculates_once_per_day(void) {
  td::g_epoch = BASE_EPOCH;
  td::g_sunrise = BASE_EPOCH + 100;
  td::g_sunset = BASE_EPOCH + 200;
  TimeManager::update();
  TEST_ASSERT_EQUAL_INT(1, td::g_sunCalculations);
  TimeManager::update();
  TEST_ASSERT_EQUAL_INT(1, td::g_sunCalculations);
}

void test_new_day_triggers_recalculation(void) {
  const long long t0 = BASE_EPOCH + 3 * 86400;
  td::g_epoch = t0;
  td::g_sunrise = t0 + 100;
  td::g_sunset = t0 + 200;
  TimeManager::update();
  TimeManager::update();
  td::g_epoch = t0 + 86400;
  TimeManager::update();
  TEST_ASSERT_EQUAL_INT(2, td::g_sunCalculations);
}

void test_heat_and_light_windows(void) {
  const long long t0 = BASE_EPOCH + 8 * 86400;
  td::g_epoch = t0;
  td::g_sunrise = t0 - 100;
  td::g_sunset = t0 + 4000;
  TimeManager::update();

  TEST_ASSERT_TRUE(TimeManager::isHeatOn());
  TEST_ASSERT_FALSE(TimeManager::isLightOn());

  td::g_epoch = t0 + 1800;
  TEST_ASSERT_TRUE(TimeManager::isHeatOn());
  TEST_ASSERT_TRUE(TimeManager::isLightOn());

  td::g_epoch = t0 + 5000;
  TEST_ASSERT_FALSE(TimeManager::isHeatOn());
  TEST_ASSERT_FALSE(TimeManager::isLightOn());
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_begin_configures_ntp);
  RUN_TEST(test_update_calculates_once_per_day);
  RUN_TEST(test_new_day_triggers_recalculation);
  RUN_TEST(test_heat_and_light_windows);
  return UNITY_END();
}