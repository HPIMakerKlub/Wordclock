#include "WordClockControl.h"

WordClockControl::WordClockControl(uint8_t led_pin, SettingsStorage *settingsStorage) {
  _settingsStorage = settingsStorage;
  _pixels = new Adafruit_NeoPixel(110, led_pin, NEO_GRB + NEO_KHZ800);
}

void WordClockControl::initHardware() {
  _pixels->begin();
}

void WordClockControl::displayTime(uint8_t hours, uint8_t minutes) {
  //clear all LEDs
  _pixels->clear();
  _pixels->setBrightness(_settingsStorage->getBrightness());

  if (minutes < 5) {
    //light up "ES IST"
    _lightWord(w_es);
    _lightWord(w_ist);
    // light up "XX UHR"
    _lightHour(hours);
    if (hours != 1) {
      _lightWord(w_uhr);
    }
  } else if ((minutes >= 5) && (minutes < 10)) {
    // light up "FUENF NACH XX"
    _lightWord(w_fuenf_1);
    _lightWord(w_nach);
    _lightHour(hours);
  } else if ((minutes >= 10) && (minutes < 15)) {
    // light up "ZEHN NACH XX"
    _lightWord(w_zehn_1);
    _lightWord(w_nach);
    _lightHour(hours);
  } else if ((minutes >= 15) && (minutes < 20)) {
    _lightWord(w_viertel);
    _lightWord(w_nach);
    _lightHour(hours);
  } else if ((minutes >= 20) && (minutes < 25)) {
    _lightWord(w_zehn_1);
    _lightWord(w_vor_1);
    _lightWord(w_halb);
    _lightHour(hours + 1);
  } else if ((minutes >= 25) && (minutes < 30)) {
    _lightWord(w_fuenf_1);
    _lightWord(w_vor_1);
    _lightWord(w_halb);
    _lightHour(hours + 1);
  } else if ((minutes >= 30) && (minutes < 35)) {
    #if DEBUG > 1
      Serial.println("HALB!");
    #endif
    _lightWord(w_halb);
    _lightHour(hours + 1);
  } else if ((minutes >= 35) && (minutes < 40)) {
    _lightWord(w_fuenf_1);
    _lightWord(w_nach);
    _lightWord(w_halb);
    _lightHour(hours + 1);
  } else if ((minutes >= 40) && (minutes < 45)) {
    _lightWord(w_zehn_1);
    _lightWord(w_nach);
    _lightWord(w_halb);
    _lightHour(hours + 1);
  } else if ((minutes >= 45) && (minutes < 50)) {
    _lightWord(w_viertel);
    _lightWord(w_vor_2);
    _lightHour(hours + 1);
  } else if ((minutes >= 50) && (minutes < 55)) {
    _lightWord(w_zehn_1);
    _lightWord(w_vor_2);
    _lightHour(hours + 1);
  } else {
    _lightWord(w_fuenf_1);
    _lightWord(w_vor_2);
    _lightHour(hours + 1);
  }
  _pixels->show();
}

void WordClockControl::_lightWord(const uint8_t val[]) {
  while(*val != 255) {
    #if DEBUG > 3
      Serial.print(*val);
      Serial.print(",");
    #endif
    _pixels->setPixelColor(*val, _pixels->Color(_settingsStorage->getR(), _settingsStorage->getG(), _settingsStorage->getB()));
    val++;
  }
}

void WordClockControl::_lightHour(uint8_t hours) {
  hours = hours % 12;
  switch (hours) {
    case 0:
      _lightWord(w_zwoelf);
      break;
    case 1:
      _lightWord(w_eins);
      break;
    case 2:
      _lightWord(w_zwei);
      break;
    case 3:
      _lightWord(w_drei);
      break;
    case 4:
      _lightWord(w_vier);
      break;
    case 5:
      _lightWord(w_fuenf_2);
      break;
    case 6:
      _lightWord(w_sechs);
      break;
    case 7:
      _lightWord(w_sieben);
      break;
    case 8:
      _lightWord(w_acht);
      break;
    case 9:
      _lightWord(w_neun);
      break;
    case 10:
      _lightWord(w_zehn_2);
      break;
    case 11:
      _lightWord(w_elf);
      break;
  }
}
