#include <cmath>
#include <unity.h>

#include "DallasTemperature.h"
#include "SensorManager.h"
#include "testdouble.h"

namespace td = testdouble;

void setUp(void) { td::resetAll(); }
void tearDown(void) {}

static void runFor(unsigned long ms) {
  for (unsigned long i = 0; i < ms; i++) {
    td::advanceMillis(1);
    SensorManager::update();
  }
}

void test_no_read_before_interval(void) {
  runFor(2999);
  TEST_ASSERT_EQUAL_INT(0, td::g_dallasRequests);
  TEST_ASSERT_FALSE(SensorManager::hasValidTemps());
}

void test_first_cycle_requests_then_reads(void) {
  td::queueDallasReads(25.5f, 20.1f);
  runFor(2000);
  TEST_ASSERT_EQUAL_INT(2, td::g_dallasRequests);
  TEST_ASSERT_EQUAL_FLOAT(25.5f, SensorManager::getHotTemp());
  TEST_ASSERT_EQUAL_FLOAT(20.1f, SensorManager::getColdTemp());
  TEST_ASSERT_TRUE(SensorManager::hasValidTemps());
}

void test_refresh_cadence_under_6s(void) {
  td::queueDallasReads(25.5f, 20.1f);
  runFor(4000);
  td::queueDallasReads(26.0f, 21.0f);
  runFor(3200);
  TEST_ASSERT_EQUAL_FLOAT(26.0f, SensorManager::getHotTemp());
  TEST_ASSERT_EQUAL_FLOAT(21.0f, SensorManager::getColdTemp());
}

void test_hot_disconnect_error_after_five_cycles(void) {
  for (int i = 0; i < 6; i++) {
    td::queueDallasReads(DEVICE_DISCONNECTED_C, 20.0f);
    runFor(4000);
  }
  TEST_ASSERT_TRUE(SensorManager::isHotProbeError());
  TEST_ASSERT_FALSE(SensorManager::isColdProbeError());
}

void test_hot_error_clears_on_recovery(void) {
  for (int i = 0; i < 6; i++) {
    td::queueDallasReads(DEVICE_DISCONNECTED_C, 20.0f);
    runFor(4000);
  }
  TEST_ASSERT_TRUE(SensorManager::isHotProbeError());
  td::queueDallasReads(30.0f, 20.0f);
  runFor(4000);
  TEST_ASSERT_FALSE(SensorManager::isHotProbeError());
}

void test_overheat_on_hot_threshold(void) {
  td::queueDallasReads(41.0f, 20.0f);
  runFor(4000);
  TEST_ASSERT_TRUE(SensorManager::isOverheatError());
  td::queueDallasReads(30.0f, 20.0f);
  runFor(4000);
  TEST_ASSERT_FALSE(SensorManager::isOverheatError());
}

void test_overheat_on_cold_threshold(void) {
  td::queueDallasReads(30.0f, 36.0f);
  runFor(4000);
  TEST_ASSERT_TRUE(SensorManager::isOverheatError());
}

void test_dallas_disabled_clears_flags_and_stops_reads(void) {
  td::queueDallasReads(41.0f, 36.0f);
  runFor(4000);
  TEST_ASSERT_TRUE(SensorManager::isOverheatError());
  SensorManager::setDallasState(false);
  int requestsBefore = td::g_dallasRequests;
  runFor(7000);
  TEST_ASSERT_EQUAL_INT(requestsBefore, td::g_dallasRequests);
  TEST_ASSERT_FALSE(SensorManager::isHotProbeError());
  TEST_ASSERT_FALSE(SensorManager::isColdProbeError());
  TEST_ASSERT_FALSE(SensorManager::isOverheatError());
}

void test_dallas_reenable_starts_fresh(void) {
  td::queueDallasReads(41.0f, 36.0f);
  runFor(4000);
  SensorManager::setDallasState(false);
  SensorManager::setDallasState(true);
  for (int i = 0; i < 6; i++) {
    td::queueDallasReads(DEVICE_DISCONNECTED_C, 20.0f);
    runFor(4000);
  }
  TEST_ASSERT_TRUE(SensorManager::isHotProbeError());
}

void test_dht_reads_update_values(void) {
  td::queueDhtRead(55.0f, 26.0f);
  runFor(2600);
  TEST_ASSERT_EQUAL_FLOAT(55.0f, SensorManager::getAmbientHumidity());
  TEST_ASSERT_EQUAL_FLOAT(26.0f, SensorManager::getAmbientTemp());
}

void test_dht_error_after_repeated_failures(void) {
  td::queueDhtRead(55.0f, 26.0f);
  runFor(2600);
  TEST_ASSERT_FALSE(SensorManager::isDhtError());
  for (int i = 0; i < 6; i++) {
    td::queueDhtRead(NAN, NAN);
    runFor(2600);
  }
  TEST_ASSERT_TRUE(SensorManager::isDhtError());
  SensorManager::setDhtState(false);
  TEST_ASSERT_FALSE(SensorManager::isDhtError());
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_no_read_before_interval);
  RUN_TEST(test_first_cycle_requests_then_reads);
  RUN_TEST(test_refresh_cadence_under_6s);
  RUN_TEST(test_hot_disconnect_error_after_five_cycles);
  RUN_TEST(test_hot_error_clears_on_recovery);
  RUN_TEST(test_overheat_on_hot_threshold);
  RUN_TEST(test_overheat_on_cold_threshold);
  RUN_TEST(test_dallas_disabled_clears_flags_and_stops_reads);
  RUN_TEST(test_dallas_reenable_starts_fresh);
  RUN_TEST(test_dht_reads_update_values);
  RUN_TEST(test_dht_error_after_repeated_failures);
  return UNITY_END();
}