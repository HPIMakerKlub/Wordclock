#include <DNSServer.h>
#include <WiFiManager.h> //Install via library manager. Version 0.12. is being used.
#include <Streaming.h> //Some nice serial printing. See http://arduiniana.org/libraries/streaming/

//custom stuff
#include "SettingsStorage.h"
#include "WordClockControl.h"
#include "WiFiSettingsManagement.h"
#include "ESPTime.h" //Library for retrieving the current time. Autosyncs the time.
//#include "WiFiManager.h"

#define DEBUG_MODE true

#define LED_PIN  D8

ESPTime time1 = ESPTime();
SettingsStorage settings = SettingsStorage();
WordClockControl wordclock(LED_PIN, &settings);
WiFiSettingsManagement settingsManagement(&settings);

void setup() {
  Serial.begin(115200);
  Serial << "Hello out there!\n"
    << "Checkout the source code online: https://github.com/HPIMakerKlub/Wordclock\n\n" << endl;

  uint8_t initResult = settings.init();

  // Load settings
  switch(initResult) {
    case SettingsStorage::InitResult::InitResult_Ok:
      Serial << "Auto setup" << endl;
      Serial << "Loaded settings: " << endl
        << "SSID = " << settings.getSSID() << endl
        << "PSK = " << settings.getPSK() << endl
        << "Brightness = " << settings.getBrightness() << endl
        << "Color: r=" << settings.getR() << " g=" << settings.getG() << " b=" << settings.getB() << endl;
    case SettingsStorage::InitResult::InitResult_File_Created:
      #if DEBUG
        Serial << "FS initialized!" << endl;
      #endif
      break;
    default:
      Serial << "Loading settings failed catastrohpical!" << endl;
      ESP.restart();
      break;
  }
  settingsManagement.connect();
  wordclock.initHardware();
  time1.printTime();
  Serial << "Init done";
}

long lastUpdate = 0;
int i = 0;

void loop() {
  if (lastUpdate + 100L < millis()) {
    int hours = time1.getHour();
    int minutes = time1.getMinute();
    wordclock.displayTime(hours, minutes);
    lastUpdate = millis();
    i++;
    if ((i % 10) == 0) {
      time1.printTime();
    }
  }
  settingsManagement.handleClient();
}

/*
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      pixels.show();

      delay(wait);

      for (uint16_t i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
} */
