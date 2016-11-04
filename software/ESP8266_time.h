/*
 * Time.h - Library to retrieve current time via NTP
 * When using any of the methods it's automagically made sure, the
 * time is still correct and gets updatet if necessary.
 */

#ifndef ESP8266_Time_h
#define ESP8266_Time_h

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define NTP_PACKET_SIZE 48

class ESP8266_Time
{
  public:
    ESP8266_Time();
    void update();
    int getHours();
    int getMinutes();
    int getSeconds();
  private:
    unsigned int _localPort = 2390;  
    // A UDP instance to let us send and receive packets over UDP
    WiFiUDP _udp;
    IPAddress _timeServerIP; // time.nist.gov NTP server address
    
    const char* ntpServerName;
   
    byte _packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
    
    unsigned long _epoch; //last synced time
    unsigned long _lastMillis; //millis() of last synced time

    unsigned long _sendNTPpacket(IPAddress& address);

    unsigned long _getEpoche();
};

#endif
