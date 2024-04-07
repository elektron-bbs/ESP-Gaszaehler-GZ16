//connect to Wifi in Station Mode
void startStation() {            //connect to Wifi in Station Mode
  webtype = 0;                          // Station
  WiFi.mode(WIFI_STA);                  // Station
  if (edhcp == false) {                 // DHCP aus
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.println(F("DHCP off, use static IP"));
#endif
    //WiFi.config(eip, edns, esgw, esnm);     // Style Arduino
    WiFi.config(eip, esgw, esnm, edns);       // Style ESP8266
  } else {                              // DHCP ein
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.println(F("DHCP on"));
#endif
    wifi_station_dhcpc_start();
  }
  connectWifiBestRssi(); // connect to AP with best RSSI
  // WiFi.begin(essid.c_str(), epass.c_str());
  char* chrOwnHostname = &OwnStationHostname[0];
  wifi_station_set_hostname(chrOwnHostname);
  httpUpdater.setup(&server);
  createWebServer();
}

// connect to AP with best RSSI
void connectWifiBestRssi() {
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.println(F("Scan networks for best RSSI..."));
  Serial.println(F("------------------------------"));
#endif
  // int8_t scanNetworks(bool async = false, bool show_hidden = false, uint8 channel = 0, uint8* ssid = NULL);
  // WifiNetworks = WiFi.scanNetworks(0, 1, 0, (uint8*)atoi(essid.c_str())); // Anzahl AP im Netzwerk der gespeicherten SSID
  WifiNetworks = WiFi.scanNetworks(); // Scanne Netzwerke
  uint8_t* usedMAC = {};              // Gets the MAC address of the router
  uint8_t usedChannel = 0;
  int16_t rssi = -999;
  int16_t bestRssi = -999;
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  uint8_t* MAC;          // Gets the MAC address of the router
  char str[18];          // for sprintf IP-Address and MAC
  uint8_t foundChannel = 0;
#endif
  for (int8_t i = 0; i < WifiNetworks; ++i) {
    String wifiSSID = WiFi.SSID(i);
    if (wifiSSID == essid) {
      rssi = WiFi.RSSI(i);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
      MAC = WiFi.BSSID(i);  // Gets the MAC address of the router
      foundChannel = WiFi.channel(i); // Gets the Wifi channel
      Serial.print(wifiSSID);
      Serial.print(F(", "));
      sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X", MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);
      Serial.print(str);
      Serial.print(F(", "));
      Serial.print(foundChannel);
      Serial.print(F(", "));
      Serial.println(rssi);
#endif
      if (rssi > bestRssi) {
        bestRssi = rssi;
        usedMAC = WiFi.BSSID(i);  // Gets the MAC address of the router
        usedChannel = WiFi.channel(i); // Gets the Wifi channel
      }
    }
  }
  if (usedChannel == 0) { // Network not found
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("Network "));
    Serial.print(essid);
    Serial.println(F(" not found!"));
#endif
  } else {
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("Try connect to "));
    Serial.print(essid);
    Serial.print(F(", "));
    sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X", usedMAC[0], usedMAC[1], usedMAC[2], usedMAC[3], usedMAC[4], usedMAC[5]);
    Serial.print(str);
    Serial.print(F(", "));
    Serial.print(usedChannel);
    Serial.print(F(", "));
    Serial.println(bestRssi);
#endif
    // wl_status_t begin(const char* ssid, const char *passphrase = NULL, int32_t channel = 0, const uint8_t* bssid = NULL, bool connect = true);
    // wl_status_t begin(char* ssid, char *passphrase = NULL, int32_t channel = 0, const uint8_t* bssid = NULL, bool connect = true);
    // wl_status_t begin(const String& ssid, const String& passphrase = emptyString, int32_t channel = 0, const uint8_t* bssid = NULL, bool connect = true);
    WiFi.begin(essid.c_str(), epass.c_str(), usedChannel, usedMAC);
    // ########## End new for connect to AP with best RSSI ##########
  }
  WiFi.scanDelete(); // Delete the last scan result from memory.
}

// Start Accesspoint
void setupAP(void) {
  digitalWrite(LED_yellow, LOW);                      // LED ein
  digitalWrite(LED_green, HIGH);                    // LED aus
  webtype = 1;                                        // Accesspoint
  MqttConnect = false;                                // MQTT aus
  estart = 0;                                 // Zähler zurück setzen
  EEPROM_write_byte(EEPROM_ADDR_START, estart);      // write Start Count to EEPROM
  char* chrOwnHostname = &OwnStationHostname[0];    // create ssid for AP
  WiFi.mode(WIFI_AP);
  WiFi.softAP(chrOwnHostname);                      // Start Accesspoint
  String logtext = F("Wifi Access Point ");
  logtext += OwnStationHostname;
  logtext += F(" started");
  Serial.println(logtext);
  appendLogFile(logtext);
  Serial.print(F("SoftAP IP: "));
  Serial.println(WiFi.softAPIP());
  createWebServer();
}

// Start Wifi WPS Connection
bool start_WPS_connect() {                // Start Wifi WPS Connection
  digitalWrite(LED_red, LOW);             // LED ein
  digitalWrite(LED_green, HIGH);          // LED aus
  digitalWrite(LED_yellow, HIGH);         // LED aus
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);                    // Station
  String logText = F("Wifi WPS started");
  appendLogFile(logText);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.println(logText);
#endif
  wpsSuccess = WiFi.beginWPSConfig(); // timeout nach ca. 20 Sekunden, gibt aber trotzdem 1 zurück
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.print(F("WPS wpsSuccess: ")); Serial.println(wpsSuccess);
#endif
  int8_t connectResult = WiFi.waitForConnectResult(); // Timeout default 60 Sekunden
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.print(F("WPS waitForConnectResult: ")); Serial.println(connectResult);
  Serial.print(F("WPS WiFi-Status: ")); Serial.print(WiFi.status());
  Serial.print(F(" (")); Serial.print(strWifiStatus[WiFi.status()]); Serial.println(')');
#endif
  if (wpsSuccess) {
    if (connectResult == 3) { // WL_CONNECTED
      String qssid = WiFi.SSID();
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
      Serial.print(F("WPS SSID: ")); Serial.println(qssid);
      Serial.print(F("WPS PSK:  ")); Serial.println(WiFi.psk());
#endif
      digitalWrite(LED_green, LOW);       // LED ein
      digitalWrite(LED_red, HIGH);        // LED aus
      webtype = 0;                        // Station
      // Well this means not always success :-/ in case of a timeout we have an empty ssid
      if (qssid.length() > 0) {
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
        Serial.print(F("WPS connected to: ")); Serial.println(qssid);
#endif
        char* chrOwnHostname = &OwnStationHostname[0];
        wifi_station_set_hostname(chrOwnHostname);
        httpUpdater.setup(&server);
        createWebServer();
        logText = F("Wifi WPS successful");
        appendLogFile(logText);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
        Serial.println(logText);
        Serial.print(F("Local IP: ")); Serial.println(WiFi.localIP());
#endif
      }
    } else if (connectResult < 0) { // -1 = Timeout
      wpsSuccess = false;
      logText = F("Wifi WPS timeout");
      appendLogFile(logText);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
      Serial.println(logText);
#endif
    } else { // ERROR
      wpsSuccess = false;
      logText = F("Wifi WPS ERROR ");
      logText += connectResult;
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
      logText += F(" (");
      logText += strWifiStatus[connectResult];
      logText += ')';
#endif
      appendLogFile(logText);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
      Serial.println(logText);
#endif
    }
  }
  return wpsSuccess;
}

bool testWifi(void) {
  int c = 0;
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.println(F("Wifi waiting to connect"));
  Serial.print(F("Status: "));
#endif
  while ( c < 120 ) {                                     // 120 = 1 Minute
    //while ( c < 1200 ) {                                // 1200 = 10 Minuten
    if (WiFi.status() == WL_CONNECTED) {
      ConnectWifi = true;
      reconnCount += 1;                                 // Counter Reconnects erhöhen
      String logtext = F("Wifi connected to ");
      logtext += WiFi.SSID();
      logtext += F(", channel ");
      logtext += WiFi.channel();
      appendLogFile(logtext);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
      Serial.println(F(""));
      Serial.println(logtext);
      Serial.print(F("Local IP: "));
      Serial.println(WiFi.localIP());
#endif
      digitalWrite(LED_green, LOW);                       // LED ein
      return true;
    }
    digitalWrite(LED_green, !digitalRead(LED_green));     // LED toggle
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("WiFi-Status: ")); Serial.print(WiFi.status()); Serial.print(F(" ("));  Serial.print(strWifiStatus[WiFi.status()]); Serial.println(')'); // 3 = connected
#endif
    c++;
    delay(500);
  }
  String logtext = F("Wifi connect timed out");
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.println("");
  Serial.println(logtext);
#endif
  appendLogFile(logtext);
  digitalWrite(LED_green, HIGH) ;   // LED aus
  return false;
}

void createWebServer() {
  server.on ( "/", SiteIndex);
  server.on ( "/index.htm", SiteIndex);
  server.on ( "/index.html", SiteIndex);
  server.on ( "/diagramm_d.htm", SiteDiagramm_d);
  server.on ( "/diagramm_m.htm", SiteDiagramm_m);
  server.on ( "/diagramm_j.htm", SiteDiagramm_j);
  server.on ( "/diagramm_jahre.htm", SiteDiagramm_jahre);
  server.on ( "/info.htm", SiteInfo);
  server.on ( "/files.htm", SiteFiles);
  server.on ( "/statistik.htm", SiteStatistik );
  server.on ( "/log.htm", SiteLogSystem);
  server.on ( "/log_d.htm", SiteLogTag);
  server.on ( "/log_m.htm", SiteLogMonat);
  server.on ( "/log_j.htm", SiteLogJahr);
  server.on ( "/setup.htm", SiteSetup);
  server.on ( "/setuptime.htm", SiteSetupTime);
  server.on ( "/setuplan.htm", SiteSetupWifi);
  server.on ( "/setupmqtt.htm", SiteSetupMqtt);
  server.on ("/eepromclear", []() {
    digitalWrite(LED_red, LOW) ;     // LED ein
    String sResponse = F("<!DOCTYPE HTML>\r\n<html>");
    sResponse += F("<p>Clearing SSID and Password from the EEPROM</p></html>");
    server.send(200, "text/html", sResponse);
    Serial.println(F("Clearing SSID and Password from EEPROM"));
    EEPROM_write_string(EEPROM_ADDR_SSID, "");   // write String to EEPROM
    EEPROM_write_string(EEPROM_ADDR_PASS, "");   // write String to EEPROM
    digitalWrite(LED_red, HIGH) ;     // LED aus
  });
  server.on ("/eepromclearall", []() {
    digitalWrite(LED_red, LOW) ;     // LED ein
    String sResponse = F("<!DOCTYPE HTML>\r\n<html>");
    sResponse += F("<p>Clearing EEPROM all</p></html>");
    server.send(200, "text/html", sResponse);
    Serial.println(F("Clearing EEPROM all"));
    for (int i = 0; i < EEPROM_MAX_ADDR; ++i) {
      EEPROM.write(i, 255);
    }
    EEPROM.commit();
    digitalWrite(LED_red, HIGH) ;     // LED aus
  });
#ifdef DEBUG_OUTPUT_SERIAL_EEPROM
  server.on ("/eepromread", []() {
    digitalWrite(LED_red, LOW) ;     // LED ein
    String sResponse = F("<!DOCTYPE HTML>\r\n<html>");
    sResponse += F("<p>Read the EEPROM</p></html>");
    server.send(200, "text/html", sResponse);
    eeprom_read_table();
    digitalWrite(LED_red, HIGH) ;     // LED aus
  });
#endif
#if defined DEBUG_OUTPUT_SERIAL_DS1307
  server.on ("/ds1307read", []() {
    digitalWrite(LED_red, LOW) ;     // LED ein
    String sResponse = F("<!DOCTYPE HTML>\r\n<html>");
    sResponse += F("<p>Read the DS1307 RAM</p></html>");
    server.send(200, "text/html", sResponse);
    DS1307_read_table();
    digitalWrite(LED_red, HIGH) ;     // LED aus
  });
#endif
  server.on ("/ds1307clearram", []() {
    digitalWrite(LED_red, LOW) ;     // LED ein
    String sResponse = F("<!DOCTYPE HTML>\r\n<html>");
    sResponse += F("<p>Clear the DS1307 RAM</p></html>");
    server.send(200, "text/html", sResponse);
    DS1307_clear_ram();
    digitalWrite(LED_red, HIGH) ;     // LED aus
  });
  server.on ("/restart", []() {
    digitalWrite(LED_red, LOW) ;     // LED ein
    String sResponse = F("<!DOCTYPE HTML>\r\n<html>");
    sResponse += F("<head><meta http-equiv='refresh' content='5; URL=/'></head><body>");
    sResponse += F("<a href='index.htm'>Home</a><br>");
    sResponse += F("<p>Restart ESP8266...</p></body></html>");
    server.send(200, "text/html", sResponse);
    appendLogFile("GZ16 ESP8266 Restart initialized by web");
    ESP.restart();
  });

  // serves all LittleFS with max-age control
  // void serveStatic (const char *uri, fs::FS &fs, const char *path, const char *cache_header=NULL)
  //  server.serveStatic("/favicon.ico", LittleFS, "/favicon.ico", "max-age=31536000, immutable");
  //  server.serveStatic("/robots.txt", LittleFS, "/robots.txt", "max-age=31536000, immutable");
  //  server.serveStatic("/css", LittleFS, "/css", "max-age=31536000, immutable");
  //  server.serveStatic("/htm", LittleFS, "/htm", "no-cache");
  //  server.serveStatic("/pic", LittleFS, "/pic", "max-age=31536000, immutable");
  //  server.serveStatic("/pdf", LittleFS, "/pdf", "max-age=31536000, immutable");
  // Lenkt die Abfrage unbekanter Dateien auf handleUnknown
  server.onNotFound(handleUnknown);

  // Start the server
  server.begin();
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.print(F("Webserver on "));
  Serial.print(WiFi.hostname());
  Serial.println(F(" started"));
#endif
}

String getContentType(String filename) {
  if (server.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html; charset=utf-8";
  //else if (filename.endsWith(".html")) return "text/html; charset=utf-8";
  else if (filename.endsWith(".css")) return "text/css; charset=utf-8";
  //else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  //else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".svg")) return "image/svg+xml; charset=utf-8";
  //else if (filename.endsWith(".xml")) return "text/xml";
  //else if (filename.endsWith(".pdf")) return "application/x-pdf";
  //else if (filename.endsWith(".zip")) return "application/x-zip";
  //else if (filename.endsWith(".gz")) return "application/x-gzip";
  else if (filename.endsWith(".log")) return "application/octet-stream";
  else if (filename.endsWith(".pdf")) return "application/octet-stream";
  return "text/plain";
}

// Es wird versucht, die angegebene Datei aus dem LittleFS hochzuladen
void handleUnknown() {
  digitalWrite(LED_red, LOW) ;     // LED ein
  String filename = server.uri();
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.print(F("HTTP requested file: "));
  Serial.println(filename);
#endif
  File pageFile = LittleFS.open(filename, "r");
  if (pageFile) {
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    uint32_t fsizeDisk = pageFile.size();
    Serial.print(F("HTTP requested file size: "));
    Serial.print(fsizeDisk);
    Serial.println(F(" bytes"));
#endif
    String contentTyp = getContentType(filename);
    //server.sendHeader("Content-Length", (String)(fsizeDisk));
    if (filename.endsWith("htm")) {
      server.sendHeader(F("cache-control"), F("private, no-cache")); // A 'cache-control' header is missing or empty.
    } else {
      server.sendHeader(F("cache-control"), F("max-age=31536000, immutable")); // Cache-Control: max-age=31536000, immutable
    }
    server.sendHeader(F("X-Content-Type-Options"), F("nosniff")); // X-Content-Type-Options: nosniff
    //server.sendHeader(F("Connection"), F("close"));
    size_t sent = server.streamFile(pageFile, contentTyp);
    //delay(200);
    pageFile.close();
    ulWifiTxBytes += sent;
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("HTTP TX file: "));
    Serial.print(filename);
    Serial.print(F(", Bytes: "));
    Serial.print(sent);
    Serial.print(F(", Free Heap: "));
    uint32_t uis = ESP.getFreeHeap();
    Serial.println(formatBytes(uis).c_str());
#endif
  } else {
    Site404();
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.println(F("HTTP requested file not found!"));
#endif
  }
  digitalWrite(LED_red, HIGH) ;     // LED aus
}
