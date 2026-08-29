#include <unity.h>

#include "LampManager.h"
#include "testdouble.h"

namespace td = testdouble;

static bool hasWrite(int pin, int value) {
  for (const auto &w : td::g_digitalWrites) {
    if (w.pin == pin && w.value == value) {
      return true;
    }
  }
  return false;
}

void setUp(void) { td::resetAll(); }
void tearDown(void) {}

void test_begin_creates_two_off_channels(void) {
  LampManager::begin();
  TEST_ASSERT_EQUAL_INT(2, td::g_dimmerChannelCount);
  TEST_ASSERT_EQUAL_UINT32(0, td::g_dimmerLevels[0]);
  TEST_ASSERT_EQUAL_UINT32(0, td::g_dimmerLevels[1]);
  TEST_ASSERT_EQUAL_UINT8(0, LampManager::getLampViBrightness());
  TEST_ASSERT_EQUAL_UINT8(0, LampManager::getLampIrBrightness());
}

void test_begin_relay_low(void) {
  LampManager::begin();
  TEST_ASSERT_TRUE(hasWrite(12, LOW));
  TEST_ASSERT_FALSE(LampManager::getDaylightLampState());
}

void test_zero_cross_ok_by_default(void) {
  LampManager::begin();
  TEST_ASSERT_FALSE(LampManager::isZeroCrossError());
}

void test_zero_cross_failure_sets_error(void) {
  td::g_zeroCrossStatus = 1;
  LampManager::begin();
  TEST_ASSERT_TRUE(LampManager::isZeroCrossError());
}

void test_brightness_clamped_to_100(void) {
  LampManager::begin();
  LampManager::setLampViBrightness(150);
  TEST_ASSERT_EQUAL_UINT32(100, td::g_dimmerLevels[0]);
  LampManager::setLampIrBrightness(255);
  TEST_ASSERT_EQUAL_UINT32(100, td::g_dimmerLevels[1]);
}

void test_state_reflects_level(void) {
  LampManager::begin();
  LampManager::setLampViBrightness(50);
  TEST_ASSERT_TRUE(LampManager::getLampViState());
  LampManager::setLampViBrightness(0);
  TEST_ASSERT_FALSE(LampManager::getLampViState());
}

void test_set_state_restores_last_level(void) {
  LampManager::begin();
  LampManager::setLampViBrightness(60);
  LampManager::setLampViState(false);
  TEST_ASSERT_EQUAL_UINT8(0, LampManager::getLampViBrightness());
  LampManager::setLampViState(true);
  TEST_ASSERT_EQUAL_UINT8(60, LampManager::getLampViBrightness());
}

void test_ir_state_restores_last_level(void) {
  LampManager::begin();
  LampManager::setLampIrBrightness(35);
  LampManager::setLampIrState(false);
  TEST_ASSERT_FALSE(LampManager::getLampIrState());
  LampManager::setLampIrState(true);
  TEST_ASSERT_EQUAL_UINT8(35, LampManager::getLampIrBrightness());
}

void test_daylight_state_writes_relay(void) {
  LampManager::begin();
  LampManager::setDaylightLampState(true);
  TEST_ASSERT_TRUE(LampManager::getDaylightLampState());
  TEST_ASSERT_TRUE(hasWrite(12, HIGH));
  LampManager::setDaylightLampState(false);
  TEST_ASSERT_FALSE(LampManager::getDaylightLampState());
  TEST_ASSERT_TRUE(hasWrite(12, LOW));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_begin_creates_two_off_channels);
  RUN_TEST(test_begin_relay_low);
  RUN_TEST(test_zero_cross_ok_by_default);
  RUN_TEST(test_zero_cross_failure_sets_error);
  RUN_TEST(test_brightness_clamped_to_100);
  RUN_TEST(test_state_reflects_level);
  RUN_TEST(test_set_state_restores_last_level);
  RUN_TEST(test_ir_state_restores_last_level);
  RUN_TEST(test_daylight_state_writes_relay);
  return UNITY_END();
}