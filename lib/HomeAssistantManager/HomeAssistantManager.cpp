#include "HomeAssistantManager.h"
// TODO: FIx this file to follow header
#include <Arduino.h>
#include <ArduinoHA.h>
#include <WiFi.h>

#include "LampManager.h"
#include "LedManager.h"
#include "SensorManager.h"
#include "ClimateManager.h"

#include "secrets.h"

namespace HAManager {
namespace  {
const char *mdnsName = MDNS_NAME;
const char *mqttName = MQTT_USER;
const char *mqttPass = MQTT_PASS;

WiFiClient client;
HADevice device;
HAMqtt mqtt(client, device);

HAButton resetButton("resetButton");
HAButton debugButton("debugButton");
HALight rgbLed("rgbLed", HALight::RGBFeature);
HALight lampVI("lampVi", HALight::BrightnessFeature);
HALight lampIR("lampIr", HALight::BrightnessFeature);
HALight daylightLamp("dayLamp");

HASensorNumber hotTempSensor("hotTemp", HASensorNumber::PrecisionP1);
HASensorNumber coldTempSensor("coldTemp", HASensorNumber::PrecisionP1);
HASensorNumber ambientTempSensor("ambiTemp", HASensorNumber::PrecisionP1);
HASensorNumber ambientHumiditySensor("ambiHum", HASensorNumber::PrecisionP1);
HASwitch manualMode("manualMode");
HASwitch sensorMode("sensorMode");
HABinarySensor isHotProbeError("hotError");
HABinarySensor isColdProbeError("coldError");
HABinarySensor isOverheatError("overheatError");
HABinarySensor isDimmerError("dimmerError");
HABinarySensor isDhtReadError("dhtError");
HANumber targetTempHA("targetTemp", HANumber::PrecisionP1);

HASensor resetReason("resetReason");

bool isSensorMode = true;

void onNumberCommand(HANumeric number, HANumber *sender) {
  if (sender == &targetTempHA) {
    if (!number.isSet()) {
      // the reset command was send by Home Assistant
      ClimateManager::setTargetTemp(32.0);
    } else {
      float temp = number.toFloat(); ClimateManager::setTargetTemp(temp);
    }
  } else {
    sender->setState(number);
  }
}
void onButtonCommand(HAButton *sender) {
  if (sender == &resetButton) {
    ESP.restart();
  } else if (sender == &debugButton) {
    // TODO: Add in a debug functionality or remove it completely
  }
}

void onStateCommand(bool state, HALight *sender) {
  if (ClimateManager::getManualModeState()) {
    if (sender == &rgbLed) {
      LedManager::setState(state);
    } else if (sender == &lampVI) {
      LampManager::setLampViState(state);
    } else if (sender == &lampIR) {
      LampManager::setLampIrBrightness(state ? 100 : 0);
    } else if (sender == &daylightLamp) {
      LampManager::setDaylightLampState(state);
    }

    sender->setState(state);
  }
}

void onRGBColorCommand(HALight::RGBColor color, HALight *sender) {
  if (ClimateManager::getManualModeState()) {
    if (sender == &rgbLed) {
      LedManager::setColor(color.red, color.green, color.blue);
    }
    sender->setRGBColor(color);
  }
}

void onBrightnessCommand(uint8_t brightness, HALight *sender) {
  if (ClimateManager::getManualModeState()) {
    if (sender == &lampVI) {
      LampManager::setLampViBrightness(brightness);
    } else if (sender == &lampIR) {
      LampManager::setLampIrBrightness(brightness);
    }
    sender->setBrightness(brightness);
  }
}

void onStatusSwitch(bool state, HASwitch *sender) {
  if (sender == &manualMode) {
    ClimateManager::setManualModeState(state);
    Serial.printf("MANUAL MODE %s\n", (state ? "ON" : "OFF"));
  } else if (sender == &sensorMode) {
      SensorManager::setDallasState(state);
      SensorManager::setDhtState(state);
      isSensorMode = state;
  }
  sender->setState(state);
}
    
const char *resetReasonName(esp_reset_reason_t reason) {
  switch (reason) {
  case ESP_RST_POWERON:
    return "poweron";
  case ESP_RST_SW:
    return "software";
  case ESP_RST_PANIC:
    return "panic";
  case ESP_RST_INT_WDT:
    return "int_wdt";
  case ESP_RST_TASK_WDT:
    return "task_wdt";
  case ESP_RST_WDT:
    return "wdt";
  case ESP_RST_DEEPSLEEP:
    return "deepsleep";
  case ESP_RST_BROWNOUT:
    return "brownout";
  case ESP_RST_SDIO:
    return "sdio";
  case ESP_RST_USB:
    return "usb";
  case ESP_RST_JTAG:
    return "jtag";
  case ESP_RST_EFUSE:
    return "efuse";
  case ESP_RST_PWR_GLITCH:
    return "pwr_glitch";
  case ESP_RST_CPU_LOCKUP:
    return "cpu_lockup";
  default:
    return "unknown";
  }
}

void onConnection() {
  resetReason.setValue(resetReasonName(esp_reset_reason()));
}
} // namespace

void begin() {
  byte mac[6];
  WiFi.macAddress(mac);
  device.setUniqueId(mac, sizeof(mac));
  device.setName(mdnsName);
  device.setManufacturer("adam");
  device.setModel(mdnsName);
  device.setSoftwareVersion("0.1.0");

  resetButton.setIcon("mdi:restart");
  resetButton.setName("Reset");

  debugButton.setIcon("mdi:bug-check");
  debugButton.setName("Debug");

  rgbLed.setIcon("mdi:led-strip-variant");
  rgbLed.setName("RGB LED");

  lampVI.setIcon("mdi:lightbulb");
  lampVI.setName("Halogen Lamp");
  lampVI.setBrightnessScale(100);

  lampIR.setIcon("mdi:heat-wave");
  lampIR.setName("Ceramic Heat Emitter");
  lampIR.setBrightnessScale(100);

  daylightLamp.setIcon("mdi:lightbulb-fluorescent-tube");
  daylightLamp.setName("Daylight Lamp");

  hotTempSensor.setIcon("mdi:thermometer");
  hotTempSensor.setName("Hot End Temp");
  hotTempSensor.setDeviceClass("temperature");
  hotTempSensor.setStateClass("measurement");
  hotTempSensor.setUnitOfMeasurement("°C");
  hotTempSensor.setForceUpdate(true);

  coldTempSensor.setIcon("mdi:thermometer");
  coldTempSensor.setName("Cold End Temp");
  coldTempSensor.setDeviceClass("temperature");
  coldTempSensor.setStateClass("measurement");
  coldTempSensor.setUnitOfMeasurement("°C");
  coldTempSensor.setForceUpdate(true);

  ambientTempSensor.setIcon("mdi:thermometer");
  ambientTempSensor.setName("Ambient Temp");
  ambientTempSensor.setDeviceClass("temperature");
  ambientTempSensor.setStateClass("measurement");
  ambientTempSensor.setUnitOfMeasurement("°C");
  ambientTempSensor.setForceUpdate(true);

  ambientHumiditySensor.setIcon("mdi:water-percent");
  ambientHumiditySensor.setName("Ambient Humidity");
  ambientHumiditySensor.setDeviceClass("humidity");
  ambientHumiditySensor.setStateClass("measurement");
  ambientHumiditySensor.setUnitOfMeasurement("%");
  ambientHumiditySensor.setForceUpdate(true);

  resetReason.setIcon("mdi:cog-refresh");
  resetReason.setName("Reset Reason");
  resetReason.setForceUpdate(true);


  mqtt.onConnected(onConnection);

  manualMode.setIcon("mdi:gesture-tap");
  manualMode.setName("Manual Mode");

  sensorMode.setIcon("mdi:leak");
  sensorMode.setName("Sensor Mode");

  isHotProbeError.setIcon("mdi:thermometer-alert");
  isHotProbeError.setName("Heat Probe Error");
  isHotProbeError.setDeviceClass("problem");

  isColdProbeError.setIcon("mdi:thermometer-alert");
  isColdProbeError.setName("Cold Probe Error");
  isColdProbeError.setDeviceClass("problem");

  isDhtReadError.setIcon("mdi:alert-circle-outline");
  isDhtReadError.setName("DHT Read Error");
  isDhtReadError.setDeviceClass("problem");

  isOverheatError.setIcon("mdi:thermometer-high");
  isOverheatError.setName("Overheat");
  isOverheatError.setDeviceClass("heat");

  isDimmerError.setIcon("mdi:alert-circle-outline");
  isDimmerError.setName("Dimmer Error");
  isDimmerError.setDeviceClass("problem");

  targetTempHA.setIcon("mdi:thermometer");
  targetTempHA.setName("Target Temp");
  targetTempHA.setUnitOfMeasurement("°C");
  targetTempHA.setMin(15.0);
  targetTempHA.setMax(40.0);
  targetTempHA.setStep(0.5);
  targetTempHA.setMode(HANumber::ModeSlider);

  resetButton.onCommand(onButtonCommand);
  debugButton.onCommand(onButtonCommand);

  rgbLed.onStateCommand(onStateCommand);
  rgbLed.onRGBColorCommand(onRGBColorCommand);

  lampVI.onStateCommand(onStateCommand);
  lampVI.onBrightnessCommand(onBrightnessCommand);
  lampIR.onStateCommand(onStateCommand);
  lampIR.onBrightnessCommand(onBrightnessCommand);

  daylightLamp.onStateCommand(onStateCommand);

  manualMode.onCommand(onStatusSwitch);
  sensorMode.onCommand(onStatusSwitch);

  targetTempHA.onCommand(onNumberCommand);

  mqtt.setBufferSize(512);
  mqtt.begin(BROKER_ADDR, mqttName, mqttPass);
}

void update() {
  LedManager::Color ledColor = LedManager::getColor();
  rgbLed.setState(LedManager::getState());
  rgbLed.setRGBColor(HALight::RGBColor(ledColor.r, ledColor.g, ledColor.b));

  lampVI.setState(LampManager::getLampViState());
  lampVI.setBrightness(LampManager::getLampViBrightness());
  lampIR.setState(LampManager::getLampIrState());
  lampIR.setBrightness(LampManager::getLampIrBrightness());
  daylightLamp.setState(LampManager::getDaylightLampState());

  hotTempSensor.setValue(SensorManager::getHotTemp());
  coldTempSensor.setValue(SensorManager::getColdTemp());
  ambientTempSensor.setValue(SensorManager::getAmbientTemp());
  ambientHumiditySensor.setValue(SensorManager::getAmbientHumidity());

  manualMode.setState(ClimateManager::getManualModeState());
  sensorMode.setState(isSensorMode);

  isHotProbeError.setState(SensorManager::isHotProbeError());
  isColdProbeError.setState(SensorManager::isColdProbeError());
  isOverheatError.setState(SensorManager::isOverheatError());
  isDimmerError.setState(LampManager::isZeroCrossError());
  isDhtReadError.setState(SensorManager::isDhtError());

  targetTempHA.setState((float)ClimateManager::getTargetTemp());
  mqtt.loop();
}

}
