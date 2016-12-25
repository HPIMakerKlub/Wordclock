#include "WiFiSettingsManagement.h"

#include "WiFiManager.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Streaming.h>
#include "FS.h"

WiFiManager wifiManager;

WiFiSettingsManagement::WiFiSettingsManagement(SettingsStorage *settingsStorage) {
  _settingsStorage = settingsStorage;
  _server = new ESP8266WebServer(80);
  _apIP = new IPAddress(192, 168, 4, 1);
  _netMsk = new IPAddress(255, 255, 255, 0);
}

bool WiFiSettingsManagement::connect() {
  _server->on("/", [this]() {
      _handleFileRead("/index.html");
    });
  _server->on("/api/wifi_list.json", [this]() {
    int n = WiFi.scanNetworks();
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    JsonArray& networksJson = json.createNestedArray("networks");
    for (int i=0; i < n; i++) {
      JsonObject& wifiObject = networksJson.createNestedObject();
      wifiObject["name"] = WiFi.SSID(i);
      wifiObject["strength"] = 100 + WiFi.RSSI(i);
      wifiObject["encryption"] = !(WiFi.encryptionType(i) == ENC_TYPE_NONE);
    }
    String response;
    json.printTo(response);
    _server->send(200, "text/json", response);
  });
  /*_server->on("/api/reconnect.json", [this]() {
    _server->send(200, "text/plain", "Trying to reconnect");
    _wiFiConnected = _connectWiFi();
    if (!_wiFiConnected) {
      _startAP();
    }
  });*/

  _server->on("/api/config.json", [this]() {
    bool error = false;
    if (_server->hasArg("brightness")) {
      SettingsStorage::SaveResult res = _settingsStorage->setBrightness(_server->arg("brightness").toInt());
      if (res != SettingsStorage::SaveResult::SaveResult_Ok) {
        error = true;
        _server->send(503, "text/plain", "sth went wrong");
      }
    }

    if (_server->hasArg("r")) {
      SettingsStorage::SaveResult res = _settingsStorage->setR(_server->arg("r").toInt());
      if (res != SettingsStorage::SaveResult::SaveResult_Ok) {
        error = true;
        _server->send(503, "text/plain", "sth went wrong");
      }
    }

    if (_server->hasArg("g")) {
      SettingsStorage::SaveResult res = _settingsStorage->setG(_server->arg("g").toInt());
      if (res != SettingsStorage::SaveResult::SaveResult_Ok) {
        error = true;
        _server->send(503, "text/plain", "sth went wrong");
      }
    }

    if (_server->hasArg("b")) {
      SettingsStorage::SaveResult res = _settingsStorage->setB(_server->arg("b").toInt());
      if (res != SettingsStorage::SaveResult::SaveResult_Ok) {
        error = true;
        _server->send(503, "text/plain", "sth went wrong");
      }
    }

    if (_server->hasArg("ssid")) {
      String ssid = _server->arg("ssid");
      uint8_t length = ssid.length() + 1;
      char *ssid_char = new char[length]();
      ssid.toCharArray(ssid_char, length);
      Serial << "SSID = " << ssid_char << endl;
      SettingsStorage::SaveResult res = _settingsStorage->setSSID(ssid_char);
      if (res != SettingsStorage::SaveResult::SaveResult_Ok) {
        error = true;
        _server->send(503, "text/plain", "sth went wrong");
      }
    }

    if (_server->hasArg("psk")) {
      String psk = _server->arg("psk");
      uint8_t length = psk.length() + 1;
      char *psk_char = new char[length]();
      psk.toCharArray(psk_char, length );
      Serial << "Current SSID = " << _settingsStorage->getSSID() << "  PSK = " << psk_char << endl;
      SettingsStorage::SaveResult res = _settingsStorage->setPSK(psk_char);
      if (res != SettingsStorage::SaveResult::SaveResult_Ok) {
        error = true;
        _server->send(503, "text/plain", "sth went wrong");
      }
    }

    if (!error) {
      _server->send(200, "text/plain", "ok");
    }
    Serial.println("Done doing stuff");
  });
  _server->onNotFound([this](){
    if(!_handleFileRead(_server->uri())) {
      /*if (!_wiFiConnected) {
        _server->sendHeader("Location", "http://192.168.4.1/index.html", true);
        _server->send ( 302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
        _server->client().stop();
      } else {*/
        _server->send(404, "text/plain", "FileNotFound");
      //}
    }
  });

  // Try to auto connect
  /*_server->begin();
  if (_connectWiFi()) {
    Serial.println("We're online.");
  } else {
    Serial.println("Waiting for WiFi to be configured");
    _startAP();
    while (!_wiFiConnected) {
      _dnsServer.processNextRequest();
      _server->handleClient();
      yield();
    }
    Serial.println("WiFi configured! Exit.");
  }*/
  wifiManager.autoConnect("Wordclock_v2");
  _server->begin();
  MDNS.begin("wordclock");
  MDNS.addService("http", "tcp", 80);
}

void WiFiSettingsManagement::handleClient() {
  _server->handleClient();
}

bool WiFiSettingsManagement::_handleFileRead(String path){
  Serial << "Reading from disk: " << path << endl;
  if(path.endsWith("/")) path += "index.htm";
  String contentType = _getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = _server->streamFile(file, contentType);
    file.close();
    Serial << "Done reading" << endl;
    return true;
  }
  Serial << "Error reading" << endl;
  return false;
}

/*bool WiFiSettingsManagement::_connectWiFi() {
  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  Serial << "Connecting to " << _settingsStorage->getSSID() << " pass=" << _settingsStorage->getPSK() << endl;
  WiFi.begin(_settingsStorage->getSSID(), _settingsStorage->getPSK());
  long startTime = millis();
  while ((WiFi.status() != WL_CONNECTED) && (millis() < startTime + 12000L)) {
    yield();
    delay(50);
  }
  bool success = (WiFi.status() == WL_CONNECTED);
  if (success) {
    Serial << "success. IP = " << WiFi.localIP() << endl;
    _server->begin();
  } else {
    Serial << "not connected." << endl;
  }
  return success;
}

void WiFiSettingsManagement::_startAP() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(*_apIP, *_apIP, *_netMsk);
  WiFi.softAP("Wordclock");
  delay(500);
  Serial << "AP IP address: " << Serial.println(WiFi.softAPIP()) << endl;
  _dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  _dnsServer.start(53, "*", *_apIP);
}*/

String WiFiSettingsManagement::_getContentType(String filename){
  if(_server->hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".svg")) return "image/svg+xml";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}
