#ifndef SETTINGSSTORAGE_h
#define SETTINGSSTORAGE_h

#include <ESP8266WiFi.h>

class SettingsStorage {
  public:
    SettingsStorage();
    
    enum InitResult { InitResult_Ok, InitResult_File_Created, InitResult_Error };
    enum SaveResult { SaveResult_Ok, SaveResult_Error };
    
    InitResult init();

    char* getSSID();
    SaveResult setSSID(char* ssid);

    char* getPSK();
    SaveResult setPSK(char* psk);
    
    uint8_t getBrightness();
    SaveResult setBrightness(uint8_t brightness);

    uint8_t getR();
    SaveResult setR(uint8_t r);

    uint8_t getG();
    SaveResult setG(uint8_t g);

    uint8_t getB();
    SaveResult setB(uint8_t b);
    
    uint32_t getColor();
    SaveResult setColor(uint32_t color);
    
    /*
    static const uint8_t INIT_OK = 0;
    static const uint8_t INIT_FILE_CREATED = 1;
    static const uint8_t INIT_CATASTROPHIC = 2; *

    /*
    static const uint8_t SAVECONFIG_OK = 0;
    static const uint8_t SAVECONFIG_FAIL = 1;*/
  private:
    char* _ssid;
    char* _psk;
    uint8_t _brightness;
    uint8_t _r;
    uint8_t _g;
    uint8_t _b;
    SaveResult _saveConfigFile();
};

#endif



