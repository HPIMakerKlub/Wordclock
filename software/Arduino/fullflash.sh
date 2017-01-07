esptool.py --port $1 erase_flash

/opt/arduino-1.6.8/arduino --board esp8266:esp8266:d1_mini --upload Wordclock.ino --port $1
screen $1 115200

