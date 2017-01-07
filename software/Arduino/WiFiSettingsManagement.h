#ifndef WIFISETTINGSMANAGEMENT_H
#define WIFISETTINGSMANAGEMENT_H

#include "SettingsStorage.h"

#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>

class WiFiSettingsManagement {
  public:
    WiFiSettingsManagement(SettingsStorage *settingsStorage);
    bool connect();
    void handleClient();
  private:
    SettingsStorage *_settingsStorage;
    ESP8266WebServer *_server;
    //void _startAP(void);
    bool _handleFileRead(String path);
    //bool _connectWiFi();
    //bool _wiFiConnected = false;
    String _getContentType(String filename);
    DNSServer _dnsServer;
    IPAddress *_apIP;
    IPAddress *_netMsk;
};

#endif
