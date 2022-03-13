//connect to Wifi in Station Mode
void startStation() {            //connect to Wifi in Station Mode
  webtype = 0;                          // Station
  WiFi.mode(WIFI_STA);                  // Station
  if (edhcp == false) {                 // DHCP aus
    Serial.println(F("DHCP off, use static IP"));
    //WiFi.config(eip, edns, esgw, esnm);     // Style Arduino
    WiFi.config(eip, esgw, esnm, edns);       // Style ESP8266
  } else {                              // DHCP ein
    Serial.println(F("DHCP on"));
    wifi_station_dhcpc_start();
  }
  WiFi.begin(essid.c_str(), epass.c_str());
  char* chrOwnHostname = &OwnStationHostname[0];
  wifi_station_set_hostname(chrOwnHostname);
  MDNS.begin(chrOwnHostname);
  httpUpdater.setup(&server);
  MDNS.addService("http", "tcp", 80);
  createWebServer();
  client.setServer(eMqttBroker.c_str(), eMqttPort); // Sets the server details.
  //client.setCallback(callback);
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
bool start_WPS_connect() {              // Start Wifi WPS Connection
  digitalWrite(LED_red, LOW);                      // LED ein
  digitalWrite(LED_green, HIGH);                    // LED aus
  digitalWrite(LED_yellow, HIGH);                    // LED aus
  WiFi.mode(WIFI_STA);                    // Station
  String logText = F("Wifi WPS started");
  appendLogFile(logText);
  Serial.println(logText);
  bool wpsSuccess = WiFi.beginWPSConfig();
  if (wpsSuccess) {
    digitalWrite(LED_green, LOW);                       // LED ein
    digitalWrite(LED_red, HIGH);                    // LED aus
    webtype = 0;                        // Station
    // Well this means not always success :-/ in case of a timeout we have an empty ssid
    String qssid = WiFi.SSID();
    String qpass = WiFi.psk();
    if ((qssid.length() > 0) && (qssid.length() > 0)) {
      // WPSConfig has already connected in STA mode successfully to the new station.
      char* chrOwnHostname = &OwnStationHostname[0];
      wifi_station_set_hostname(chrOwnHostname);
      MDNS.begin(chrOwnHostname);
      httpUpdater.setup(&server);
      MDNS.addService("http", "tcp", 80);
      createWebServer();
      client.setServer(eMqttBroker.c_str(), eMqttPort); // Sets the server details.
      //client.setCallback(callback);
      logText = F("Wifi WPS connected to: ");
      logText += qssid;
      appendLogFile(logText);
      Serial.println(logText);
      Serial.print(F("Local IP: "));
      Serial.println(WiFi.localIP());
      essid = qssid;                                // SSID übernehmen
      epass = qpass;                                // Wifi Passwort übernehmen
      EEPROM_write_string(EEPROM_ADDR_SSID, essid);   // write String to EEPROM
      EEPROM_write_string(EEPROM_ADDR_PASS, epass);   // write String to EEPROM
      appendLogFile(F("Wifi settings saved"));
    } else {
      wpsSuccess = false;
    }
  }
  return wpsSuccess;
}
/*
   https://www.bountysource.com/issues/36284780-_trywps-true
   If I set _tryWPS = true;
   and the WPS fails then the configuration of the access point doen't have an IP

   this is exactly why i have not made the function public,
   have not found a way to make it work if it fails to connect yet

   Lösung mit Neustart?
*/

bool testWifi(void) {
  int c = 0;
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.println(F("Wifi waiting to connect"));
    Serial.print(F("Status: "));
  }
  while ( c < 120 ) {                                     // 120 = 1 Minute
    //while ( c < 1200 ) {                                // 1200 = 10 Minuten
    if (WiFi.status() == WL_CONNECTED) {
      ConnectWifi = true;
      ulReconncount += 1;                                 // Counter Reconnects erhöhen
      String logtext = F("Wifi connected to ");
      logtext += WiFi.SSID();
      logtext += F(", channel ");
      logtext += WiFi.channel();
      appendLogFile(logtext);
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        Serial.println(F(""));
        Serial.println(logtext);
        Serial.print(F("Local IP: "));
        Serial.println(WiFi.localIP());
      }
      digitalWrite(LED_green, LOW);                       // LED ein
      return true;
    }
    digitalWrite(LED_green, !digitalRead(LED_green));     // LED toggle
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(WiFi.status());
    }
    c++;
    delay(500);
  }
  String logtext = F("Wifi connect timed out");
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.println("");
    Serial.println(logtext);
  }
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
  server.on ("/eepromread", []() {
    digitalWrite(LED_red, LOW) ;     // LED ein
    String sResponse = F("<!DOCTYPE HTML>\r\n<html>");
    sResponse += F("<p>Read the EEPROM</p></html>");
    server.send(200, "text/html", sResponse);
    eeprom_read_table();
    digitalWrite(LED_red, HIGH) ;     // LED aus
  });
  server.on ("/ds1307read", []() {
    digitalWrite(LED_red, LOW) ;     // LED ein
    String sResponse = F("<!DOCTYPE HTML>\r\n<html>");
    sResponse += F("<p>Read the DS1307 RAM</p></html>");
    server.send(200, "text/html", sResponse);
    DS1307_read_table();
    digitalWrite(LED_red, HIGH) ;     // LED aus
  });
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

  // serves all LittleFS with 24hr max-age control
  server.serveStatic("/pdf", LittleFS, "/pdf", "max-age=86400");
  server.serveStatic("/pic", LittleFS, "/pic", "max-age=86400");
  server.serveStatic("/static", LittleFS, "/static" , "max-age=86460");

  // Lenkt die Abfrage unbekanter Dateien auf handleUnknown
  server.onNotFound(handleUnknown);

  // Start the server
  server.begin();
  Serial.print(F("Webserver on "));
  Serial.print(WiFi.hostname());
  Serial.println(F(" started"));
}

String getContentType(String filename) {
  if (server.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html; charset=utf-8";
  else if (filename.endsWith(".html")) return "text/html; charset=utf-8";
  else if (filename.endsWith(".css")) return "text/css";
  //else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
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
  File pageFile = LittleFS.open(filename, "r");
  if (pageFile)  {
    // String contentTyp = StaticRequestHandler::getContentType(filename);
    String contentTyp = getContentType(filename);
    size_t sent = server.streamFile(pageFile, contentTyp);
    pageFile.close();
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(F("HTTP TX Bytes: "));
      Serial.print(sent);
      Serial.print(F(", Free Heap: "));
      uint32_t uis = ESP.getFreeHeap();
      Serial.println(formatBytes(uis).c_str());
    }
  } else {
    Site404();
  }
  digitalWrite(LED_red, HIGH) ;     // LED aus
}
