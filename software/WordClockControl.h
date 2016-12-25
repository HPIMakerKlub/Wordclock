#ifndef WORDCLOCKCONTROL_H
#define WORDCLOCKCONTROL_H

#include "SettingsStorage.h"
#include "xy.h"

#include <Adafruit_NeoPixel.h>

class WordClockControl {
  public:
    WordClockControl(uint8_t led_pin, SettingsStorage *settingsStorage);
    void displayTime(uint8_t hours, uint8_t minutes);
    void initHardware();
  private:
    void _lightWord(const uint8_t val[]);
    void _lightHour(uint8_t hours);
    Adafruit_NeoPixel *_pixels;
    SettingsStorage *_settingsStorage;
};

#endif