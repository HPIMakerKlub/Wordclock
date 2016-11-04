#include <Adafruit_NeoPixel.h>
#include "xy.h"
#include "ESP8266_time.h"

#define LED_PIN  D8
// #define COLOR_ORDER GRB
// #define CHIPSET     WS2811

ESP8266_Time time1;

void lightHour(int hours, uint32_t color);
void lightWord(const uint8_t val[], uint32_t color);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(110, LED_PIN, NEO_GRB + NEO_KHZ800);

uint32_t color = pixels.Color(255, 0, 255);

void setup() {
  Serial.begin(115200);
  Serial.println("Hi");
  //FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, 110);
  //FastLED.setBrightness( brightness );
  pixels.begin();
  pixels.setBrightness(80);

  
  lightWord(w_es, color);
  lightWord(w_ist, color);
  lightWord(w_viertel, color);
  lightWord(w_vor, color);
  lightWord(w_zwoelf, color);
  pixels.show();
  
  Serial.print("Connecting to ");
  Serial.println("fluepke");
  WiFi.begin("Fluepke", "unsicher");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  time1.update();

  delay(2000);
}

void loop() {
  int hours = time1.getHours() + 1;
  int minutes = time1.getMinutes();
  //clear all LEDs
  pixels.clear();

  //light up "ES IST"
  lightWord(w_es, color);
  lightWord(w_ist, color);

  Serial.print(hours);
  Serial.print(":");
  Serial.println(minutes);
  
  if (minutes < 5) {
    // light up "XX UHR"
    lightHour(hours, color);
    lightWord(w_uhr, color);
  } else if ((minutes >= 5) && (minutes < 10)) {
    // light up "FUENF NACH XX"
    lightWord(w_fuenf_1, color);
    lightWord(w_nach, color);
    lightHour(hours, color);
  } else if ((minutes >= 10) && (minutes < 15)) {
    // light up "ZEHN NACH XX"
    lightWord(w_zehn_1, color);
    lightWord(w_nach, color);
    lightHour(hours, color);
  } else if ((minutes >= 15) && (minutes < 30)) {
    lightWord(w_viertel, color);
    lightWord(w_nach, color);
    lightHour(hours, color);
  } else if ((minutes >= 30) && (minutes < 45)) {
    Serial.println("HALB!");
    lightWord(w_halb, color);
    lightHour(hours + 1, color);
  } else if ((minutes >= 45) && (minutes < 50)) {
    lightWord(w_viertel, color);
    lightWord(w_vor, color);
    lightHour(hours + 1, color);
  } else if ((minutes >= 50) && (minutes < 55)) {
    lightWord(w_zehn_1, color);
    lightWord(w_vor, color);
    lightHour(hours + 1, color);
  } else {
    lightWord(w_fuenf_1, color);
    lightWord(w_vor, color);
    lightHour(hours + 1, color);
  }
  pixels.show();
  Serial.println("\n\n\n");
  delay(100);
}

void lightWord(const uint8_t val[], uint32_t color) {
  while(*val != 255) {
    Serial.print(*val);
    Serial.print(",");
    pixels.setPixelColor(*val, color);
    val++;
  }
}

void lightHour(int hours, uint32_t color) {
  hours = hours % 12;
  switch (hours) {
    case 0:
      lightWord(w_zwoelf, color);
      break;
    case 1:
      lightWord(w_eins, color);
      break;
    case 2:
      lightWord(w_zwei, color);
      break;
    case 3:
      lightWord(w_drei, color);
      break;
    case 4:
      lightWord(w_vier, color);
      break;
    case 5:
      lightWord(w_fuenf_2, color);
      break;
    case 6:
      lightWord(w_sechs, color);
      break;
    case 7:
      lightWord(w_sieben, color);
      break;
    case 8:
      lightWord(w_acht, color);
      break;
    case 9:
      lightWord(w_neun, color);
      break;
    case 10:
      lightWord(w_zehn_2, color);
      break;
    case 11:
      lightWord(w_elf, color);
      break;
  }
}

