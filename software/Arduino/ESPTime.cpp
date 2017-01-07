// Inspired by: http://www.arduinoclub.de/2016/05/07/arduino-ide-esp8266-ntp-server-timezone/
#include "ESPTime.h"

#include "Streaming.h"

ESPTime::ESPTime() {
  _udp.begin(_localPort);
}
//### NTP, TIME & TIMEZONE ###################################################################
bool ESPTime::updateTime(char* ntpServerName)
{
  if ((millis() < _lastTimeUpdate + 14400000L) && (millis() > _lastTimeUpdate) && (timeStatus() == timeSet)) {
      return true;
  }
  DBG_OUTPUT_PORT << "NTP request..." << endl;
  IPAddress ntpServerIP; // NTP server's ip address

  while (_udp.parsePacket() > 0); // discard any previously received packets
  DBG_OUTPUT_PORT << "Transmit NTP Request" << endl;
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  DBG_OUTPUT_PORT << ntpServerName << ": " <<  ntpServerIP << endl;

  _sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = _udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      DBG_OUTPUT_PORT << "Receive NTP Response";
      _udp.read(_packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 = (unsigned long)_packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)_packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)_packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)_packetBuffer[43];
      setTime(secsSince1900 - 2208988800UL);
      //setTime(23, 55, 0, 30, 3, 2016); //simulate time for test
      _lastTimeUpdate = millis();
      return true;
    }
  }
  DBG_OUTPUT_PORT << "FATAL ERROR : No NTP Response.";
  return false; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void ESPTime::_sendNTPpacket(IPAddress &address)
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
  _packetBuffer[12] = 49;
  _packetBuffer[13] = 0x4E;
  _packetBuffer[14] = 49;
  _packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  _udp.beginPacket(address, 123); //NTP requests are to port 123
  _udp.write(_packetBuffer, NTP_PACKET_SIZE);
  _udp.endPacket();
}

time_t ESPTime::getTime() {
  if (!updateTime(_ntpServerName1)) { updateTime(_ntpServerName2); }
  return _CE->toLocal(now(), &tcr);
}

uint8_t ESPTime::getHour() {
  return hour(getTime());
}

uint8_t ESPTime::getMinute() {
  return minute(getTime());
}

void ESPTime::printTime()
{
  time_t t = getTime();
  DBG_OUTPUT_PORT.print('\n');
  sPrintI00(hour(t));
  sPrintDigits(minute(t));
  sPrintDigits(second(t));
  DBG_OUTPUT_PORT.print(' ');
  DBG_OUTPUT_PORT.print(dayShortStr(weekday(t)));
  DBG_OUTPUT_PORT.print(' ');
  sPrintI00(day(t));
  DBG_OUTPUT_PORT.print(' ');
  DBG_OUTPUT_PORT.print(monthShortStr(month(t)));
  DBG_OUTPUT_PORT.print(' ');
  DBG_OUTPUT_PORT.print(year(t));
  DBG_OUTPUT_PORT.println(' ');
}
 
//Print an integer in "00" format (with leading zero).
//Input value assumed to be between 0 and 99.
void ESPTime::sPrintI00(int val)
{
    if (val < 10) DBG_OUTPUT_PORT.print('0');
    DBG_OUTPUT_PORT.print(val, DEC);
    return;
}
 
//Print an integer in ":00" format (with leading zero).
//Input value assumed to be between 0 and 99.
void ESPTime::sPrintDigits(int val)
{
    DBG_OUTPUT_PORT.print(':');
    if (val < 10) DBG_OUTPUT_PORT.print('0');
    DBG_OUTPUT_PORT.print(val, DEC);
}
