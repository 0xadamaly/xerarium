#include <unity.h>

#include "ClimateManager.h"
#include "DallasTemperature.h"
#include "LampManager.h"
#include "SensorManager.h"
#include "TimeManager.h"
#include "testdouble.h"

namespace td = testdouble;

static constexpr long long BASE_EPOCH = 1767225600LL;
static int currentDay = 0;

void setUp(void) { td::resetAll(); }
void tearDown(void) {}

static void beginManagers(void) {
  SensorManager::begin();
  LampManager::begin();
  ClimateManager::begin();
}

static void runSensorFor(unsigned long ms) {
  for (unsigned long i = 0; i < ms; i++) {
    td::advanceMillis(1);
    SensorManager::update();
  }
}

static void makeDallasValid(float hot, float cold) {
  td::queueDallasReads(hot, cold);
  runSensorFor(4000);
}

static void setupDay(long long riseOffset, long long setOffset) {
  currentDay++;
  td::g_epoch = BASE_EPOCH + (long long)currentDay * 86400;
  td::g_sunrise = td::g_epoch + riseOffset;
  td::g_sunset = td::g_epoch + setOffset;
  TimeManager::update();
}

static void climateStep(void) {
  td::advanceMillis(600);
  ClimateManager::update();
}

void test_no_pid_before_valid_temps(void) {
  beginManagers();
  setupDay(-4000, 4000);
  LampManager::setLampViBrightness(30);
  LampManager::setLampIrBrightness(30);
  climateStep();
  TEST_ASSERT_EQUAL_UINT32(30, td::g_dimmerLevels[0]);
  TEST_ASSERT_EQUAL_UINT32(30, td::g_dimmerLevels[1]);
}

void test_manual_mode_blocks_climate_writes(void) {
  beginManagers();
  setupDay(-4000, 4000);
  ClimateManager::setManualModeState(true);
  LampManager::setLampViBrightness(40);
  LampManager::setLampIrBrightness(40);
  climateStep();
  TEST_ASSERT_EQUAL_UINT32(40, td::g_dimmerLevels[0]);
  TEST_ASSERT_EQUAL_UINT32(40, td::g_dimmerLevels[1]);
}

void test_manual_mode_blocks_daylight(void) {
  beginManagers();
  setupDay(-4000, 4000);
  ClimateManager::setManualModeState(true);
  climateStep();
  TEST_ASSERT_FALSE(LampManager::getDaylightLampState());
}

void test_daylight_follows_schedule_when_auto(void) {
  beginManagers();
  setupDay(-4000, 4000);
  ClimateManager::setManualModeState(false);
  makeDallasValid(28.0f, 24.0f);
  climateStep();
  TEST_ASSERT_TRUE(LampManager::getDaylightLampState());
  td::g_epoch += 5000;
  climateStep();
  TEST_ASSERT_FALSE(LampManager::getDaylightLampState());
  TEST_ASSERT_EQUAL_UINT32(0, td::g_dimmerLevels[0]);
  TEST_ASSERT_EQUAL_UINT32(0, td::g_dimmerLevels[1]);
}

void test_heat_off_lamps_off_once(void) {
  beginManagers();
  setupDay(60000, 70000);
  ClimateManager::setManualModeState(false);
  LampManager::setLampViBrightness(50);
  LampManager::setLampIrBrightness(50);
  climateStep();
  TEST_ASSERT_EQUAL_UINT32(0, td::g_dimmerLevels[0]);
  TEST_ASSERT_EQUAL_UINT32(0, td::g_dimmerLevels[1]);
  size_t writesAfter = td::g_dimmerLevelWrites.size();
  climateStep();
  TEST_ASSERT_EQUAL_UINT(writesAfter, td::g_dimmerLevelWrites.size());
  TEST_ASSERT_EQUAL_UINT32(0, td::g_dimmerLevels[0]);
}

void test_pid_drives_both_lamps_equally(void) {
  beginManagers();
  setupDay(-4000, 4000);
  ClimateManager::setManualModeState(false);
  makeDallasValid(28.0f, 24.0f);
  climateStep();
  TEST_ASSERT_EQUAL_UINT32(td::g_dimmerLevels[0], td::g_dimmerLevels[1]);
  TEST_ASSERT_GREATER_THAN(0, td::g_dimmerLevels[0]);
  TEST_ASSERT_LESS_OR_EQUAL(100, td::g_dimmerLevels[0]);
  TEST_ASSERT_EQUAL_UINT8((uint8_t)td::g_dimmerLevels[0],
                          LampManager::getLampViBrightness());
}

void test_probe_error_forces_lamps_off(void) {
  beginManagers();
  setupDay(-4000, 4000);
  ClimateManager::setManualModeState(false);
  makeDallasValid(30.0f, 24.0f);
  for (int i = 0; i < 6; i++) {
    td::queueDallasReads(DEVICE_DISCONNECTED_C, DEVICE_DISCONNECTED_C);
    runSensorFor(4000);
  }
  TEST_ASSERT_TRUE(SensorManager::isHotProbeError());
  LampManager::setLampViBrightness(50);
  LampManager::setLampIrBrightness(50);
  climateStep();
  TEST_ASSERT_EQUAL_UINT32(0, td::g_dimmerLevels[0]);
  TEST_ASSERT_EQUAL_UINT32(0, td::g_dimmerLevels[1]);
}

void test_overheat_error_forces_lamps_off(void) {
  beginManagers();
  setupDay(-4000, 4000);
  ClimateManager::setManualModeState(false);
  makeDallasValid(41.0f, 24.0f);
  TEST_ASSERT_TRUE(SensorManager::isOverheatError());
  LampManager::setLampViBrightness(50);
  LampManager::setLampIrBrightness(50);
  climateStep();
  TEST_ASSERT_EQUAL_UINT32(0, td::g_dimmerLevels[0]);
  TEST_ASSERT_EQUAL_UINT32(0, td::g_dimmerLevels[1]);
}

void test_target_temp_roundtrip(void) {
  ClimateManager::setTargetTemp(35.5);
  TEST_ASSERT_EQUAL_DOUBLE(35.5, ClimateManager::getTargetTemp());
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_no_pid_before_valid_temps);
  RUN_TEST(test_manual_mode_blocks_climate_writes);
  RUN_TEST(test_manual_mode_blocks_daylight);
  RUN_TEST(test_daylight_follows_schedule_when_auto);
  RUN_TEST(test_heat_off_lamps_off_once);
  RUN_TEST(test_pid_drives_both_lamps_equally);
  RUN_TEST(test_probe_error_forces_lamps_off);
  RUN_TEST(test_overheat_error_forces_lamps_off);
  RUN_TEST(test_target_temp_roundtrip);
  return UNITY_END();
}