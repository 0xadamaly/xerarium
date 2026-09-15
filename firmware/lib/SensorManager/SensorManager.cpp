#include "SensorManager.h"

#include <DHT.h>
#include <DallasTemperature.h>
#include <OneWire.h>

namespace SensorManager {
namespace {

constexpr uint8_t HOT_TEMP_PIN = 4;
constexpr uint8_t COLD_TEMP_PIN = 5;
constexpr uint8_t DHT_PIN = 6;
constexpr uint8_t DHT_TYPE = DHT22;

constexpr unsigned long dallasInterval = 3000;
constexpr unsigned long dallasConversionTime = 750;
constexpr unsigned long dhtInterval = 2500;

const float overHeatColdThreshold = 35.0;
const float overHeatHotThreshold = 40.0;
const int maxConsecutiveErrors = 5;

static int numHotProbeErrors = 0;
static int numColdProbeErrors = 0;
static int numDhtErrors = 0;
static bool HOT_PROBE_ERROR = false;
static bool COLD_PROBE_ERROR = false;
static bool OVERHEAT_ERROR = false;
static bool DHT_ERROR = false;

OneWire oneWireHot(HOT_TEMP_PIN);
OneWire oneWireCold(COLD_TEMP_PIN);
DallasTemperature tempSensorHot(&oneWireHot);
DallasTemperature tempSensorCold(&oneWireCold);
DHT dht(DHT_PIN, DHT_TYPE);

static bool dallasEnabled = true;
static bool dhtEnabled = true;
static bool dallasValid = false;

float hotTemp = 0.0;
float coldTemp = 0.0;
float ambientTemp = 0.0;
float humidity = 0.0;

void handleDallasTempSensors() {
  static unsigned long lastReadDallas = 0;
  static unsigned long lastRequestTime = 0;
  static bool isConverting = false;

  unsigned long now = millis();

  if (!isConverting) {
    if ((now - lastReadDallas) < dallasInterval) {
      return;
    }
    lastReadDallas = now;

    tempSensorHot.requestTemperatures();
    tempSensorCold.requestTemperatures();
    lastRequestTime = now;
    isConverting = true;
  }

  if (isConverting && (now - lastRequestTime >= dallasConversionTime)) {
    float tHot = tempSensorHot.getTempCByIndex(0);
    float tCold = tempSensorCold.getTempCByIndex(0);

    if (tHot != DEVICE_DISCONNECTED_C) {
      hotTemp = tHot;
      numHotProbeErrors = 0;
      dallasValid = true;
    } else {
      numHotProbeErrors++;
    }
    if (tCold != DEVICE_DISCONNECTED_C) {
      coldTemp = tCold;
      numColdProbeErrors = 0;
    } else {
      numColdProbeErrors++;
    }
    isConverting = false;
  }
}

void handleDHTSensor() {
  static unsigned long lastReadDht = 0;

  unsigned long now = millis();

  if ((now - lastReadDht) < dhtInterval) {
    return;
  }
  lastReadDht = now;

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (!isnan(h) && !isnan(t)) {
    humidity = h;
    ambientTemp = t;
    numDhtErrors = 0;
  } else {
    numDhtErrors++;
  }
}

void handleErrors() {
  if (dallasEnabled) {
    HOT_PROBE_ERROR = (numHotProbeErrors > maxConsecutiveErrors);
    COLD_PROBE_ERROR = (numColdProbeErrors > maxConsecutiveErrors);
    OVERHEAT_ERROR =
        (coldTemp > overHeatColdThreshold) || (hotTemp > overHeatHotThreshold);
  } else {
    HOT_PROBE_ERROR = false;
    COLD_PROBE_ERROR = false;
    OVERHEAT_ERROR = false;
  }
  if (dhtEnabled) {
    DHT_ERROR = (numDhtErrors > maxConsecutiveErrors);
  } else {
    DHT_ERROR = false;
  }
}
} // namespace

void begin() {
  tempSensorHot.begin();
  tempSensorCold.begin();
  tempSensorHot.setWaitForConversion(false);
  tempSensorCold.setWaitForConversion(false);

  dht.begin();
}

void update() {
  if (dallasEnabled) {
    handleDallasTempSensors();
  }
  if (dhtEnabled) {
    handleDHTSensor();
  }
  handleErrors();
}

float getHotTemp() { return hotTemp; }

float getColdTemp() { return coldTemp; }

float getAmbientTemp() { return ambientTemp; }

float getAmbientHumidity() { return humidity; }

bool hasValidTemps() { return dallasValid; }

void setDallasState(bool state) {
  dallasEnabled = state;
  if (!state) {
    numHotProbeErrors = 0;
    numColdProbeErrors = 0;
    HOT_PROBE_ERROR = false;
    COLD_PROBE_ERROR = false;
    OVERHEAT_ERROR = false;
  }
}

void setDhtState(bool state) {
  dhtEnabled = state;
  if (!state) {
    numDhtErrors = 0;
    DHT_ERROR = false;
  }
}

bool isHotProbeError() { return HOT_PROBE_ERROR;}
bool isColdProbeError() { return COLD_PROBE_ERROR;}
bool isDhtError() { return DHT_ERROR;}
bool isOverheatError() { return OVERHEAT_ERROR;}


} // namespace SensorManager
