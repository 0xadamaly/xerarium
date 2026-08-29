#include <unity.h>

#include "LedManager.h"
#include "testdouble.h"

namespace td = testdouble;

void setUp(void) { td::resetAll(); }
void tearDown(void) {}

void test_begin_sets_pin_mode(void) {
  LedManager::begin();
  TEST_ASSERT_FALSE(td::g_pinModes.empty());
  TEST_ASSERT_EQUAL_INT(48, td::g_pinModes.back().pin);
}

void test_set_color_turns_on(void) {
  LedManager::begin();
  LedManager::setColor(255, 0, 0);
  TEST_ASSERT_TRUE(LedManager::getState());
  TEST_ASSERT_EQUAL_UINT8(255, LedManager::getColor().r);
  TEST_ASSERT_EQUAL_UINT8(0, LedManager::getColor().g);
}

void test_set_color_black_is_off(void) {
  LedManager::begin();
  LedManager::setColor(0, 0, 0);
  TEST_ASSERT_FALSE(LedManager::getState());
}

void test_set_state_off_writes_black(void) {
  LedManager::begin();
  LedManager::setColor(10, 20, 30);
  LedManager::setState(false);
  TEST_ASSERT_FALSE(LedManager::getState());
  TEST_ASSERT_EQUAL_UINT8(0, td::g_neoWrites.back().r);
  TEST_ASSERT_EQUAL_UINT8(0, td::g_neoWrites.back().g);
  TEST_ASSERT_EQUAL_UINT8(0, td::g_neoWrites.back().b);
}

void test_set_state_on_restores_last_color(void) {
  LedManager::begin();
  LedManager::setColor(10, 20, 30);
  LedManager::setState(false);
  LedManager::setState(true);
  TEST_ASSERT_TRUE(LedManager::getState());
  TEST_ASSERT_EQUAL_UINT8(10, LedManager::getColor().r);
  TEST_ASSERT_EQUAL_UINT8(20, LedManager::getColor().g);
  TEST_ASSERT_EQUAL_UINT8(30, LedManager::getColor().b);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_begin_sets_pin_mode);
  RUN_TEST(test_set_color_turns_on);
  RUN_TEST(test_set_color_black_is_off);
  RUN_TEST(test_set_state_off_writes_black);
  RUN_TEST(test_set_state_on_restores_last_color);
  return UNITY_END();
}