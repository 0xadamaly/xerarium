#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <secrets.h>

#include "ClimateManager.h"
#include "HomeAssistantManager.h"
#include "LampManager.h"
#include "LedManager.h"
#include "SensorManager.h"
#include "TimeManager.h"

const char *ssid = SECRET_SSID;
const char *password = SECRET_PASS;
const char *mdnsName = MDNS_NAME;

void startWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  Serial.println("Connecting Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  if (MDNS.begin(mdnsName)) {
    Serial.printf("MDNS initialized - %s.local\n", mdnsName);
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  Serial.print("Wifi RSSI=");
  Serial.println(WiFi.RSSI());
}

// void systemCheck() {
//
//   bool testLed = true;
//   bool testTempProbes = true;
//   bool testDHT = true;
//   bool testRelay = true;
//   bool testDimmers = true;
//
//   if (testLed) {
//
//     setRGBLed(255, 0, 0);
//     delay(2000);
//     setRGBLed(0, 255, 0);
//     delay(2000);
//     setRGBLed(0, 0, 255);
//     delay(2000);
//     setRGBLed(0, 0, 0);
//   }
//
//   Serial.printf("----------input test----------");
//   // hot probe
//   setRGBLed(255, 0, 0);
//   if (testTempProbes) {
//     tempSensorHot.begin();
//     tempSensorHot.requestTemperatures();
//     float t = tempSensorHot.getTempCByIndex(0);
//     if (t == DEVICE_DISCONNECTED_C) {
//       Serial.printf("Hot temp probe disconnected");
//     } else {
//       Serial.printf("Hot temp probe: %.2f C\n", t);
//     }
//     delay(1000);
//     // cold probe
//     tempSensorCold.begin();
//     tempSensorCold.requestTemperatures();
//     t = tempSensorCold.getTempCByIndex(0);
//     if (t == DEVICE_DISCONNECTED_C) {
//       Serial.printf("Cold temp probe disconnected");
//     } else {
//       Serial.printf("Cold temp probe: %.2f C\n", t);
//     }
//   }
//
//   // if (testDHT) {
//   //   //dht probe
//
//   //   float h = dht.readHumidity();
//   //   float t = dht.readTemperature();
//   //   if (isnan(h) || isnan(t)) {
//   //     Serial.printf("DHT read fail\n");
//   //   } else {
//   //     Serial.printf("DHT read: %.1f C, %.1f %% humidity\n", t, h);
//   //   }
//   // }
//   setRGBLed(0, 0, 0);
//   delay(1000);
//
//   Serial.printf("----------output test----------");
//   if (testRelay) {
//     // relay on/off
//     Serial.printf("Relay On");
//     digitalWrite(RELAY_PIN, HIGH);
//     setRGBLed(0, 255, 0);
//     delay(5000);
//     Serial.printf("Relay Off");
//     digitalWrite(RELAY_PIN, LOW);
//     setRGBLed(0, 0, 0);
//     delay(5000);
//   }
//   if (testDimmers) {
//     // visible lamp dimmer
//     neopixelWrite(RGB_LED_PIN, 0, 0, 255);
//     for (int i = 0; i <= 100; i += 20) {
//
//       setLampVI(i);
//       Serial.printf("vi lamp level: %3d %%\n", rbdimmer_get_level(dimmer_vi));
//       delay(5000);
//     }
//     setLampVI(0);
//     setRGBLed(0, 0, 0);
//     delay(1000);
//     setRGBLed(0, 0, 255);
//     // infrared lamp dimmer
//     for (int i = 0; i <= 100; i += 20) {
//       setLampIR(i);
//       Serial.printf("ir lamp level: %3d %%\n", rbdimmer_get_level(dimmer_ir));
//       delay(5000);
//     }
//     setLampIR(0);
//     setRGBLed(0, 0, 0);
//   }
// }


void setup() {
  Serial.begin(115200);
  startWifi();
  TimeManager::begin();
  ClimateManager::begin();
  SensorManager::begin();
  HAManager::begin();
  LampManager::begin();
  LedManager::begin();
}

void loop() {
  TimeManager::update();
  ClimateManager::update();
  SensorManager::update();
  HAManager::update();
}
