#include "SettingsStorage.h"
#include <FS.h>
#include <ArduinoJson.h>
#include <Streaming.h>

SettingsStorage::SettingsStorage() {
  _ssid = "NotConfigured";
  _psk = "NotConfigured";
  _brightness = 128;
  _r = 255;
  _g = 255;
  _b = 255;
}

/*
Initializes the filesystem and loads the current configuration

  static const uint8_t INIT_OK = 0;
  static const uint8_t INIT_FILE_CREATED = 1;
  static const uint8_t INIT_CATASTROPHIC = 2;

*/
SettingsStorage::InitResult SettingsStorage::init() {
  if (SPIFFS.begin()) {
    #if DEBUG_MODE
      Serial << "Mounted FS" << endl;
    #endif
    File configFile = SPIFFS.open("/config.json", "r");
    if (configFile) {
      #if DEBUG_MODE
        Serial << "Opened config file" << endl;
      #endif
      size_t size = configFile.size();
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);
      
      Serial << "Config file size = " << size << endl;
      
      configFile.readBytes(buf.get(), size);
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.parseObject(buf.get());
      #if DEBUG
        json.printTo(Serial);
      #endif

      if (json.success()) {
        #if DEBUG
          Serial << "JSON parsed" << endl;
        #endif

        _ssid = strdup(json["ssid"]);
        _psk = strdup(json["psk"]);
        _brightness = atoi(json["brightness"]);
        _r = atoi(json["r"]);
        _g = atoi(json["g"]);
        _b = atoi(json["b"]);
        return InitResult_Ok;
      } else {
        Serial << "JSON parse error. Creating new one." << endl;
        if (_saveConfigFile()) {
          Serial << "Done." << endl;
          return InitResult_File_Created;
        } else {
          Serial << "Failed." << endl;
          return InitResult_Error;
        }
      }
    } else {
      Serial << "Error opening config file. Creating new one." << endl;
      if (_saveConfigFile()) {
        Serial << "Done." << endl;
        return InitResult_File_Created;
      } else {
        Serial << "Failed." << endl;
        return InitResult_Error;
      }
    }
  } else {
    Serial << "Error mounting FS" << endl;
    return InitResult_Error;
  }
}

SettingsStorage::SaveResult SettingsStorage::_saveConfigFile() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["ssid"] = _ssid;
  json["psk"] = _psk;
  json["brightness"] = _brightness;
  json["r"] = _r;
  json["g"] = _g;
  json["b"] = _b;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial << "Error opening config file for writing." << endl;
    return SaveResult_Error;
  }

  json.printTo(Serial);
  #if DEBUG_MODE
  #endif
  json.printTo(configFile);
  configFile.close();
  return SaveResult_Ok;
}

char* SettingsStorage::getSSID() {
  return _ssid;
}

SettingsStorage::SaveResult SettingsStorage::setSSID(char* ssid) {
  _ssid = ssid;
  return _saveConfigFile();
}

char* SettingsStorage::getPSK() {
  return _psk;
}

SettingsStorage::SaveResult SettingsStorage::setPSK(char* psk) {
  _psk = psk;
  return _saveConfigFile();
}

uint8_t SettingsStorage::getBrightness() {
  return _brightness;
}

SettingsStorage::SaveResult SettingsStorage::setBrightness(uint8_t brightness) {
  _brightness = brightness;
  return _saveConfigFile();
}

uint8_t SettingsStorage::getR() {
  return _r;
}

SettingsStorage::SaveResult SettingsStorage::setR(uint8_t r) {
  _r = r;
  return _saveConfigFile();
}

uint8_t SettingsStorage::getG() {
  return _g;
}

SettingsStorage::SaveResult SettingsStorage::setG(uint8_t g) {
  _g = g;
  return _saveConfigFile();
}

uint8_t SettingsStorage::getB() {
  return _b;
}

SettingsStorage::SaveResult SettingsStorage::setB(uint8_t b) {
  _b = b;
  return _saveConfigFile();
}

uint32_t SettingsStorage::getColor() {
  return (uint32_t)_r << 16 | (uint32_t)_g << 8 | _b;
}

SettingsStorage::SaveResult SettingsStorage::setColor(uint32_t color) {
  _r = (color >> 16) & 255;
  _g = (color >> 8) & 255;
  _b = color & 255;
  return _saveConfigFile();
}
