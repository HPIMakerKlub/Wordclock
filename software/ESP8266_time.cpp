/*
 * ESP8266_Time.cpp
 */

#include "ESP8266_time.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


ESP8266_Time::ESP8266_Time() {
  ntpServerName = "time.nist.gov";
  _udp.begin(_localPort);
}

void ESP8266_Time::update() {
  Serial.println("Updating");
  //get a random server from the pool
  WiFi.hostByName(ntpServerName, _timeServerIP); 
  
  _sendNTPpacket(_timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);
  
  int cb = _udp.parsePacket();

  if (cb) {
    _udp.read(_packetBuffer, NTP_PACKET_SIZE);
    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(_packetBuffer[40], _packetBuffer[41]);
    unsigned long lowWord = word(_packetBuffer[42], _packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
     // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    _epoch = secsSince1900 - seventyYears;
    _lastMillis = millis();
    Serial.println("Update done");
  }
}

int ESP8266_Time::getHours() {
  unsigned long temp = _getEpoche();
  return (temp % 86400L) / 3600;
}

int ESP8266_Time::getMinutes() {
  unsigned long temp = _getEpoche();
  return (temp % 3600) / 60;
}


int ESP8266_Time::getSeconds() {
  unsigned long temp = _epoch + (millis() - _lastMillis) / 1000;
  return (temp % 60);
}

// send an NTP request to the time server at the given address
unsigned long ESP8266_Time::_sendNTPpacket(IPAddress& address)
{
  // set all bytes in the buffer to 0
  memset(_packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  _packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  _packetBuffer[1] = 0;     // Stratum, or type of clock
  _packetBuffer[2] = 6;     // Polling Interval
  _packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  _packetBuffer[12]  = 49;
  _packetBuffer[13]  = 0x4E;
  _packetBuffer[14]  = 49;
  _packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  _udp.beginPacket(address, 123); //NTP requests are to port 123
  _udp.write(_packetBuffer, NTP_PACKET_SIZE);
  _udp.endPacket();
}

unsigned long ESP8266_Time::_getEpoche() {
  // update if last time was long 1h ago
  if ((millis() - _lastMillis) > 3600000UL || _lastMillis == 0 || millis() < _lastMillis) {
    Serial.println("Update time");
    update();
  }
  return _epoch + (millis() - _lastMillis) / 1000;
}

