const char* strDir[6] PROGMEM = {"", "css", "htm", "log", "pic", "pdf"};

// ############################## Statistik ##############################
void SiteStatistik() {
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  SerialPrintLine();
#endif
  String sResponse = "";                        // Response HTML
  sResponse.reserve(4096); // ohne: Dauer: 74829 µS, 2585 Byte, Free Heap: 29.20 kB
  int c = 0;                                    // Counter für Anzeige Zeilen
  insertHeaderCSS(sResponse);                   // Header und CCS einfügen
  insertMenu_Index(sResponse);                  // Index Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);           // Überschrift Tabelle Zeile 2 einfügen

  // Tabelle Zeile 3 einfuegen
  sResponse += F("<tr><td class=\"CXB\" colspan=\"5\">Maximalverbrauch</td></tr>");

  // Tabelle Zeile 4, 5, 6 einfuegen (Maximalwerte)
  unsigned long l = DS1307_read_long(DS1307_ADDR_MAXS0DAYTIME);        // read timestamp Maximum Tag
  if (l != 0xFFFFFFFF) {                                               // Zeit bereits geschrieben
    c += 1;                                                            // Counter erhöhen
    sResponse += F("<tr><td class=\"rx\">pro Tag</td>"
                   "<td class=\"rx\" colspan=\"2\">");
    sResponse += (DateToString(l));
    sResponse += F("</td>"
                   "<td class=\"rx\">");
    sResponse += longToStr(s0_count_max_day);                          // read max S0-Count day
    sResponse += F(" m³</td>"
                   "<td class=\"lx\">&nbsp;</td></tr>");
  }
  l = DS1307_read_long(DS1307_ADDR_MAXS0MONTHTIME);                    // read timestamp Maximum month
  if (l != 0xFFFFFFFF) {                                               // Zeit bereits geschrieben
    c += 1;                                                            // Counter erhöhen
    sResponse += F("<tr><td class=\"rx\">pro Monat</td>"
                   "<td class=\"rx\" colspan=\"2\">");
    sResponse += (month(l));
    sResponse += F("/");
    sResponse += (year(l));
    sResponse += F("</td>"
                   "<td class=\"rx\">");
    sResponse += longToStr(s0_count_max_month);                         // read max S0-Count month
    sResponse += F(" m³</td>"
                   "<td class=\"lx\">&nbsp;</td></tr>");
  }
  l = DS1307_read_long(DS1307_ADDR_MAXS0YEARTIME);                      // read timestamp Maximum year
  if (l != 0xFFFFFFFF) {                                                // Zeit bereits geschrieben
    c += 1;                                                             // Counter erhöhen
    sResponse += F("<tr><td class=\"rx\">pro Jahr</td>"
                   "<td class=\"rx\" colspan=\"2\">");
    sResponse += (year(l));
    sResponse += F("</td>"
                   "<td class=\"rx\">");
    sResponse += longToStr(s0_count_max_year);                          // read max S0-Count year
    sResponse += F(" m³</td>"
                   "<td class=\"lx\">&nbsp;</td></tr>");
  }
  if (c == 0) {                                                         // noch keine Daten geschrieben
    sResponse += F("<tr><td class=\"c\" colspan=\"5\">"
                   "<br><br>Noch keine Daten vorhanden!<br><br><br>"
                   "</td></tr>");
  }
  // Tabelle Zeile 7 einfuegen
  sResponse += F("<tr><td class=\"CXB\" colspan=\"5\">Jahreszeiten</td></tr>");
  // Tabelle Zeile 8, 9, 10, 11 einfuegen (Jahreszeiten)
  c = 0;                                                      // Reset Counter für Anzeige Zeilen
  if (day() > 15) {
    l = now() - 28944000;         // - 11 Monate (335 Tage)
  } else {
    l = now() - 28857600;         // - 11 Monate (334 Tage)
  }
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.print(F("heute -11 Monate: "));
  Serial.println(DateToString(l));
#endif
  while (month(l) % 3 != 0) {     // Prüfung Monat durch 3 teilbar
    if (day(l) < 15) {
      l = l + 2678400;            // + 1 Monat (31 Tage)
    } else {
      l = l + 2419200;            // + 1 Monat (28 Tage)
    }
  }
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.print(F("Beginn Jahreszeit: "));
  Serial.println(DateToString(l));
#endif
  String Line;
  int daint = 0;                            // Integer Tag
  int moint = 0;                            // Integer Monat
  int yeint = 0;                            // Integer Year
  String datum;                             // String Datum
  int Space2;
  int Space3;
  long s0_count_int = 0;
  long s0_count_avg_int = 0;                 // S0-Counter Durchschnitt
  long s0_count_ges_int = 0;                 // S0-Counter gesamt
  byte monat = month(l);                     // Monat Beginn
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.print(F("Monat Beginn: "));
  Serial.println(monat);
#endif
  int tage = 0;
  String FileName = (F("/log/y_"));
  FileName += (year(l));
  FileName += (F(".log"));
nocheinmal:               // Sprungmarke für 2. Durchlauf bei 2 Jahren
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.print(F("Datei: "));
  Serial.println(FileName);
#endif
  if (LittleFS.exists(FileName)) {    // Returns true if a file with given path exists, false otherwise.
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("LittleFS Reading Data from: "));
    Serial.println(FileName);
#endif
    File LogFile = LittleFS.open(FileName, "r");      // Open text file for reading.
    while (LogFile.available()) {
      Line = LogFile.readStringUntil('\n');         // Lets read line by line from the file
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
      Serial.println(Line);
#endif
      daint = Line.substring(0, 2).toInt();         // Integer Tag
      moint = Line.substring(3, 5).toInt();         // Integer Monat
      yeint = Line.substring(6, 10).toInt();        // Integer Year
      datum = Line.substring(0, 10);                // String Datum
      Space2 = Line.indexOf(' ', 19);               // nach Uhrzeit folgt Count absolut
      Space3 = Line.indexOf(' ', Space2 + 1 );      // nach absulut Count folgt Count
      s0_count_int = Line.substring(Space3 + 1).toInt();
      while (moint > (monat + 2)) {
        monat += 3;
      }
      if (moint >= monat && moint <= monat + 2) {
        s0_count_ges_int = s0_count_ges_int + s0_count_int;
        tage = tage + daint;
      }
      if (moint == monat + 2) {                     // Jahreszeit
        sResponse = WebsiteStatistikJahreszeiten(sResponse, monat, yeint);
        //sResponse = WebsiteStatistikJahreszeiten(sResponse, monat, ye);
        // sResponse = WebsiteStatistikJahreszeiten(sResponse, monat, s0_count_ges_int, tage, ye);
        // Ende Zeitraum einfügen
        sResponse += F(" - ");
        sResponse += datum;                                     // Datum einfügen
        sResponse += F("</td>"
                       "<td class=\"rx\">");
        sResponse += longToStr(s0_count_ges_int);               // Gesamtverbrauch einfügen
        sResponse += F(" m³</td>"
                       "<td class=\"rx\">");
        s0_count_avg_int = s0_count_ges_int / tage;
        sResponse += longToStr(s0_count_avg_int);               // Durchschnitt Verbrauch pro Tag einfügen
        sResponse += F(" m³/d&nbsp;</td></tr>");
        s0_count_ges_int = 0;   // Zähler zurück setzen
        tage = 0;               // Anzahl Tage zurück setzen
      }
    }
    LogFile.close();
  } else {                          // Datei Vorjahr existiert nicht
    yeint = year();                 // aktuelles Jahr
    monat = 0;
    FileName = (F("/log/y_"));
    FileName += (year());           // aktuelles Jahr
    FileName += (F(".log"));
    if (LittleFS.exists(FileName)) {  // Returns true if a file with given path exists, false otherwise.
      goto nocheinmal;              // 2. Durchlauf starten
    } else {                        // Datei aktuelles Jahr existiert auch nicht
      monat = month();              // aktuellen Monat übernehmen
      yeint = year();                 // aktuelles Jahr
      //ye = year();
    }
  }
  // wenn über 2 Jahre, 2. Durchlauf starten
  if (yeint < year()) {
    monat = 0;
    FileName = (F("/log/y_"));
    FileName += (year());     // aktuelles Jahr
    FileName += (F(".log"));
    goto nocheinmal;
  }
  // aktuelle Jahreszeit zuletzt
  s0_count_ges_int = s0_count_ges_int + s0_count_month;   // Counter aktuell addieren
  tage = tage + day();                                    // Tage addieren
  sResponse = WebsiteStatistikJahreszeiten(sResponse, month(), yeint);
  //  sResponse = WebsiteStatistikJahreszeiten(sResponse, month(), ye);
  // sResponse = WebsiteStatistikJahreszeiten(sResponse, month(), s0_count_ges_int, tage, ye);
  // Ende Zeitraum einfügen
  sResponse += F(" - ");
  sResponse += DateToString(now());                       // Datum einfügen
  sResponse += F("</td>"
                 "<td class=\"rx\">");
  sResponse += longToStr(s0_count_ges_int);               // Gesamtverbrauch einfügen
  sResponse += F(" m³</td>"
                 "<td class=\"rx\">");
  s0_count_avg_int = s0_count_ges_int / tage;
  sResponse += longToStr(s0_count_avg_int);               // Durchschnitt Verbrauch pro Tag einfügen
  sResponse += F(" m³/d&nbsp;</td></tr>");

  // Footer anhängen und senden
  insertFooterSend(sResponse);                                      // Footer anhängen und senden
}

// ############################## Einstellungen diverse anzeigen ##############################
void SiteSetup()  {
  String sResponse = "";                        // Response HTML
  sResponse.reserve(4096); // ohne: Dauer: 2947 µS, 2643 Byte2, Free Heap: 28.37 kB
  String submit = server.arg("submit");         // welcher Button wurde betätigt
  String logtext = "";
  int countargs = server.args();                // Anzahl Argumente
  if (countargs != 0) {
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    SerialPrintLine();                        // Trennlinie seriell ausgeben
    Serial.print(F("Button wurde betaetigt(submit): "));
    Serial.println(submit);
    Serial.print(F("Anzahl Argumente(countargs): "));
    Serial.println(countargs);
#endif
    if (submit == "saves0count") {                        // Button "Zählerstand speichern" betätigt
      String qs0count = server.arg("s0count");            // S0-Count
      // Prüfung auf Zahl einfügen
      qs0count.replace(",", ".");                         // Komma durch Punkt ersetzen
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
      Serial.print(F("S0-Count: "));
      Serial.println(qs0count);
#endif
      s0_count_abs = round(qs0count.toFloat() * 100.0);   // "toFloat" funktioniert nicht richtig, deshalb runden
      DS1307_write_long(DS1307_ADDR_S0COUNTABS, s0_count_abs);     // save S0-Counter to DS1307 RAM
      logtext = F("Set Counter to: ");
      logtext += longToStr(s0_count_abs);
      logtext += F(" m³");
    }
    if (submit == "resetMaxDay") {                                    // Button "Rückstellen" betätigt
      s0_count_max_day = 0;                                           // Maximum pro Tag löschen
      DS1307_write_long(DS1307_ADDR_MAXS0DAY, s0_count_max_day);      // Maximum pro Tag speichern
      DS1307_write_long(DS1307_ADDR_MAXS0DAYTIME, now());             // aktuelle Uhrzeit speichern
      logtext = F("Reset max consumption per day");
    }
    if (submit == "resetMaxMonth") {                                  // Button "Rückstellen" betätigt
      s0_count_max_month = 0;                                         // Maximum pro Monat löschen
      DS1307_write_long(DS1307_ADDR_MAXS0MONTH, s0_count_max_month);  // Maximum Monat speichern
      DS1307_write_long(DS1307_ADDR_MAXS0MONTHTIME, now());           // aktuelle Uhrzeit speichern
      logtext = F("Reset max consumption per month");
    }
    if (submit == "resetMaxYear") {                                   // Button "Rückstellen" betätigt
      s0_count_max_year = 0;                                          // Maximum pro Jahr löschen
      DS1307_write_long(DS1307_ADDR_MAXS0YEAR, s0_count_max_year);    // Maximum Jahr speichern
      DS1307_write_long(DS1307_ADDR_MAXS0YEARTIME, now());            // aktuelle Uhrzeit speichern
      logtext += F("Reset max consumption per year");
    }
    if (submit == "sets0trip1") {                                     // Button "Rückstellen" betätigt
      EEPROM_write_long(EEPROM_ADDR_S0TRIP1, s0_count_abs);           // S0-Count Trip begin set to S0-Count
      EEPROM_write_long(EEPROM_ADDR_S0TRIP1TIME, now());              // S0-Count Trip time set
      logtext = F("Reset Counter 1 to: ");
      logtext += longToStr(s0_count_abs);
      logtext += F(" m³");
    }
    if (submit == "sets0trip2") {                                     // Button "Rückstellen" betätigt
      EEPROM_write_long(EEPROM_ADDR_S0TRIP2, s0_count_abs);           // S0-Count Trip begin set to S0-Count
      EEPROM_write_long(EEPROM_ADDR_S0TRIP2TIME, now());              // S0-Count Trip time set
      logtext = F("Reset Counter 2 to: ");
      logtext += longToStr(s0_count_abs);
      logtext += F(" m³");
    }
    if (submit == "setser") {
      SerialOutput = !SerialOutput;// toggle boolean
      EEPROM_write_byte(EEPROM_ADDR_SERIALOUTPUT, SerialOutput);                        // write byte at address
      logtext = F("Serial Output: ");
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        logtext += F("on");
      } else {                    // serielle Ausgabe ausgeschaltet
        logtext += F("off");
      }
    }
    appendLogFile(logtext);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.println(logtext);
#endif
  }

  // Beginn HTML
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/button.css\">");
  insertMenu_Setup(sResponse);                       // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);                // Überschrift Tabelle Zeile 2 einfügen
  // Zeile 3 einfügen
  sResponse += F("<tr><td class=\"CXB\" colspan=\"5\">diverse Einstellungen</td></tr>");
  // Tabelle Zeile 4 einfügen
  sResponse += F("<tr>"
                 "<td colspan=\"2\" class=\"r\">Zählerstand:</td>"
                 "<td class=\"r\"><input type=number aria-label=\"Zählerstand\" min=0.00 max=99999.99 step=0.01 name=\"s0count\" maxlength=\"8\" value=\"");
  sResponse += s0_count_abs / 100.0;
  //  sResponse += longToStr(s0_count_abs);
  sResponse += F("\"></td>"
                 "<td>m³</td>"
                 "<td><button type=\"submit\" name=\"submit\" value=\"saves0count\">Speichern</button></td>"
                 "</tr>"
                 // Tabelle Zeile 5 einfügen
                 "<tr><td colspan=\"2\" class=\"r\">Maximalverbrauch pro Tag:</td>"
                 "<td class=\"r\">");
  sResponse += longToStr(s0_count_max_day);    // Maximum pro Tag
  sResponse += F("</td>"
                 "<td>m³</td>"
                 "<td><button type=\"submit\" name=\"submit\" value=\"resetMaxDay\">Rückstellen</button></td>"
                 "</tr>"
                 // Tabelle Zeile 6 einfügen
                 "<tr><td colspan=\"2\" class=\"r\">Maximalverbrauch pro Monat:</td>"
                 "<td class=\"r\">");
  sResponse += longToStr(s0_count_max_month);    // Maximum pro Monat
  sResponse += F("</td>"
                 "<td>m³</td>"
                 "<td><button type=\"submit\" name=\"submit\" value=\"resetMaxMonth\">Rückstellen</button></td>"
                 "</tr>"
                 // Tabelle Zeile 7 einfügen
                 "<tr><td colspan=\"2\" class=\"r\">Maximalverbrauch pro Jahr:</td>"
                 "<td class=\"r\">");
  sResponse += longToStr(s0_count_max_year);    // Maximum pro Jahr
  sResponse += F("</td>"
                 "<td>m³</td>"
                 "<td><button type=\"submit\" name=\"submit\" value=\"resetMaxYear\">Rückstellen</button></td>"
                 "</tr>"
                 // Tabelle Zeile 8 einfügen
                 "<tr><td colspan=\"2\" class=\"r\">Verbrauch seit ");
  unsigned long l = EEPROM_read_long(EEPROM_ADDR_S0TRIP1TIME);            // read timestamp
  sResponse += DateToString(l);     // return Date String from Timestamp
  sResponse += F(", ");
  sResponse += TimeToStringHM(l);     // return Time String from Timestamp
  sResponse += F(" Uhr:</td>"
                 "<td class=\"r\">");
  l = EEPROM_read_long(EEPROM_ADDR_S0TRIP1);                  // read S0-Count Trip begin
  sResponse += longToStr(s0_count_abs - l);
  sResponse += F("</td>"
                 "<td>m³</td>"
                 "<td><button type=\"submit\" name=\"submit\" value=\"sets0trip1\">Rückstellen</button></td>"
                 "</tr>"
                 // Tabelle Zeile 9 einfügen
                 "<tr><td colspan=\"2\" class=\"r\">Verbrauch seit ");
  l = EEPROM_read_long(EEPROM_ADDR_S0TRIP2TIME);            // read timestamp
  sResponse += DateToString(l);     // return Date String from Timestamp
  sResponse += F(", ");
  sResponse += TimeToStringHM(l);     // return Time String from Timestamp
  sResponse += F(" Uhr:</td>"
                 "<td class=\"r\">");
  l = EEPROM_read_long(EEPROM_ADDR_S0TRIP2);                  // read S0-Count Trip begin
  sResponse += longToStr(s0_count_abs - l);
  sResponse += F("</td>"
                 "<td>m³</td>"
                 "<td><button type=\"submit\" name=\"submit\" value=\"sets0trip2\">Rückstellen</button></td>"
                 "</tr>"
                 // Tabelle Zeile 10 einfügen
                 "<tr><td colspan=\"2\" class=\"r\">serielle Ausgabe:</td>"
                 "<td class=\"r\">");
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    sResponse += F("ein");
  } else {                    // serielle Ausgabe ausgeschaltet
    sResponse += F("aus");
  }
  sResponse += F("</td>"
                 "<td>&nbsp;</td>"
                 "<td><button type=\"submit\" name=\"submit\" value=\"setser\">");
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    sResponse += F("ausschalten");
  } else {                    // serielle Ausgabe ausgeschaltet
    sResponse += F("einschalten");
  }
  sResponse += F("</button></td></tr>");

  // Footer anhängen und senden
  insertFooterSend(sResponse);                       // Footer anhängen und senden
}

// ############################## Einstellungen MQTT anzeigen ##############################
void SiteSetupMqtt()  {
  String sResponse = "";                        // Response HTML
  sResponse.reserve(4096); // ohne: Dauer: 2425 µS, 3162 Byte, Free Heap: 27.27 kB
  String qbroker = server.arg("broker");        // Broker
  String quser = server.arg("user");            // Username
  String qpass = server.arg("pass");            // Passwort
  String qport = server.arg("port");            // Port
  String qabs = server.arg("abs");              // Zählerstand
  String qmom = server.arg("mom");              // Momentanverbrauch
  String qrssi = server.arg("rssi");            // WLAN-RSSI
  String qrecon = server.arg("recon");          // WLAN-Reconnects
  String qivall = server.arg("ivall");          // Intervall
  String submit = server.arg("submit");         // welcher Button wurde betätigt
  int countargs = server.args();                // Anzahl Argumente
  if (countargs != 0) {
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    SerialPrintLine();            // Trennlinie seriell ausgeben
    Serial.print(F("MQTT-Broker: "));
    Serial.println(qbroker);
    Serial.print(F("Username: "));
    Serial.println(quser);
    Serial.print(F("Passwort: "));
    Serial.println(qpass);
    Serial.print(F("Port: "));
    Serial.println(qport);
    Serial.print(F("Intervall: "));
    Serial.println(qivall);
    Serial.print(F("welcher Button wurde betaetigt(submit): "));
    Serial.println(submit);
    Serial.print(F("Anzahl Argumente(countargs): "));
    Serial.println(countargs);
#endif
    if (submit == "connect") {                            // Button "Verbinden" betätigt
      if (qbroker.length() == 0) {                      // no MQTT Broker given
        MqttConnect = false;                           // MQTT ausschalten
        eMqttBroker = "";                            // MQTT-Broker löschen
        String logtext = F("MQTT disabled");
        appendLogFile(logtext);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
        Serial.println(logtext);
#endif
        if (client.connected()) {                           // MQTT Broker connected
          client.disconnect();                                // Disconnects the client
        }
        EEPROM_write_boolean(EEPROM_ADDR_MQTTCONNECT, MqttConnect);  // write MQTT-Connect
        EEPROM_write_string(EEPROM_ADDR_MQTTBROKER, eMqttBroker);    // write String to EEPROM
      } else {
        MqttConnect = true;                           // MQTT einschalten
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
        Serial.print(F("MQTT: Connect to broker: "));
        Serial.print(qbroker);
        Serial.print(F(" Port: "));
        Serial.println(qport);
#endif
        if (client.connected()) {                           // MQTT Broker connected
          client.disconnect();                                // Disconnects the client
        }
        client.setServer(qbroker.c_str(), qport.toInt());   // Sets the server details.
        if (quser.length() == 0) {                          // no MQTT Username
          client.connect(OwnStationHostname.c_str());       // Connects the client without authentification
        } else {
          client.connect(OwnStationHostname.c_str(), quser.c_str(), qpass.c_str()); // Connects with authentification
        }
        if (client.connected()) {                           // MQTT Broker connected
          eMqttBroker = qbroker;                            // MQTT-Broker übernehmen
          eMqttUsername = quser;                            // MQTT-Username übernehmen
          eMqttPassword = qpass;                            // MQTT-Password übernehmen
          eMqttPort = qport.toInt();                        // MQTT-Port übernehmen
          String logtext = F("MQTT Broker ");
          logtext += (eMqttBroker);
          logtext += F(" connected");
          appendLogFile(logtext);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
          Serial.println(logtext);
#endif
          client.publish(OwnStationHostname.c_str(), "Connected");      // publish "Connected"
          EEPROM_write_string(EEPROM_ADDR_MQTTBROKER, eMqttBroker);     // write String to EEPROM
          EEPROM_write_string(EEPROM_ADDR_MQTTUSERNAME, eMqttUsername); // write String to EEPROM
          EEPROM_write_string(EEPROM_ADDR_MQTTPASSWORD, eMqttPassword); // write String to EEPROM
          EEPROM_write_int(EEPROM_ADDR_MQTTPORT, eMqttPort);            // write MQTT Server Port to EEPROM
          EEPROM_write_boolean(EEPROM_ADDR_MQTTCONNECT, MqttConnect);   // write MQTT-Connect
          appendLogFile(F("MQTT server settings saved"));
        } else {                                                      // Connects the client Error
          eMqttBroker = qbroker;                            // MQTT-Broker übernehmen
          String logtext = F("MQTT connect to ");
          logtext += (qbroker);
          logtext += F(" failed");
          appendLogFile(logtext);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
          Serial.println(logtext);
#endif
        }
      }
    }
    if (submit == "save") {                             // Button "Speichern" betätigt
      if (qabs == "1") {                                // Zählerstand
        eMqttPublish_s0_count_abs = true;
      } else {
        eMqttPublish_s0_count_abs = false;
      }
      EEPROM_write_boolean(EEPROM_ADDR_MQTTPUBLISHABS, eMqttPublish_s0_count_abs);  // write boolean at address
      if (qmom == "1") {                                // Momentanverbrauch
        eMqttPublish_s0_count_mom = true;
      } else {
        eMqttPublish_s0_count_mom = false;
      }
      EEPROM_write_boolean(EEPROM_ADDR_MQTTPUBLISHMOM, eMqttPublish_s0_count_mom);  // write boolean at address
      if (qrssi == "1") {                               // WLAN-RSSI
        eMqttPublish_rssi = true;
      } else {
        eMqttPublish_rssi = false;
      }
      EEPROM_write_boolean(EEPROM_ADDR_MQTTPUBLISHRSSI, eMqttPublish_rssi);          // write boolean at address
      if (qrecon == "1") {                               // WLAN-Reconnects
        eMqttPublish_recon = true;
      } else {
        eMqttPublish_recon = false;
      }
      EEPROM_write_boolean(EEPROM_ADDR_MQTTPUBLISHRECON, eMqttPublish_recon);          // write boolean at address
      eMqttPublish_Intervall = qivall.toInt();          // Intervall
      EEPROM_write_byte(EEPROM_ADDR_MQTTINTERVALL, eMqttPublish_Intervall);          // write byte at address
      appendLogFile(F("MQTT publish settings saved"));
    }
  }
  // Beginn HTML
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/button.css\">");
  insertMenu_Setup(sResponse);                       // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);                // Überschrift Tabelle Zeile 2 einfügen
  // Zeile 3 einfügen
  sResponse += F("<tr><td class=\"CXB\" colspan=\"5\">Einstellungen MQTT Server</td></tr>"
                 // Zeile 4 einfügen
                 "<tr><td class=\"r\">MQTT-Broker:</td>"
                 "<td class=\"l\"><input name=\"broker\" autocomplete=\"on\" aria-label=\"Broker\" type=\"text\" maxlength=\"64\" value=\"");
  sResponse += eMqttBroker;
  sResponse += F("\"></td>"
                 "<td colspan=\"3\" class=\"l\">(Name oder Adresse, leer um Dienst abzuschalten)</td></tr>"
                 // Zeile 5 einfügen
                 "<tr><td class=\"r\">Username:</td>"
                 "<td colspan=\"4\" class=\"l\"><input name=\"user\" autocomplete=\"username\" aria-label=\"Username\" type=\"text\" maxlength=\"64\" value=\"");
  sResponse += eMqttUsername;
  sResponse += F("\"></td></tr>"
                 // Zeile 6 einfügen
                 "<tr><td class=\"r\">Passwort:</td>"
                 "<td colspan=\"4\" class=\"l\"><input name=\"pass\" autocomplete=\"current-password\" aria-label=\"Passwort\" type=\"password\" maxlength=\"64\" value=\"");
  sResponse += eMqttPassword;
  sResponse += F("\"></td></tr>"
                 // Zeile 7 einfügen
                 "<tr><td class=\"r\">Port:</td>"
                 "<td colspan=\"3\" class=\"l\"><input type=\"number\" name=\"port\" aria-label=\"Port\" maxlength=\"5\" value=\"");
  sResponse += eMqttPort;
  sResponse += F("\"></td>"
                 "<td><button type=\"submit\" name=\"submit\" value=\"connect\">Verbinden</button></td>"
                 "</tr>");
  // Zeile 8 einfügen
  if (webtype == 0) {                                     // Station
    if (MqttConnect == false) {                           // MQTT ausgeschaltet
      sResponse += F("<tr><td colspan=\"5\" class=\"c\">MQTT ausgeschaltet.</td></tr>");
    } else {                                              // MQTT eingeschaltet
      if (client.connected()) {                           // MQTT Broker connected
        //if (MqttConnect == true) {
        sResponse += F("<tr><td colspan=\"5\" class=\"c\">MQTT Verbindung hergestellt.</td></tr>");
      } else {                                            // MQTT Broker not connected
        sResponse += F("<tr><td colspan=\"5\" class=\"cred\">MQTT Verbindung nicht hergestellt! (Status: ");
        sResponse += (client.state());
        sResponse += F(")</td></tr>");
      }
    }
  } else {                                     // keine Netzwerkverbindung
    sResponse += F("<tr><td colspan=\"5\" class=\"cred\">Keine Verbindung mit Netzwerk!</td></tr>");
  }
  // Zeile 9 einfügen
  sResponse += F("<tr><td colspan=\"5\" class=\"CXB\">Publizierung durch Gaszähler</td></tr>"
                 // Zeile 10 einfügen
                 "<tr><td class=\"r\">Zählerstand:</td>"
                 "<td class=\"l\"><input name=\"abs\" aria-label=\"Zählerstand\" type=\"checkbox\" value=\"1\"");
  sResponse += (eMqttPublish_s0_count_abs == 1 ? " checked" : "");
  sResponse += F("></td><td colspan=\"3\" class=\"l\">");
  if (eMqttPublish_s0_count_abs == true) {
    sResponse += OwnStationHostname;
    sResponse += F("/CountAbs");
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td></tr>"
                 // Zeile 11 einfügen
                 "<tr><td class=\"r\">Momentanverbrauch:</td>"
                 "<td class=\"l\"><input name=\"mom\" aria-label=\"Momentanverbrauch\" type=\"checkbox\" value=\"1\"");
  sResponse += (eMqttPublish_s0_count_mom == 1 ? " checked" : "");
  sResponse += F("></td><td colspan=\"3\" class=\"l\">");
  if (eMqttPublish_s0_count_mom == true) {
    sResponse += OwnStationHostname;
    sResponse += F("/CountMom");
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td></tr>"
                 // Zeile 12 einfügen
                 "<tr><td class=\"r\">RSSI:</td>"
                 "<td class=\"l\"><input name=\"rssi\" aria-label=\"RSSI\" type=\"checkbox\" value=\"1\"");
  sResponse += (eMqttPublish_rssi == 1 ? " checked" : "");
  sResponse += F("></td><td colspan=\"3\" class=\"l\">");
  if (eMqttPublish_rssi == true) {
    sResponse += OwnStationHostname;
    sResponse += F("/RSSI");
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td></tr>"
                 // Zeile 13 einfügen
                 "<tr><td class=\"r\">Neuverbindungen:</td>"
                 "<td class=\"l\"><input name=\"recon\" aria-label=\"Neuverbindungen\" type=\"checkbox\" value=\"1\"");
  sResponse += (eMqttPublish_recon == 1 ? " checked" : "");
  sResponse += F("></td><td colspan=\"3\" class=\"l\">");
  if (eMqttPublish_recon == true) {
    sResponse += OwnStationHostname;
    sResponse += F("/WiFiReconnects");
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td></tr>"
                 // Zeile 14 einfügen
                 "<tr><td class=\"r\">Intervall:</td>"
                 "<td colspan=\"3\" class=\"l\">"
                 "<select name=\"ivall\" aria-label=\"Intervall\">");
  byte intervall[10] = {1, 2, 3, 5, 6, 10, 15, 20, 30, 60};
  for (int i = 0; i < 10; ++i) {
    sResponse += F("<option");
    if (intervall[i] == eMqttPublish_Intervall) {
      sResponse += F(" selected=\"\"");
    }
    sResponse += F(">");
    sResponse += intervall[i];
    sResponse += F("</option>");
  }
  sResponse += F("</select>&nbsp;Minuten</td>"
                 "<td><button type=\"submit\" name=\"submit\" value=\"save\">Speichern</button></td>"
                 "</tr>");

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## Einstellungen WLAN anzeigen ##############################
void SiteSetupWifi()  {
  String sResponse = "";                           // Response HTML
  sResponse.reserve(8192); // ohne: Dauer: 1591774 µS, 3955 Byte, Free Heap: 27.71 kB
  IPAddress ipadr;                                 // IP address
  char str[18];                                    // for sprintf IP-Address an MAC
  int errorIpAddr = 0;                             // Error IP address
  uint8_t* MAC_CON = WiFi.BSSID();                 // Gets the MAC address of the router you are connected to
  uint8_t* MAC;                                    // Gets the MAC address of the router
  uint8_t countargs = server.args();               // Anzahl Argumente
  uint8_t setssid = server.arg("setssid").toInt(); // Auswahl SSID
  String hssid = server.arg("hiddenssid");         // Auswahl versteckte SSID
  String qpass = server.arg("pw");                 // WLAN Passwort
  String qdhcp = server.arg("dhcp");               // 1-DHCP ein, 0-DHCP aus
  String qip = server.arg("ip");                   // IP-Adresse statisch
  String qdns = server.arg("dns");                 // Domain Name Server statisch
  String qsnm = server.arg("sn");                  // Subnetzmaske  statisch
  String qsgw = server.arg("gw");                  // Gateway statisch
  String submit = server.arg("submit");            // welcher Button wurde betätigt
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  SerialPrintLine();                               // Trennlinie seriell ausgeben
  Serial.print(F("Anzahl Argumente(countargs): "));
  Serial.println(countargs);
#endif
  if (submit != "wps") {   // bei WPS nicht scannen
  //if (countargs == 0 || submit == "rescan") {   // erster Aufruf ohne Argumente
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.println(F("Scanne Netzwerke..."));
#endif
    WifiNetworks = WiFi.scanNetworks();         // Scanne Netzwerke
  }
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.print(F("Anzahl gefundener Netzwerke(networks): "));
  Serial.println(WifiNetworks);

  String ssid;
  int32_t rssi;
  uint8_t encryptionType;
  uint8_t *bssid;
  int32_t channel;
  bool hidden;

  if (WifiNetworks <= 0) {
    Serial.println(F("No networks found"));
  } else if (WifiNetworks > 0) {
    Serial.printf(PSTR("%d networks found:\n"), WifiNetworks);

    // Print unsorted scan results
    for (int8_t i = 0; i < WifiNetworks; i++) {
      WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel, hidden);

      // get extra info
      const bss_info *bssInfo = WiFi.getScanInfoByIndex(i);
      String phyMode;
      const char *wps = "";
      if (bssInfo) {
        phyMode.reserve(12);
        phyMode = F("802.11");
        String slash;
        if (bssInfo->phy_11b) {
          phyMode += 'b';
          slash = '/';
        }
        if (bssInfo->phy_11g) {
          phyMode += slash + 'g';
          slash = '/';
        }
        if (bssInfo->phy_11n) {
          phyMode += slash + 'n';
        }
        if (bssInfo->wps) {
          wps = PSTR("WPS");
        }
      }
      Serial.printf(PSTR("  %02d: [CH %02d] [%02X:%02X:%02X:%02X:%02X:%02X] %ddBm %c %c %-11s %3S %s\n"), i, channel, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5], rssi, (encryptionType == ENC_TYPE_NONE) ? ' ' : '*', hidden ? 'H' : 'V', phyMode.c_str(), wps, ssid.c_str());
      yield();
    }
  } else {
    Serial.printf(PSTR("WiFi scan error: %d"), WifiNetworks);
  }

  if (countargs != 0) {                         // erster Aufruf ohne Argumente
    Serial.print(F("Auswahl SSID(setssid): "));
    Serial.println(setssid);
    Serial.print(F("Auswahl versteckte SSID(hssid): "));
    Serial.println(hssid);
    Serial.print(F("WLAN Passwort(qpass): "));
    Serial.println(qpass);
    Serial.print(F("1-DHCP ein, 0-DHCP aus(qdhcp): "));
    Serial.println(qdhcp);
    Serial.print(F("IP-Adresse statisch(qip): "));
    Serial.println(qip);
    Serial.print(F("Domain Name Server statisch(qdns): "));
    Serial.println(qdns);
    Serial.print(F("Gateway statisch(qsgw): "));
    Serial.println(qsgw);
    Serial.print(F("Subnetzmaske statisch(qsnm): "));
    Serial.println(qsnm);
    Serial.print(F("welcher Button wurde betaetigt(submit): "));
    Serial.println(submit);
  }
#endif
  // Beginn HTML
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/button.css\">");
  insertMenu_Setup(sResponse);                       // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);                // Überschrift Tabelle Zeile 2 einfügen
  // Zeile 3 einfügen
  sResponse += (F("<tr><td class=\"CXB\" colspan=\"5\">Einstellungen WLAN</td></tr>"));
  // Zeile 4 einfügen
  if (WifiNetworks <= 0 && countargs == 0) {                      // kein Netzwerk gefunden!
    sResponse += F("<tr><td colspan=\"5\" class=\"cred\">"
                   "<br><br>Kein Netzwerk gefunden!<br><br><br></td></tr>"
                   "<tr><td colspan=\"2\"></td>"
                   "<td><button type=\"submit\" name=\"submit\" value=\"rescan\">Scannen</button></td>"
                   "<td colspan=\"2\"></td></tr>"
                   "<tr><td colspan=\"5\"><br><br></td></tr>");
  } else {
    if (submit == "wps") {
      //estart = 0;                                 // Zähler zurück setzen
      //EEPROM_write_byte(EEPROM_ADDR_START, estart);      // write Start Count to EEPROM
      sResponse += F("<tr><td colspan=\"5\" class=\"cred\">"
                     "<br><br>Starte WPS (Wi-Fi Protected Setup)."
                     "<br>Sollte es nicht gelingen, werden die alten Einstellungen weiter verwendet.<br><br></td></tr>"
                     "<script>setTimeout(function(){window.location.href='setuplan.htm'},60000);</script>");
      // Footer anhängen und senden
      insertFooterSend(sResponse);                       // Footer anhängen und senden
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
      Serial.println(F("WPS-Button pressed"));
      Serial.println(F("Wait 1 second for sending website"));
#endif
      delay(1000); // wait for sending website

      // starte WiFi Protected Setup (WPS)
      if (!start_WPS_connect()) {                         // Wifi WPS Connection failed
        String logText = F("Wifi WPS connection failed, restart");
        appendLogFile(logText);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
        Serial.println(logText);
#endif
        delay(1000);
        ESP.restart();
      }
    }
    if (submit == "save") {
      if (qdhcp == "1") {     // DHCP ein
        edhcp = true;
      } else {                // DHCP aus
        edhcp = false;
        byte ipaddress[4];
        if (str2ip((char*)qip.c_str(), ipaddress)) {            // convert String to Array of 4 Bytes
          eip = ipaddress;                                      // statische IP übernehmen
          EEPROM_write_ipaddress(EEPROM_ADDR_IP, eip);          // write IPAddress to EEPROM
        } else {
          errorIpAddr = 1;                                      // Error IP address
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
          Serial.println(F("ERROR static IP-Address"));
#endif
        }
        if (str2ip((char*)qsgw.c_str(), ipaddress)) {           // convert String to Array of 4 Bytes
          esgw = ipaddress;                                     // Standard-Gateway übernehmen
          EEPROM_write_ipaddress(EEPROM_ADDR_GATEWAY, esgw);    // write IPAddress to EEPROM
        } else {
          errorIpAddr = 2;                                      // Error IP address
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
          Serial.println(F("ERROR Standard-Gateway"));
#endif
        }
        if (str2ip((char*)qdns.c_str(), ipaddress)) {           // convert String to Array of 4 Bytes
          edns = ipaddress;                                     // statischen Domain Name Server übernehmen
          EEPROM_write_ipaddress(EEPROM_ADDR_DNS, edns);        // write IPAddress to EEPROM
        } else {
          errorIpAddr = 3;                                      // Error IP address
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
          Serial.println(F("ERROR Domain Name Server"));
#endif
        }
        if (str2ip((char*)qsnm.c_str(), ipaddress)) {           // convert String to Array of 4 Bytes
          esnm = ipaddress;                                     // Subnetmask übernehmen
          EEPROM_write_ipaddress(EEPROM_ADDR_NETMASK, esnm);    // write IPAddress to EEPROM
        } else {
          errorIpAddr = 4;                                      // Error IP address
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
          Serial.println(F("ERROR Subnetmask"));
#endif
        }
      }
      if (errorIpAddr == 0) {                                    // no Error IP address
        EEPROM_write_boolean(EEPROM_ADDR_DHCP, edhcp);          // write EEPROM DHCP on or off
      }
    }
    // neue Tabelle innerhalb Zeile 4
    sResponse += F("<tr><td colspan=\"5\" class=\"c\">"
                   // neue Tabelle
                   "<table class=\"table802\">"
                   // neue Tabelle Zeile 1 Überschrift
                   "<tr>"
                   "<th class=\"tdW6p\">&nbsp;</th>"
                   "<th class=\"l\">Name (SSID)</th>"
                   "<th>MAC-Adresse</th>"
                   "<th>Kanal</th>"
                   "<th>RSSI</th>"
                   "<th>Verschlüsselung</th>"
                   "</tr>");
    // neue Tabelle Netzwerke auflisten ab Zeile 2
    for (int8_t i = 0; i < WifiNetworks; ++i) {
      String wifiSSID = WiFi.SSID(i);
      sResponse += F("<tr>"
                     // Spalte 1
                     "<td><input type=\"radio\" name=\"setssid\" aria-label=\"SSID\" value=\"");
      sResponse += i; // Nr. in Array
      sResponse += F("\"");
      MAC = WiFi.BSSID(i);  // Gets the MAC address of the router
      if (memcmp(MAC, MAC_CON, 6) == 0) { // Compare two blocks of memory
        sResponse += (wifiSSID == essid ? " checked" : "");
      }
      sResponse += F("></td>"
                     // Spalte 2
                     "<td class=\"l\">");
      sResponse += wifiSSID; // Beschriftung
      sResponse += F("</td>"
                     // Spalte 3
                     "<td>");
      sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X", MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);
      sResponse += str;
      sResponse += F("</td>"
                     // Spalte 4
                     "<td class=\"c\">");
      sResponse += WiFi.channel(i);
      sResponse += F("</td>"
                     // Spalte 5
                     "<td class=\"c\">");
      sResponse += WiFi.RSSI(i);
      sResponse += F(" dB</td>"
                     // Spalte 6
                     "<td class=\"c\">");
      switch (WiFi.encryptionType(i)) {
        case 2:
          sResponse += F("WPA (TKIP)");
          break;
        case 4:
          sResponse += F("WPA2 (CCMP)");
          break;
        case 5:
          sResponse += F("WEP");
          break;
        case 7:
          sResponse += F("NONE");
          break;
        case 8:
          sResponse += F("AUTO");
          break;
        default:
          sResponse += F("unknown");
          break;
      }
      sResponse += F("</td></tr>");
    }
    // neue Tabelle letzte Zeile Eingabefeld SSID
    sResponse += F("<tr><td><input name=\"setssid\" aria-label=\"SSID\" type=\"radio\" value=\"255\"></td>"
                   "<td class=\"l\"><input class=\"iW80p\" name=\"hiddenssid\" autocomplete=\"on\" aria-label=\"SSID\" type=\"text\" maxlength=\"32\"></td>"
                   "<td colspan=\"4\">&nbsp;</td></tr>"
                   // neue Tabelle Ende und Ende Zeile 4
                   "</table></td></tr>"
                   // Zeile 5 einfügen
                   "<tr><td><button type=\"submit\" name=\"submit\" value=\"wps\">WPS</button></td>"
                   "<td class=\"r\">Passwort:</td>"
                   "<td colspan=\"2\" class=\"l\"><input class=\"iW70p\" name=\"pw\" autocomplete=\"current-password\" aria-label=\"Passwort\" type=\"password\" maxlength=\"64\"></td>"
                   "<td><button type=\"submit\" name=\"submit\" value=\"connect\">Verbinden</button></td></tr>");
    if (submit == "connect") {
      // ################## new ##################
      String qssid = "";
      if (setssid == 255) {                        // verstecktes Netzwerk
        qssid = hssid;                             // versteckte SSID übernehmen
      } else {
        qssid = WiFi.SSID(setssid);
      }
      if (qssid.length() > 0 && qssid.length() < 32 && qpass.length() > 0 && qpass.length() < 64) {
        // ################## new ##################
        sResponse += F("<tr><td colspan=\"5\" class=\"cred\">"
                       "Versuche Verbindung zum Netzwerk ");
        sResponse += qssid;
        sResponse += F(" herzustellen.<br>Sollte es nicht gelingen, werden die alten Einstellungen weiter verwendet.</td></tr>"
                       "<script>setTimeout(function(){window.location.href='setuplan.htm'},60000);</script>");
        // Footer anhängen und senden
        insertFooterSend(sResponse);                    // Footer anhängen und senden
        delay(1000);

        if (webtype == 1) {                             // Accesspoint
          WiFi.softAPdisconnect(false);                 // Accesspoint nicht abschalten
          WiFi.mode(WIFI_AP_STA);                       // Accesspoint und Station
        } else {
          WiFi.mode(WIFI_STA);                          // Station
        }
        WiFi.disconnect();
        String logtext = F("Wifi try new connect to: ");
        logtext += qssid;
        appendLogFile(logtext);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
        Serial.println("");
        Serial.println(logtext);
#endif
        if (edhcp == false) {                           // DHCP aus
          WiFi.config(eip, esgw, esnm, edns);           // Style ESP8266
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
          Serial.println(F("DHCP off, use static IP"));
#endif
        } else {                                        // DHCP ein
          wifi_station_dhcpc_start();
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
          Serial.println(F("DHCP on"));
#endif
        }

        if (setssid == 255) {                       // verstecktes Netzwerk
          WiFi.begin(qssid.c_str(), qpass.c_str()); // Connect to hidden Network
        } else {
          MAC_CON = WiFi.BSSID(setssid);  // Gets the MAC address of the router
          uint8_t usedChannel = WiFi.channel(setssid);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
          Serial.print(F("use AP MAC "));
          sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X", MAC_CON[0], MAC_CON[1], MAC_CON[2], MAC_CON[3], MAC_CON[4], MAC_CON[5]);
          Serial.print(str);
          Serial.print(F(", "));
          Serial.println(usedChannel);
#endif
          WiFi.begin(qssid.c_str(), qpass.c_str(), usedChannel, MAC_CON); // Connect to new Network
        }
        if (testWifi()) {
          essid = qssid;                                // SSID übernehmen
          epass = qpass;                                // Wifi Passwort übernehmen
          EEPROM_write_string(EEPROM_ADDR_SSID, essid); // write String to EEPROM
          EEPROM_write_string(EEPROM_ADDR_PASS, epass); // write String to EEPROM
          appendLogFile(F("Wifi settings saved"));
          // zusätzliche Zeile einfügen bei Connect
          sResponse += (F("<tr><td colspan=\"5\" class=\"c\">Verbunden mit Netzwerk: "));
          sResponse += essid;
          sResponse += (F("</td></tr>"));
        } else {
          connectWifiBestRssi(); // connect to AP with best RSSI
          // WiFi.begin(essid.c_str(), epass.c_str());     // Connect to saved Network
          if (testWifi()) {
            // zusätzliche Zeile einfügen bei Connect
            sResponse += (F("<tr><td colspan=\"5\" class=\"c\">Verbunden mit Netzwerk: "));
            sResponse += essid;
            sResponse += (F("</td></tr>"));
          }
        }
      } else {
        // zusätzliche Zeile einfügen bei Fehler Name (SSID) oder Passwort zu kurz
        sResponse += (F("<tr><td colspan=\"5\" class=\"cred\">Name (SSID) oder Passwort zu kurz oder zu lang!</td></tr>"));
      }
    }
    // Zeile 6 einfügen
    sResponse += F("<tr><td class=\"CXB\" colspan=\"5\">Netzwerk</td></tr>"
                   // Zeile 7 einfügen
                   "<tr><td class=\"r\"><input type=\"radio\" name=\"dhcp\" aria-label=\"DHCP\" value=\"1\"");
    if (edhcp == true) sResponse += (F(" checked"));  // DHCP ein
    sResponse += F("></td>"
                   "<td colspan = \"2\">IP-Adresse automatisch (DHCP)</td>"
                   "<td colspan=\"2\">");
    if (edhcp == true) {                              // DHCP ein
      ipadr = WiFi.localIP();
      sprintf(str, "%d.%d.%d.%d", ipadr[0], ipadr[1], ipadr[2], ipadr[3]);
      sResponse += str;
    } else {                                          // DHCP aus
      sResponse += (F("&nbsp;"));
    }
    sResponse += F("</td></tr>"
                   // Zeile 8 einfügen
                   "<tr><td class=\"r\"><input type=\"radio\" name=\"dhcp\" aria-label=\"DHCP\" value=\"0\"");
    if (edhcp == false) sResponse += (F(" checked")); // DHCP aus
    sResponse += F("></td>"
                   "<td colspan=\"2\">IP-Adresse statisch</td>"
                   "<td colspan=\"2\">"
                   "<input name=\"ip\" aria-label=\"IP\" type=\"text\" id=\"ip\" maxlength=\"15\" value=\"");
    sprintf(str, "%d.%d.%d.%d", eip[0], eip[1], eip[2], eip[3]);
    sResponse += str;
    sResponse += F("\"></td></tr>"
                   // Zeile 9 einfügen
                   "<tr><td colspan=\"3\" class=\"r\">Domain Name Server:</td>"
                   "<td colspan=\"2\">"
                   "<input name=\"dns\" aria-label=\"DNS\" type=\"text\" id=\"dns\"  maxlength=\"15\" value=\"");
    if (edhcp == false) {                             // DHCP aus
      ipadr = edns;                                   // Domain Name Server übernehmen
    } else {                                          // DHCP ein
      ipadr = WiFi.dnsIP();
    }
    sprintf(str, "%d.%d.%d.%d", ipadr[0], ipadr[1], ipadr[2], ipadr[3]);
    sResponse += str;
    sResponse += F("\"></td></tr>"
                   // Zeile 10 einfügen
                   "<tr><td colspan=\"3\" class=\"r\">Standardgateway:</td>"
                   "<td colspan=\"2\">"
                   "<input name=\"gw\" aria-label=\"Gateway\" type=\"text\" id=\"gw\"  maxlength=\"15\" value=\"");
    if (edhcp == false) {                             // DHCP aus
      ipadr = esgw;                                   // eigenes Standard-Gateway übernehmen
    } else {                                          // DHCP ein
      ipadr = WiFi.gatewayIP();
    }
    sprintf(str, "%d.%d.%d.%d", ipadr[0], ipadr[1], ipadr[2], ipadr[3]);
    sResponse += str;
    sResponse += F("\"></td></tr>"
                   // Zeile 11 einfügen
                   "<tr><td colspan=\"3\" class=\"r\">Subnetzmaske:</td>"
                   "<td>"
                   "<input name=\"sn\" aria-label=\"Subnetz\" type=\"text\" id=\"sn\"  maxlength=\"15\" value=\"");
    if (edhcp == false) {                             // DHCP aus
      ipadr = esnm;                                   // eigene Subnetzkaske übernehmen
    } else {                                          // DHCP ein
      ipadr = WiFi.subnetMask();
    }
    sprintf(str, "%d.%d.%d.%d", ipadr[0], ipadr[1], ipadr[2], ipadr[3]);
    sResponse += str;
    sResponse += F("\"></td><td><button type=\"submit\" name=\"submit\" value=\"save\">Speichern</button></td></tr>");
  }
  // zusätzliche Zeile einfügen bei Fehler IP Adresse
  if (errorIpAddr > 0) {  // Error IP address
    sResponse += F("<tr><td colspan=\"5\" class=\"cred\">");
    if (errorIpAddr == 1) {  // Error IP address
      sResponse += F("IP-Adresse statisch");
    } else if (errorIpAddr == 2) {  // Error Standardgateway
      sResponse += F("Standardgateway");
    } else if (errorIpAddr == 3) {  // Error Domain Name Server
      sResponse += F("Domain Name Server");
    } else if (errorIpAddr == 4) {  // Error Subnetzmaske
      sResponse += F("Subnetzmaske");
    }
    sResponse += F(" ungültig!</td></tr>");
  }
  // Serial.println(F("Delete the last scan result from memory."));
  WiFi.scanDelete(); // Delete the last scan result from memory.
  // Website /setuplan.htm HTTP TX Bytes: 4389, Free Heap: 23.33 kB
  // Website /setuplan.htm HTTP TX Bytes: 4385, Free Heap: 24.08 kB
  // Website /setuplan.htm HTTP TX Bytes: 4812, Free Heap: 22.77 kB
  // Footer anhängen und senden
  insertFooterSend(sResponse);                       // Footer anhängen und senden
}

// ############################## Einstellungen Zeit anzeigen ##############################
void SiteSetupTime()  {
  String sResponse = "";                        // Response HTML
  sResponse.reserve(4096); // ohne: Dauer: 2577 µS, 2805 Byte, Free Heap: 28.44 kB
  int errorNTP = 0;                             // Error NTP settings
  String da = server.arg("da");           // Day
  String mo = server.arg("mo");           // Month
  String ye = server.arg("ye");           // Year
  String ho = server.arg("ho");           // Hour
  String mi = server.arg("mi");           // Minute
  String se = server.arg("se");           // Second
  String ds = server.arg("dst");          // DST on/off
  String NtpNr = server.arg("ntp");       // NTP-Server Array-Number
  String NtpOwn = server.arg("own");      // own NTP-Server
  String submit = server.arg("submit");   // Button submit settime or setntp
  int countargs = server.args();
  if (countargs != 0) {
    if (submit == "settime") {                    // Datum und Uhrzeit stellen
      setTime(ho.toInt(), mi.toInt(), se.toInt(), da.toInt(), mo.toInt(), ye.toInt());
      DS1307_setTime();                          // set date and time to DS1307
      lastSetTime = now();
      appendLogFile(F("Web time set"));
      if (ds == "on") {
        DST = true;
      } else {
        DST = false;
      }
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
      Serial.print(F("Set Date: "));
      Serial.println(DateToString(now()));        // return Date String from Timestamp
      Serial.print(F("Set Time: "));
      Serial.println(TimeToString(now()));        // return Time String from Timestamp
      Serial.print(F("Set DST: "));
      Serial.println(DST);
#endif
      EEPROM_write_boolean(EEPROM_ADDR_DST, DST);      // write boolean at address
    }
    if (submit == "setntp") {                     // save NTP-Settings
      if (NtpOwn != "") {
        ntpServerName[2] = NtpOwn;
      }
      if (NtpNr != "") {
        ntpServerNr = NtpNr.toInt();
        if (ntpServerNr != 3) {             // Synchronisierung ein
          if (setTimeNtp()) {               // NTP set time OK
            EEPROM_write_byte(EEPROM_ADDR_NTPSERVERNUMBER, ntpServerNr);     // write eeprom NTP Server Number
            if (ntpServerNr == 2) {
              EEPROM_write_string(EEPROM_ADDR_NTPSERVERNAME, ntpServerName[2]);   // write String to EEPROM
            }
            appendLogFile(F("NTP settings saved"));
          } else {
            errorNTP = 1;                             // Error NTP Settings
          }
        } else {                            // Synchronisierung aus
          EEPROM_write_byte(EEPROM_ADDR_NTPSERVERNUMBER, 3);     // write eeprom NTP Server Number
          appendLogFile(F("NTP set time off"));
        }
      }
    }
  }
  // Beginn HTML
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/button.css\">");
  insertMenu_Setup(sResponse);                       // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);                // Überschrift Tabelle Zeile 2 einfügen
  // Zeile 3 einfügen
  sResponse += F("<tr><td class=\"CXB\" colspan=\"5\">Datum und Uhrzeit</td></tr>");
  // Tabelle Zeile 4 einfügen
  sResponse += F("<tr><td class=\"r\">Datum:</td>"
                 "<td colspan=\"4\" nowrap><input type=\"number\" aria-label=\"Tag\" name=\"da\" min=\"1\" max=\"31\" size=\"4\" maxlength=\"2\" value=\"");
  sResponse += day();
  sResponse += F("\">."
                 "<input type=\"number\" aria-label=\"Monat\" name=\"mo\" min=\"1\" max=\"12\" size=\"4\" maxlength=\"2\" value=\"");
  sResponse += month();
  sResponse += F("\">."
                 "<input type=\"number\" aria-label=\"Jahr\" name=\"ye\" min=\"1970\" max=\"2038\" size=\"4\" maxlength=\"4\" value=\"");
  sResponse += year();
  sResponse += F("\"></td></tr>");
  // Tabelle Zeile 5 einfügen
  sResponse += F("<tr><td class=\"r\">Uhrzeit:</td>"
                 "<td colspan=\"3\"><input type=\"number\" aria-label=\"Stunde\" name=\"ho\" min=\"0\" max=\"23\" size=\"4\" maxlength=\"2\" value=\"");
  sResponse += hour();
  sResponse += F("\">:"
                 "<input type=\"number\" aria-label=\"Minute\" name=\"mi\" min=\"0\" max=\"59\" size=\"4\" maxlength=\"2\" value=\"");
  sResponse += minute();
  sResponse += F("\">:"
                 "<input type=\"number\" aria-label=\"Sekunde\" name=\"se\" min=\"0\" max=\"59\" size=\"4\" maxlength=\"2\" value=\"");
  sResponse += second();
  sResponse += F("\"></td>"
                 "<td><button type=\"submit\" name=\"submit\" value=\"settime\">Stellen</button></td></tr>");
  // Tabelle Zeile 6 einfügen
  sResponse += F("<tr><td class=\"r\">Sommerzeit:</td>"
                 "<td class=\"c\"><input name=\"dst\" aria-label=\"Sommerzeit\" type=\"checkbox\" id=\"dst\"");
  if (DST == true) {
    sResponse += F(" checked");
  }
  sResponse += F("></td><td colspan=\"3\">&nbsp;</td></tr>");
  // Tabelle Zeile 7 einfügen
  sResponse += F("<tr><td class=\"CXB\" colspan=\"5\">Internetzeit</td></tr>");
  // Tabelle Zeile 8 einfügen
  sResponse += F("<tr><td class=\"r\"><input type=\"radio\" name=\"ntp\" aria-label=\"NTP 0\" value=\"0\"");
  sResponse += (ntpServerNr == 0 ? " checked" : "");
  sResponse += F("></td><td colspan=\"4\">");
  sResponse += ntpServerName[0];
  sResponse += F("</td></tr>");
  // Tabelle Zeile 9 einfügen
  sResponse += F("<tr><td class=\"r\"><input type=\"radio\" name=\"ntp\" aria-label=\"NTP 1\" value=\"1\"");
  sResponse += (ntpServerNr == 1 ? " checked" : "");
  sResponse += F("></td><td colspan=\"4\">");
  sResponse += ntpServerName[1];
  sResponse += F("</td></tr>");
  // Tabelle Zeile 10 einfügen
  sResponse += F("<tr><td class=\"r\"><input type=\"radio\" name=\"ntp\" aria-label=\"NTP 2\" value=\"2\"");
  sResponse += (ntpServerNr == 2 ? " checked" : "");
  sResponse += F("></td>"
                 "<td colspan=\"4\"><input name=\"own\" aria-label=\"NTP\" type=\"text\" id=\"own\"  maxlength=\"64\" value=\"");
  sResponse += ntpServerName[2];
  sResponse += F("\"></td></tr>"
                 // Tabelle Zeile 11 einfügen
                 "<tr><td class=\"r\"><input type=\"radio\" name=\"ntp\" aria-label=\"NTP 3\" value=\"3\"");
  sResponse += (ntpServerNr == 3 ? " checked" : "");
  sResponse += F("></td><td colspan=\"3\">keine Synchronisierung</td>"
                 "<td><button type=\"submit\" name=\"submit\" value=\"setntp\">Speichern</button></td></tr>");
  // Tabelle Zeile 12 einfügen
  if (errorNTP == 1) {                             // Error NTP Settings
    sResponse += F("<tr><td colspan=\"5\" class=\"cred\">Fehler bei Zeitsynchronisation!</td></tr>");
  } else {
    sResponse += F("<tr><td>&nbsp;</td>"
                   "<td colspan=\"4\">Uhrzeit zuletzt gestellt: ");
    if (timeStatus() == 0) {
      sResponse += F("nie");
    } else {
      sResponse += DateToString(lastSetTime);    // return Date String from Timestamp
      sResponse += F(", ");
      sResponse += TimeToString(lastSetTime);    // return Time String from Timestamp
      sResponse += F(" Uhr");
    }
    sResponse += F("</td></tr>");
  }

  // Footer anhängen und senden
  insertFooterSend(sResponse);                       // Footer anhängen und senden
}

// ############################## Systeminfo anzeigen ##############################
void SiteInfo()  {
  String sResponse = "";                       // Response HTML
  sResponse.reserve(4096); // ohne: Dauer: 4956 µS, 2166 Byte, Free Heap: 28.15 kB
  IPAddress ipadr;                             // IP address
  uint8_t* MAC;                                // MAC address
  uint8_t MAC_softAP[6];    //not sure why we have to pass a MAC address to get a MAC address.
  uint8_t MAC_STA[6];
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  sResponse += F("</head><body><form method=\"get\">"
                 "<table class=\"table802\">"
                 "<tr><td class=\"CLB\"><a href=\"index.htm\">"
                 "<img src=\"pic/logo.svg\" height=\"32\" width=\"32\" alt=\"Home\"></a></td>"
                 "<td class=\"CLB\"><a href=\"info.htm\">Informationen</a></td>"
                 "<td class=\"CLB\"><a href=\"log.htm\">Log</a></td>"
                 "<td class=\"CLB\"><a href=\"files.htm\">Dateien</a></td>"
                 "<td class=\"CLB\"><a href=\"htm/doku.htm\">Dokumentation</a></td></tr>");
  insertMenu_Ueberschrift(sResponse);              // Überschrift Tabelle Zeile 2 einfügen
  // Zeile 3 einfügen
  sResponse += F("<tr><td class=\"CXB\" colspan=\"5\">Systeminformationen</td></tr>"
                 // Zeile 4 einfügen
                 "<tr><th colspan=\"2\">Netzwerk</th>"
                 "<td>&nbsp;</td>"
                 "<th colspan=\"2\">Hardware</th></tr>"
                 // Zeile 5 einfügen
                 "<tr><td>Hostname:</td>"
                 "<td>");
  if (webtype == 0) {                           // Station
    sResponse += WiFi.hostname();
  } else {                                      // Access Point
    sResponse += OwnStationHostname;
  }
  sResponse += F("</td>"
                 "<td>&nbsp;</td>"
                 "<td>ESP8266 Chip-ID:</td>"
                 "<td>");
  uint8_t ChipID[3];
  ChipID[0] = (ESP.getChipId() >> 0)  & 0xFF;
  ChipID[1] = (ESP.getChipId() >> 8)  & 0xFF;
  ChipID[2] = (ESP.getChipId() >> 16) & 0xFF;
  char str[20];
  sprintf(str, "%02X%02X%02X", ChipID[2], ChipID[1], ChipID[0]);
  sResponse += str;
  sResponse += F("</td></tr>"
                 // Zeile 6 einfügen
                 "<tr><td>MAC:</td>"
                 "<td>");
  if (webtype == 0) {                           // Station
    MAC = WiFi.macAddress(MAC_STA);             //get MAC address of Station interface
  } else {                                      // Access Point
    MAC  = WiFi.softAPmacAddress(MAC_softAP);   //get MAC address of softAP interface
  }
  sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X", MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);
  sResponse += str;
  sResponse += F("</td>"
                 "<td>&nbsp;</td>"
                 "<td>Flash Chip-ID:</td>"
                 "<td>");
  ChipID[0] = (ESP.getFlashChipId() >> 0)  & 0xFF;
  ChipID[1] = (ESP.getFlashChipId() >> 8)  & 0xFF;
  ChipID[2] = (ESP.getFlashChipId() >> 16) & 0xFF;
  sprintf(str, "%02X%02X%02X", ChipID[2], ChipID[1], ChipID[0]);
  sResponse += str;
  sResponse += F("</td></tr>"
                 // Zeile 7 einfügen
                 "<tr><td>IP:</td>"
                 "<td>");
  if (webtype == 0) {                         // Station
    ipadr = WiFi.localIP();
  } else {                                    // Access Point
    ipadr = WiFi.softAPIP();
  }
  sprintf(str, "%d.%d.%d.%d", ipadr[0], ipadr[1], ipadr[2], ipadr[3]);
  sResponse += str;
  sResponse += F("</td>"
                 "<td>&nbsp;</td>"
                 "<td>Flash Chip Size:</td>"
                 "<td>");
  sResponse += formatBytes(ESP.getFlashChipRealSize());
  sResponse += F("</td></tr>"
                 // Zeile 8 einfügen
                 "<tr><td>Gateway:</td>"
                 "<td>");
  ipadr = WiFi.gatewayIP();
  sprintf(str, "%d.%d.%d.%d", ipadr[0], ipadr[1], ipadr[2], ipadr[3]);
  sResponse += str;
  sResponse += F("</td>"
                 "<td>&nbsp;</td>"
                 "<td>Flash Chip Speed:</td>"
                 "<td>");
  sResponse += formatHertz(ESP.getFlashChipSpeed());
  sResponse += F("</td></tr>"
                 // Zeile 9 einfügen
                 "<tr><td>Netmask:</td>"
                 "<td>");
  ipadr = WiFi.subnetMask();
  sprintf(str, "%d.%d.%d.%d", ipadr[0], ipadr[1], ipadr[2], ipadr[3]);
  sResponse += str;
  sResponse += F("</td>"
                 "<td>&nbsp;</td>");
#ifdef HW_VERSION_1                                 // Hardware Version 1 mit ESP-12E
  sResponse += F("<td>Versorgungspannung:</td>"
                 "<td>");
  sResponse += String(ESP.getVcc() / 1000.0, 2);    // using a float and 2 decimal places
#else                                               // Hardware Version 2 mit NodeMCU
  //Serial.println(analogRead(A0));
  sResponse += F("<td>Batteriepannung:</td>"
                 "<td>");
  sResponse += String(analogRead(A0) / ADC_DIV, 2); // using a float and 2 decimal places
#endif
  sResponse += F(" Volt</td></tr>"
                 // Zeile 10 einfügen
                 "<tr><td>SSID:</td>"
                 "<td>");
  if (webtype == 0) {                           // Station
    sResponse += WiFi.SSID();
  } else {                                      // Access Point
    sResponse += OwnStationHostname;
  }
  sResponse += F("</td>"
                 "<td>&nbsp;</td>"
                 "<th colspan=\"2\">Firmware</th></tr>"
                 // Zeile 11 einfügen
                 "<tr><td>Router MAC:</td>"
                 "<td>");
  if (webtype == 0) {                           // Station
    MAC = WiFi.BSSID();                // Gets the MAC address of the router you are connected to
    sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X", MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);
    sResponse += str;
  } else {                                      // Access Point
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td>"
                 "<td>&nbsp;</td>"
                 "<td>Version:</td>"
                 "<td>");
  sResponse += VERSION;
  sResponse += F(".");
  sResponse += BUILD;
  sResponse += F("</td></tr>"
                 // Zeile 12 einfügen
                 "<tr><td>RSSI:</td>"
                 "<td>");
  sResponse += WiFi.RSSI();
  sResponse += F(" dB</td>"
                 "<td>&nbsp;</td>"
                 "<td>Datum:</td>"
                 "<td>");
  sResponse += compile_date;
  sResponse += F("</td></tr>"
                 // Zeile 13 einfügen
                 "<tr><td>Wifi RX/TX:</td>"
                 "<td>");
  sResponse += formatBytes(ulWifiTxBytes);
  sResponse += F("</td>"
                 "<td>&nbsp;</td>"
                 "<td>Größe:</td>"
                 "<td>");
  sResponse += formatBytes(ESP.getSketchSize());
  sResponse += F("</td></tr>");

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## Dateien anzeigen ##############################
void SiteFiles()  {
  String sResponse = "";                        // Response HTML
  sResponse.reserve(16384); // ohne: Dauer: 3139759 µS, 14300 Byte, Free Heap: 17.91 kB
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  sResponse += (F("</head><body><form method=\"get\">"
                  "<table class=\"table802\">"
                  "<tr><td class=\"CLB\"><a href=\"index.htm\">"
                  "<img src=\"pic/logo.svg\" height=\"32\" width=\"36\" alt=\"Home\"></a></td>"
                  "<td class=\"CLB\"><a href=\"info.htm\">Informationen</a></td>"
                  "<td class=\"CLB\"><a href=\"log.htm\">Log</a></td>"
                  "<td class=\"CLB\"><a href=\"files.htm\">Dateien</a></td>"
                  "<td class=\"CLB\"><a href=\"htm/doku.htm\">Dokumentation</a></td></tr>"));
  insertMenu_Ueberschrift(sResponse);              // Überschrift Tabelle Zeile 2 einfügen
  // Tabelle Zeile 3 einfügen
  sResponse += (F("<tr><td class=\"CXB\" colspan=\"5\">Dateien</td></tr>"
                  // Tabelle Zeile 4 einfügen
                  "<tr><td colspan=\"5\">"
                  // neue Tabelle in Tabelle
                  "<table class=\"font_c tableCenter\">"
                  "<tr>"
                  "<th class=\"c\">Name</th>"
                  "<th class=\"c\">Änderungsdatum</th>"
                  "<th class=\"c\">Größe</th>"
                  "</tr>"));

  for (uint8_t c = 0; c < 6; c++) {
    sResponse += listDirectories(strDir[c]);
  }

  FSInfo fs_info;
  LittleFS.info(fs_info);

  sResponse += F("<tr><td class=\"r\"><strong>verwendet:</strong></td>"
                 "<td>&nbsp;</td><td class=\"r\" nowrap=\"nowrap\"><strong>");
  sResponse += formatBytes(fs_info.usedBytes);
  sResponse += F("</strong></td></tr>");

  sResponse += F("<tr><td class=\"r\"><strong>Gesamtgröße:</strong></td>"
                 "<td>&nbsp;</td><td class=\"r\" nowrap=\"nowrap\"><strong>");
  sResponse += formatBytes(fs_info.totalBytes);
  sResponse += F("</strong></td></tr>");

  size_t freeBytes = fs_info.totalBytes - fs_info.usedBytes;
  sResponse += F("<tr><td class=\"r\"><strong>frei:</strong></td>"
                 "<td>&nbsp;</td><td class=\"r\" nowrap=\"nowrap\"><strong>");
  sResponse += formatBytes(freeBytes);
  sResponse += F("</strong></td></tr>"
                 "</table></td></tr>");

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## Log Tag anzeigen ##############################
void SiteLogTag()  {
  String sResponse = "";                        // Response HTML
  sResponse.reserve(4096); // ohne: Dauer: 51097 µS, 2329 Byte, Free Heap: 28.60 kB
  int rows = 0;                                 // Anzahl Zeilen
  int countargs = server.args();
  String Button = server.arg("Button");
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  SerialPrintLine();            // Trennlinie seriell ausgeben
#endif
  if (countargs == 0) {
    DiagrammTimestamp = day();             // aktuellen Tag übernehmen
  } else {
    if (Button == "1") {                    // Button links
      if (DiagrammTimestamp > 1) {
        DiagrammTimestamp -= 1;
      }
    }
    if (Button == "2") {                    // Button rechts
      if (DiagrammTimestamp < LastDayOfMonth (month(), year())) {
        DiagrammTimestamp += 1;
      }
    }
  }
  // Anzahl Zeilen ermitteln
  String FileName = F("/log/d_");
  FileName += DiagrammTimestamp;
  FileName += F(".log");
  if (LittleFS.exists(FileName)) {        // Returns true if a file with given path exists, false otherwise.
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("LittleFS Reading Data from: "));
    Serial.println(FileName);
#endif
    rows = countLogLines(FileName); // Anzahl Zeilen ermitteln
  } else {
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("LittleFS Logfile not available: "));
    Serial.println(FileName);
#endif
  }
  // Beginn HTML
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/buttonxl.css\">");
  insertMenu_Log(sResponse);                       // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);              // Überschrift Tabelle Zeile 2 einfügen
  // Tabelle Zeile 3 einfügen
  sResponse += F("<tr><td class=\"CXB\">");
  if (DiagrammTimestamp > 1) {
    sResponse += F("<button type=\"submit\" name=\"Button\" value=\"1\">&#9664;</button>");
    //sResponse += "<button type=\"submit\" name=\"Button\" value=\"1\">&#9668;</button>";
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td><td class=\"CXB\" colspan=\"3\">");
  if (rows > 0) {                                             // Link einfügen
    sResponse += F("<a href=\"log/d_");
    sResponse += DiagrammTimestamp;                           // Tag
    sResponse += F(".log\">");
  }
  sResponse += F("Log Tag ");
  sResponse += DiagrammTimestamp;                           // Tag
  if (rows > 0) {                                             // Link einfügen
    sResponse += F("</a>");
  }
  sResponse += F("</td><td class=\"CXB\">");
  if (DiagrammTimestamp < LastDayOfMonth(month(), year())) {
    sResponse += F("<button type=\"submit\" name=\"Button\" value=\"2\">&#9654;</button>");
    //sResponse += "<button type=\"submit\" name=\"Button\" value=\"2\">&#9658;</button>";
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td></tr>"
                 // Tabelle Zeile 4 einfügen
                 "<tr><td colspan=\"5\">"
                 // Neue Tabelle
                 "<table class=\"font_c tableCenter\"><tr><td>");
  if (rows > 0) {
    File LogFile = LittleFS.open(FileName, "r");         // Open text file for reading.
    int j = 0;
    while (LogFile.available()) {
      //Lets read line by line from the file
      String line = LogFile.readStringUntil('\n');
      sResponse += line;
      j += 1;
      if (j < rows) {
        sResponse += F("<br />");
      }
    }
    LogFile.close();
  } else {
    sResponse += F("<br><br>Noch keine Datei vorhanden!<br><br><br>");
  }
  sResponse += F("</td></tr></table></td></tr>");

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## Log Monat anzeigen ##############################
void SiteLogMonat()  {
  String sResponse = "";                        // Response HTML
  sResponse.reserve(4096);
  int rows = 0;                                 // Anzahl Zeilen
  String Monat[13] = {"", "Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"};
  int countargs = server.args();
  String Button = server.arg("Button");
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  SerialPrintLine();            // Trennlinie seriell ausgeben
#endif
  if (countargs == 0) {
    DiagrammTimestamp = month();             // aktuellen Monat übernehmen
  } else {
    if (Button == "1") {                    // Button links
      if (DiagrammTimestamp > 1) {
        DiagrammTimestamp -= 1;
      }
    }
    if (Button == "2") {                    // Button rechts
      if (DiagrammTimestamp < 12) {
        DiagrammTimestamp += 1;
      }
    }
  }
  // Anzahl Zeilen ermitteln
  String FileName = F("/log/m_");
  FileName += DiagrammTimestamp;
  FileName += F(".log");
  if (LittleFS.exists(FileName)) {        // Returns true if a file with given path exists, false otherwise.
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("LittleFS Reading Data from: "));
    Serial.println(FileName);
#endif
    rows = countLogLines(FileName); // Anzahl Zeilen ermitteln
  } else {
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("LittleFS Logfile not available: "));
    Serial.println(FileName);
#endif
  }
  // Beginn HTML
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/buttonxl.css\">");
  insertMenu_Log(sResponse);                       // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);              // Überschrift Tabelle Zeile 2 einfügen
  // Tabelle Zeile 3 einfügen
  sResponse += F("<tr><td class=\"CXB\">");
  if (DiagrammTimestamp > 1) {
    sResponse += F("<button type=\"submit\" name=\"Button\" value=\"1\">&#9664;</button>");
    //sResponse += "<button type=\"submit\" name=\"Button\" value=\"1\">&#9668;</button>";
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td><td class=\"CXB\" colspan=\"3\">");
  if (rows > 0) {                                             // Link einfügen
    sResponse += F("<a href=\"log/m_");
    sResponse += DiagrammTimestamp;                           // Monat
    sResponse += F(".log\">");
  }
  sResponse += F("Log Monat ");
  sResponse += Monat[DiagrammTimestamp];                    // Monatsname
  if (rows > 0) {                                             // Link einfügen
    sResponse += F("</a>");
  }
  sResponse += F("</td><td class=\"CXB\">");
  if (DiagrammTimestamp < 12) {
    sResponse += F("<button type=\"submit\" name=\"Button\" value=\"2\">&#9654;</button>");
    //sResponse += "<button type=\"submit\" name=\"Button\" value=\"2\">&#9658;</button>";
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td></tr>"
                 // Tabelle Zeile 4 einfügen
                 "<tr><td colspan = \"5\">"
                 // Neue Tabelle
                 "<table class=\"font_c tableCenter\"><tr><td>");
  if (rows > 0) {
    File LogFile = LittleFS.open(FileName, "r");         // Open text file for reading.
    int j = 0;
    while (LogFile.available()) {
      //Lets read line by line from the file
      String line = LogFile.readStringUntil('\n');
      sResponse += line;
      j += 1;
      if (j < rows) {
        sResponse += F("<br />");
      }
    }
    LogFile.close();
  } else {
    sResponse += F("<br><br>Noch keine Datei vorhanden!<br><br><br>");
  }
  sResponse += F("</td></tr></table></td></tr>");

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## Log Jahr anzeigen ##############################
void SiteLogJahr()  {
  String sResponse = "";                        // Response HTML
  sResponse.reserve(4096);
  int rows = 0;                                 // Anzahl Zeilen
  int countargs = server.args();
  String Button = server.arg("Button");
  // erste vorhandene Logdatei Jahr ermitteln
  int FirstYear = year() - 1;
  String FileName = F("/log/y_");
  FileName += FirstYear;
  FileName += F(".log");
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  SerialPrintLine();            // Trennlinie seriell ausgeben
  Serial.print(F("Search first year: "));
  Serial.println(FirstYear);
#endif
  while (LittleFS.exists(FileName)) {        // Returns true if a file with given path exists, false otherwise.
    FirstYear -= 1;
    FileName = F("/log/y_");
    FileName += (FirstYear);
    FileName += F(".log");
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("Search first year: "));
    Serial.println(FirstYear);
#endif
  }
  FirstYear += 1;
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.print(F("First year: "));
  Serial.println(FirstYear);
#endif
  if (countargs == 0) {
    DiagrammTimestamp = year();             // aktuelles Jahr übernehmen
  } else {
    if (Button == "1") {                    // Button links
      if (DiagrammTimestamp > FirstYear) {
        DiagrammTimestamp -= 1;
      }
    }
    if (Button == "2") {                    // Button rechts
      if (DiagrammTimestamp < year()) {
        DiagrammTimestamp += 1;
      }
    }
  }
  // Anzahl Zeilen ermitteln
  FileName = F("/log/y_");
  FileName += DiagrammTimestamp;
  FileName += F(".log");
  if (LittleFS.exists(FileName)) {        // Returns true if a file with given path exists, false otherwise.
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("LittleFS Reading Data from: "));
    Serial.println(FileName);
#endif
    rows = countLogLines(FileName); // Anzahl Zeilen ermitteln
  } else {
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("LittleFS Logfile not available: "));
    Serial.println(FileName);
#endif
  }
  // Beginn HTML
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/buttonxl.css\">");
  insertMenu_Log(sResponse);                       // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);              // Überschrift Tabelle Zeile 2 einfügen
  // Tabelle Zeile 3 einfügen
  sResponse += F("<tr><td class=\"CXB\">");
  if (DiagrammTimestamp > FirstYear) {
    sResponse += F("<button type=\"submit\" name=\"Button\" value=\"1\">&#9664;</button>");
    //sResponse += "<button type=\"submit\" name=\"Button\" value=\"1\">&#9668;</button>";
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td><td class=\"CXB\" colspan=\"3\">");
  if (rows > 0) {                                             // Link einfügen
    sResponse += F("<a href=\"log/y_");
    sResponse += DiagrammTimestamp;                           // Jahr
    sResponse += F(".log\">");
  }
  sResponse += F("Log Jahr ");
  sResponse += DiagrammTimestamp;                           // Jahr
  if (rows > 0) {                                             // Link einfügen
    sResponse += F("</a>");
  }
  sResponse += F("</td><td class=\"CXB\">");
  if (DiagrammTimestamp < year()) {
    sResponse += F("<button type=\"submit\" name=\"Button\" value=\"2\">&#9654;</button>");
    //sResponse += "<button type=\"submit\" name=\"Button\" value=\"2\">&#9658;</button>";
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td></tr>"
                 // Tabelle Zeile 4 einfügen
                 "<tr><td colspan=\"5\">"
                 // Neue Tabelle
                 "<table class=\"font_c tableCenter\"><tr><td>");
  if (rows > 0) {
    File LogFile = LittleFS.open(FileName, "r"); // Open text file for reading.
    int j = 0;
    while (LogFile.available()) {
      //Lets read line by line from the file
      String line = LogFile.readStringUntil('\n');
      sResponse += line;
      j += 1;
      if (j < rows) {
        sResponse += F("<br />");
      }
    }
    LogFile.close();
  } else {
    sResponse += F("<br><br>Noch keine Datei vorhanden!<br><br><br>");
  }
  sResponse += F("</td></tr></table></td></tr>");

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## Log System anzeigen ##############################
void SiteLogSystem()  {
  String sResponse = "";                        // Response HTML
  sResponse.reserve(12288); // ohne: Dauer: 102857 µS, 5387 Byte, Free Heap: 25.38 kB
  String FileName = "/log/system.log";
  int rows = countLogLines(FileName); // Anzahl Zeilen ermitteln
  // Beginn HTML
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  insertMenu_Log(sResponse);                       // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);               // Überschrift Tabelle Zeile 2 einfügen
  // Tabelle Zeile 3 einfuegen
  sResponse += F("<tr><td class=\"CXB\" colspan=\"5\">Systemlog</td></tr>");
  // Tabelle Zeile 4 einfügen
  sResponse += F("<tr><td colspan=\"5\">"
                 // Neue Tabelle
                 "<table class=\"font_c tableCenter\"><tr><td>");
  File logFile = LittleFS.open(FileName, "r");  // Open text file for reading.
  int j = 0;
  while (logFile.available()) {
    //Lets read line by line from the file
    String Line = logFile.readStringUntil('\n');
    String Message = Line.substring(20, 24);
    if (Message == "GZ16") {
      sResponse += F("<span class=\"red\">");
      sResponse += Line;
      sResponse += F("</span>");
    } else {
      sResponse += Line;
    }
    j += 1;
    if (j < rows) {
      sResponse += F("<br>");
    }
  }
  logFile.close();
  sResponse += F("</td></tr></table></td></tr>");

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## Diagramm Tag ##############################
void SiteDiagramm_d() {
  String Wochentag[8] = {"", "Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Sonnabend"};
  String sResponse = "";                                     // Response HTML
  sResponse.reserve(8192); // ohne: Dauer: 46630 µS, 4172 Byte, Free Heap: 27.42 kB
  int countargs = server.args();
  String Button = server.arg("Button");
  long firstTimestamp = now() - day() * 86400; // letzter Tag des Vormonates
  while (day(firstTimestamp) > day()) {
    firstTimestamp -= 86400;                                       // - 1 Tag
  }
  firstTimestamp += 86400;                                         // + 1 Tag
  if (countargs == 0) {
    DiagrammTimestamp = now();                  // aktuelle Uhrzeit übernehmen
  } else {
    if (Button == "1") {                                       // Button links
      if (DiagrammTimestamp > firstTimestamp) {
        DiagrammTimestamp -= 86400;                                 // - 1 Tag
      }
    }
    if (Button == "2") {                                      // Button rechts
      if (day(DiagrammTimestamp) < day() || month(DiagrammTimestamp) < month() || year(DiagrammTimestamp) < year()) {
        DiagrammTimestamp += 86400;                                 // + 1 Tag
      }
    }
  }

  // Read Data from LittleFS
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  SerialPrintLine();            // Trennlinie seriell ausgeben
#endif
  String Line;
  int hoint = 0;                            // Integer Stunde
  int yeint = 0;                            // Integer Year
  int Space2;
  int Space3;
  int s0_count_int = 0;
  int s0_count_avg_int = 0;                 // S0-Counter Durchschnitt
  int s0_count_ges_int = 0;                 // S0-Counter gesamt
  unsigned int s0_count_max_int = 0;        // S0-Counter maximum
  float s0_count_max_fl = 0;                // S0-Counter maximum
  unsigned int Faktor = 1;
  unsigned int s0_count_int_arr[24] = {0};  // S0-Counter hour
  if (day(DiagrammTimestamp) == day()) {
    s0_count_int_arr[hour()] = s0_count_hour;   // S0-Counter Stunde
    s0_count_max_int = s0_count_hour;           // S0-Counter Maximum
    s0_count_ges_int = s0_count_hour;           // S0-Counter gesamt
  }
  String FileName = F("/log/d_");
  FileName += (day(DiagrammTimestamp));
  FileName += F(".log");
  if (LittleFS.exists(FileName)) {                  // Returns true if a file with given path exists, false otherwise.
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("LittleFS Reading Data from: "));
    Serial.println(FileName);
#endif
    File LogFile = LittleFS.open(FileName, "r"); // Open text file for reading.
    while (LogFile.available()) {
      Line = LogFile.readStringUntil('\n');    //Lets read line by line from the file
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
      Serial.println(Line);
#endif
      yeint = Line.substring(6, 10).toInt();               // Year
      hoint = Line.substring(11, 13).toInt();;             // Stunde
      Space2 = Line.indexOf(' ', 19);             // nach Uhrzeit folgt Count absolut
      Space3 = Line.indexOf(' ', Space2 + 1 );
      s0_count_int = Line.substring(Space3 + 1).toInt();  // liest String bis Zeilenende
      if (yeint == year(DiagrammTimestamp)) {
        s0_count_int_arr[hoint] = s0_count_int_arr[hoint] + s0_count_int;
        s0_count_ges_int = s0_count_ges_int + s0_count_int;   // S0-Counter gesamt
      }
      if (s0_count_int_arr[hoint] > s0_count_max_int) {
        s0_count_max_int = s0_count_int_arr[hoint];           // S0-Counter hour maximum
      }
    }
    LogFile.close();
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("Gesamt: "));
    Serial.println(s0_count_ges_int);     // S0-Counter gesamt
#endif
  } else {
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("LittleFS Logile not exist: "));
    Serial.println(FileName);
#endif
  }
  s0_count_max_fl = s0_count_max_int / 100.0;
  while (s0_count_max_int > 100) {
    s0_count_max_int = s0_count_max_int / 10;
    Faktor = Faktor * 10;
  }
  if (Faktor >= 10) {
    if (s0_count_max_int < 20) {
      s0_count_max_int = s0_count_max_int * 5;
      Faktor = Faktor / 5;
    }
    if (s0_count_max_int < 50) {
      s0_count_max_int = s0_count_max_int * 2;
      Faktor = Faktor / 2;
    }
  }

  // Beginn HTML
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/buttonxl.css\">");
  insertMenu_Diagramme(sResponse);                  // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);               // Überschrift Tabelle Zeile 2 einfügen

  sResponse += F("<tr><td class = \"CXB\">");
  if (DiagrammTimestamp > firstTimestamp) {
    sResponse += F("<button type=\"submit\" name=\"Button\" value=\"1\">&#9664;</button>");
    //sResponse += "<button type=\"submit\" name=\"Button\" value=\"1\">&#9668;</button>";
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td><td class=\"CXB\" colspan=\"3\">Verbrauch ");
  sResponse += Wochentag[weekday(DiagrammTimestamp)];
  sResponse += F(", ");
  sResponse += (DateToString(DiagrammTimestamp));
  sResponse += F("</td>"
                 "<td class=\"CXB\">");
  if (day(DiagrammTimestamp) < day() || month(DiagrammTimestamp) < month() || year(DiagrammTimestamp) < year()) {
    sResponse += F("<button type=\"submit\" name=\"Button\" value=\"2\">&#9654;</button>");
    //sResponse += "<button type=\"submit\" name=\"Button\" value=\"2\">&#9658;</button>";
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td></tr>"
                 // Tabelle Zeile 4 einfügen
                 "<tr><td class=\"c\" colspan=\"5\">"
                 // Beginn Diagramm
                 "<svg width=\"735\" height=\"350\">"
                 // Diagramm Hintergrund hellgrau und Rahmen schwarz Breite 1
                 "<rect x=\"50\" y=\"25\" width=\"672\" height=\"300\" fill=\"#f5f5f5\" stroke-width=\"1\" stroke=\"#000\"></rect>"
                 // Gitterlinien Farbe grau Breite 1
                 "<g stroke=\"#ccc\" stroke-width=\"1\">");
  // Gitterlinien vertikal
  for (int i = 1; i < 24; ++ i) {             // 24 h
    sResponse += F("<path d=\"M ");
    sResponse += i * 28 + 50;                 // x-position x-lines
    sResponse += F(" 25 v 305\"></path>");    // vertikale Linie ab y25 Länge 305
  }
  // Gitterlinien horizontal
  for (int i = 1; i < 10; i += 1) {
    sResponse += F("<path d=\"M 45 ");          // x-position y-lines
    sResponse += 325 - i * 30;                  // y-position y-lines (300 + 25)
    sResponse += F(" h 677\"></path>");         // vertikale Linie Länge 677
  }
  sResponse += F("</g>"                       // Ende Group
                 // Beschriftung Y-Achse
                 "<g text-anchor=\"end\">");
  for (int i = 0; i < 11; i += 1) {
    float yscala = 0.1 * Faktor * i;          // Beschriftung Y-Achse
    String ytext;
    if (Faktor >= 10) {
      ytext =  String(yscala, 0);             // using a float and 0 decimal places
    } else {
      ytext =  String(yscala, 1);             // using a float and 1 decimal places
    }
    sResponse += F("<text x=\"43\" y=\"");    // x-position y-text
    sResponse += 330 - i * 30;                // y-position y-text (300 + 25 + 5)
    sResponse += F("\">");
    sResponse += ytext;                       // text y-axis
    sResponse += F("</text>");
  }
  sResponse += F("</g>"                       // Ende Group
                 // Beschriftung X-Achse
                 "<g text-anchor=\"middle\">");
  for (int i = 0; i < 24; ++ i) {               // 24h
    sResponse += F("<text x=\"");
    sResponse += i * 28 + 64;                   // x-position x-text (50 + 14)
    sResponse += F("\" y=\"345\">");
    sResponse += i;                             // text x-axis
    sResponse += F("</text>");
  }
  sResponse += F("</g>"                       // Ende Group
                 // Säulen zeichnen
                 "<g fill=\"#ff0\" stroke-width=\"1\" stroke=\"#000\">");
  for (int i = 0; i < 24; ++ i) {                               // 24h
    int sh = s0_count_int_arr[i] / Faktor * 3;                  // Höhe Säule 3 Pixel pro Digit
    s0_count_avg_int = s0_count_avg_int + s0_count_int_arr[i];  // Durchschnitt addieren
    if (sh > 0) {
      sResponse += F("<rect x=\"");
      sResponse += i * 28 + 55;                     // x-position start Säule (50 + 5)
      sResponse += F("\" y=\"");
      sResponse += 325 - sh;                        // y-position start Balken (300 + 25)
      sResponse += F("\" width=\"18\" height=\"");
      sResponse += sh;                              // Säulenhöhe
      sResponse += F("\"></rect>");
    }
  }
  sResponse += F("</g>"                           // Ende Group
                 // Beschriftung oben
                 "<text x=\"80\" y=\"20\">Durchschnitt: ");
  float fl = s0_count_avg_int / 2400.0;              // Durchschnitt (24h /100)
  sResponse += fl;                                   // Durchschnitt
  sResponse += F(" m³/h</text>)"
                 "<text x=\"405\" y=\"20\" text-anchor=\"middle\">Maximum: ");  // Max
  sResponse += s0_count_max_fl;                      // Maximum
  sResponse += F(" m³/h</text>"
                 "<text x=\"690\" y=\"20\" text-anchor=\"end\">Gesamt: ");      // Gesamt
  fl = s0_count_ges_int / 100.0;                    // Gesamtverbrauch
  sResponse += fl;                                  // Gesamt
  sResponse += F(" m³</text>"
                 // SVG Ende
                 "</svg></td></tr>");                                           // end svg

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## Diagramm Monat ##############################
void SiteDiagramm_m() {
  String sResponse = "";                        // Response HTML
  sResponse.reserve(8192); // ohne: Dauer: 69738 µS, 6162 Byte, Free Heap: 25.05 kB
  String Monat[13] = {"", "Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"};
  int countargs = server.args();
  String Button = server.arg("Button");

  long firstTimestamp = now() - day() * 86400;  // letzter Tag des Vormonates
  while (month(firstTimestamp) > month() || year(firstTimestamp) == year()) {
    firstTimestamp -= 86400 * LastDayOfMonth(month(firstTimestamp), year(firstTimestamp));  // - 1 Monat
  }
  firstTimestamp += 86400;                    // erster Tag des nächsten Monats
  firstTimestamp += 86400 * LastDayOfMonth(month(firstTimestamp), year(firstTimestamp));    // + 1 Monat

  if (countargs == 0) {
    DiagrammTimestamp = now();                                    // aktuelle Uhrzeit übernehmen
  } else {
    if (Button == "1") {                                          // Button links
      if (DiagrammTimestamp > firstTimestamp) {
        if (day(DiagrammTimestamp) > 15) {
          DiagrammTimestamp -= 2678400;        // - 1 Monat (31 Tage)
        } else {
          DiagrammTimestamp -= 2419200;        // - 1 Monat (28 Tage)
        }
      }
    }
    if (Button == "2") {                  // Button rechts
      if (month(DiagrammTimestamp) < month() || year(DiagrammTimestamp) < year()) {
        if (day(DiagrammTimestamp) < 15) {
          DiagrammTimestamp += 2678400;        // + 1 Monat (31 Tage)
        } else {
          DiagrammTimestamp += 2419200;        // + 1 Monat (28 Tage)
        }
      }
    }
  }
  // Wochenende ermitteln
  long t = DiagrammTimestamp;
  while (day(t) > 1) {        // Monatsbeginn
    t -= 86400;               // - 1 Tag
  }
  long Monatserster = t;
  while (weekday(t) != 7) {   // Saturday
    t += 86400;               // + 1 Tag
  }
  int Saturday = day(t);
  t = Monatserster;
  while (weekday(t) != 1) {   // Sunday
    t += 86400;               // + 1 Tag
  }
  int Sunday = day(t);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.print(F("1. Saturday in month: "));
  Serial.println(Saturday);
  Serial.print(F("1. Sunday im month:   "));
  Serial.println(Sunday);
#endif

  // Read Data from LittleFS
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  SerialPrintLine();            // Trennlinie seriell ausgeben
#endif
  String Line;
  int daint = 0;                            // Integer Tag
  int yeint = 0;                            // Integer Year
  int Space2;
  int Space3;
  int s0_count_int = 0;
  int s0_count_avg_int = 0;                 // S0-Counter Durchschnitt
  unsigned int s0_count_max_int = 0;        // S0-Counter maximum
  int s0_count_ges_int = 0;                 // S0-Counter gesamt
  float s0_count_max_fl = 0;                // S0-Counter maximum
  int Faktor = 1;
  int AnzahlTage = LastDayOfMonth(month(DiagrammTimestamp), year(DiagrammTimestamp));
  unsigned int s0_count_int_arr[32] = {0}; // S0-Counter Tag
  if (month(DiagrammTimestamp) == month()) {
    s0_count_int_arr[day()] = s0_count_day;     // S0-Counter Tag in Array übernehmen
    s0_count_max_int = s0_count_day;            // S0-Counter maximum
    s0_count_ges_int = s0_count_day;            // S0-Counter gesamt
  }
  String FileName = F("/log/m_");
  FileName += (month(DiagrammTimestamp));
  FileName += F(".log");
  if (LittleFS.exists(FileName)) {                  // Returns true if a file with given path exists, false otherwise.
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("LittleFS Reading Data from: "));
    Serial.println(FileName);
#endif
    File LogFile = LittleFS.open(FileName, "r");    // Open text file for reading.
    while (LogFile.available()) {
      Line = LogFile.readStringUntil('\n');       //Lets read line by line from the file
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
      Serial.println(Line);
#endif
      yeint = Line.substring(6, 10).toInt();               // Year
      daint = Line.substring(0, 2).toInt();                // Tag
      Space2 = Line.indexOf(' ', 19);             // nach Uhrzeit folgt Count absolut
      Space3 = Line.indexOf(' ', Space2 + 1 );
      s0_count_int = Line.substring(Space3 + 1).toInt();  // liest String bis Zeilenende
      if (yeint == year(DiagrammTimestamp)) {
        s0_count_int_arr[daint] = s0_count_int_arr[daint] + s0_count_int;
        s0_count_ges_int = s0_count_ges_int + s0_count_int;   // S0-Counter gesamt
      }
      if (s0_count_int_arr[daint] > s0_count_max_int) {
        s0_count_max_int = s0_count_int_arr[daint];        // S0-Counter hour maximum
      }
    }
    LogFile.close();
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("Gesamt: "));
    Serial.println(s0_count_ges_int);     // S0-Counter gesamt
#endif
  } else {
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("LittleFS Logile not exist: "));
    Serial.println(FileName);
#endif
  }
  s0_count_max_fl = s0_count_max_int / 100.0;
  while (s0_count_max_int > 100) {
    s0_count_max_int = s0_count_max_int / 10;
    Faktor = Faktor * 10;
  }
  if (Faktor >= 10) {
    if (s0_count_max_int < 20) {
      s0_count_max_int = s0_count_max_int * 5;
      Faktor = Faktor / 5;
    }
    if (s0_count_max_int < 50) {
      s0_count_max_int = s0_count_max_int * 2;
      Faktor = Faktor / 2;
    }
  }

  // Beginn HTML
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/buttonxl.css\">");
  insertMenu_Diagramme(sResponse);                  // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);               // Überschrift Tabelle Zeile 2 einfügen
  sResponse += F("<tr><td class=\"CXB\">");
  if (DiagrammTimestamp > firstTimestamp) {
    sResponse += F("<button type=\"submit\" name=\"Button\" value=\"1\">&#9664;</button>");
    //sResponse += "<button type=\"submit\" name=\"Button\" value=\"1\">&#9668;</button>";
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td><td class=\"CXB\" colspan=\"3\">Verbrauch ");
  sResponse += (Monat[month(DiagrammTimestamp)]);
  sResponse += F((" "));
  sResponse += (year(DiagrammTimestamp));
  sResponse += F("</td><td class=\"CXB\">");
  if (month(DiagrammTimestamp) < month() || year(DiagrammTimestamp) < year()) {
    sResponse += F("<button type=\"submit\" name=\"Button\" value=\"2\">&#9654;</button>");
    //sResponse += "<button type=\"submit\" name=\"Button\" value=\"2\">&#9658;</button>";
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td></tr>"
                 "<tr><td class=\"c\" colspan=\"5\">");
  // Beginn Diagramm
  // AnzahlTage 28,  29,  30,  31
  int db[4] = {672, 667, 660, 682};                   // Diagrammbreite
  int xa[4] = {24, 23, 22, 22};                       // Abstand X-Raster
  int sb[4] = {14, 13, 12, 12};                       // Säulenbreite
  int arindex = AnzahlTage - 28;
  // Beginn SVG Breite maximal
  sResponse += F("<svg width=\"735\" height=\"350\">"  // Beginn SVG Breite maximal
                 // Diagramm Hintergrund hellgrau und Rahmen schwarz Breite 1
                 "<rect x=\"50\" y=\"25\" width=\"");  // x-position y-position
  sResponse += db[arindex];                           // Diagrammbreite
  sResponse += F("\" height=\"300\" fill=\"#f5f5f5\" stroke-width=\"1\" stroke=\"#000\"></rect>"
                 // Gitterlinien Farbe grau Breite 1
                 "<g stroke=\"#ccc\" stroke-width=\"1\">");
  // Gitterlinien vertikal
  for (int i = 1; i < AnzahlTage; ++ i) {     // Anzahl Tage
    sResponse += F("<path d=\"M ");
    sResponse += i * xa[arindex] + 50;        // x-position x-lines
    sResponse += F(" 25 v 305\"></path>");    // vertikale Linie ab y25 Länge 305
  }
  // Gitterlinien horizontal
  for (int i = 1; i < 10; i += 1) {
    sResponse += F("<path d=\"M 45 ");        // x-position y-lines
    sResponse += 325 - i * 30;                // y-position y-lines (300 + 25)
    sResponse += F(" h ");                    // horizontale Linie
    sResponse += db[arindex] + 5;             // Länge = Diagrammbreite + 5
    sResponse += F("\"></path>");
  }
  sResponse += F("</g>"                     // Ende Group
                 // Beschriftung Y-Achse
                 "<g text-anchor=\"end\">");
  for (int i = 0; i < 11; i += 1) {
    float yscala = 0.1 * Faktor * i;          // Beschriftung Y-Achse
    String ytext;
    if (Faktor >= 10) {
      ytext =  String(yscala, 0);             // using a float and 0 decimal places
    } else {
      ytext =  String(yscala, 1);             // using a float and 1 decimal places
    }
    sResponse += F("<text x=\"43\" y=\"");
    sResponse += 330 - i * 30;                // y-position y-text (300 + 25 + 5)
    sResponse += F("\">");
    sResponse += ytext;                       // text y-axis
    sResponse += F("</text>");
  }
  sResponse += F("</g>"                     // Ende Group
                 // Beschriftung X-Achse
                 "<g text-anchor=\"middle\">");
  for (int i = 1; i <= AnzahlTage; ++ i) {     // Anzahl Tage
    sResponse += F("<text x=\"");
    sResponse += i * xa[arindex] + 39;         // x-position x-text (50 - 11)
    if (i == Saturday) {                       // Sonnabend
      sResponse += F("\" y=\"345\" fill=\"red\">");
      Saturday += 7;
    } else if (i == Sunday) {                   // Sonntag
      sResponse += F("\" y=\"345\" fill=\"red\">");
      Sunday += 7;
    } else {                                    // Wochentag
      sResponse += F("\" y=\"345\">");
    }
    sResponse += i;                             // text x-axis
    sResponse += F("</text>");
  }
  sResponse += F("</g>"                       // Ende Group
                 // Säulen zeichnen
                 "<g fill=\"#ff0\" stroke-width=\"1\" stroke=\"#000\">");
  for (int i = 1; i <= AnzahlTage; ++ i) {                      // Anzahl Tage
    int sh = s0_count_int_arr[i] / Faktor * 3;                  // Höhe Säule 3 Pixel pro Digit
    s0_count_avg_int = s0_count_avg_int + s0_count_int_arr[i];  // Durchschnitt addieren
    if (sh > 0) {
      sResponse += F("<rect x=\"");
      sResponse += i * xa[arindex] + 45 - sb[arindex];  // x-position start Säule (50 - 5)
      sResponse += F("\" y=\"");
      sResponse += 325 - sh;                            // y-position start Balken (300 + 25)
      sResponse += F("\" width=\"");
      sResponse += sb[arindex];                         // Säulenbreite
      sResponse += F("\" height=\"");
      sResponse += sh;                                  // Säulenhöhe
      sResponse += F("\"></rect>");
    }
  }
  sResponse += F("</g>"                     // Ende Group
                 // Beschriftung oben
                 "<text x=\"80\" y=\"20\">Durchschnitt: ");
  float fl = s0_count_avg_int / AnzahlTage / 100.0;  // Durchschnitt (AnzahlTage / 100)
  sResponse += fl;                                   // Durchschnitt
  sResponse += F(" m³/d</text>)"
                 "<text x=\"405\" y=\"20\" text-anchor=\"middle\">Maximum: ");  // Max
  sResponse += s0_count_max_fl;                      // Maximum
  sResponse += F(" m³/d</text>"
                 "<text x=\"690\" y=\"20\" text-anchor=\"end\">Gesamt: ");      // Gesamt
  fl = s0_count_ges_int / 100.0;                    // Gesamtverbrauch
  sResponse += fl;                                  // Gesamt
  sResponse += F(" m³</text>"
                 // SVG Ende
                 "</svg></td></tr>");                                           // end svg

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## Diagramm Jahr ##############################
void SiteDiagramm_j() {
  String sResponse = "";                        // Response HTML
  sResponse.reserve(4096); // ohne: Dauer: 158870 µS, 3890 Byte, Free Heap: 26.46 kB
  int countargs = server.args();
  String Button = server.arg("Button");
  // erste vorhandene Logdatei Jahr ermitteln
  int FirstYear = year() - 1;
  String FileName = F("/log/y_");
  FileName += (FirstYear);
  FileName += F(".log");
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  SerialPrintLine();            // Trennlinie seriell ausgeben
  Serial.print(F("Search first year: "));
  Serial.println(FirstYear);
#endif
  while (LittleFS.exists(FileName)) {        // Returns true if a file with given path exists, false otherwise.
    FirstYear -= 1;
    FileName = F("/log/y_");
    FileName += (FirstYear);
    FileName += F(".log");
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("Search first year: "));
    Serial.println(FirstYear);
#endif
  }
  FirstYear += 1;
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.print(F("First year: "));
  Serial.println(FirstYear);
#endif
  if (countargs == 0) {
    DiagrammTimestamp = now();                                // aktuelle Uhrzeit übernehmen
  } else {
    if (Button == "1") {                                      // Button links
      if (year(DiagrammTimestamp) > FirstYear) {
        if (month(DiagrammTimestamp) > 6) {
          DiagrammTimestamp = DiagrammTimestamp - 31622400;   // - 1 Jahr (366 Tage)
        } else {
          DiagrammTimestamp = DiagrammTimestamp - 31536000;   // - 1 Jahr (365 Tage)
        }
      }
    }
    if (Button == "2") {                                      // Button rechts
      if (year(DiagrammTimestamp) < year()) {
        if (month(DiagrammTimestamp) < 6) {
          DiagrammTimestamp = DiagrammTimestamp + 31622400;   // + 1 Jahr (366 Tage)
        } else {
          DiagrammTimestamp = DiagrammTimestamp + 31536000;   // + 1 Jahr (365 Tage)
        }
      }
    }
  }

  // Read Data from LittleFS
  String Line;
  int moint = 0;                            // Integer Monat
  int Space2;
  int Space3;
  int s0_count_int = 0;
  int s0_count_avg_int = 0;                 // S0-Counter Durchschnitt
  int s0_count_ges_int = 0;                 // S0-Counter gesamt
  unsigned int s0_count_max_int = 0;        // S0-Counter maximum
  float s0_count_max_fl = 0;                // S0-Counter maximum
  int Faktor = 1;
  unsigned int s0_count_int_arr[13] = {0};  // S0-Counter Monat
  if (year(DiagrammTimestamp) == year()) {            // aktuelles Jahr
    s0_count_int_arr[month()] = s0_count_month;       // S0-Count aktueller Monat übernehmen
    s0_count_max_int = s0_count_month;                // S0-Count aktueller Monat in Maximum übernehmen
    s0_count_ges_int = s0_count_month;                // S0-Count aktueller Monat in Gesamt übernehmen
  }
  FileName = F("/log/y_");                           // Logdatei Jahr
  FileName += (year(DiagrammTimestamp));
  FileName += F(".log");
  if (LittleFS.exists(FileName)) {                // Returns true if a file with given path exists, false otherwise.
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("LittleFS Reading Data from: "));
    Serial.println(FileName);
#endif
    File LogFile = LittleFS.open(FileName, "r");      // Open text file for reading.
    while (LogFile.available()) {
      Line = LogFile.readStringUntil('\n');         // Lets read line by line from the file
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
      Serial.println(Line);
#endif
      moint = Line.substring(3, 5).toInt();         // Month
      Space2 = Line.indexOf(' ', 19);               // nach Uhrzeit folgt Count absolut
      Space3 = Line.indexOf(' ', Space2 + 1 );      // nach absulut Count folgt Count
      s0_count_int = Line.substring(Space3 + 1).toInt(); // liest String bis Zeilenende
      s0_count_int_arr[moint] = s0_count_int_arr[moint] + s0_count_int;
      s0_count_ges_int = s0_count_ges_int + s0_count_int;   // S0-Counter gesamt
      if (s0_count_int_arr[moint] > s0_count_max_int) {
        s0_count_max_int = s0_count_int_arr[moint];         // S0-Counter Maximum
      }
    }
    LogFile.close();
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("Gesamt: "));
    Serial.println(s0_count_ges_int);     // S0-Counter gesamt
#endif
  } else {
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("LittleFS Logile not exist: "));
    Serial.println(FileName);
#endif
  }
  s0_count_max_fl = s0_count_max_int / 100.0;
  while (s0_count_max_int > 100) {
    s0_count_max_int = s0_count_max_int / 10;
    Faktor = Faktor * 10;
  }
  if (Faktor >= 10) {
    if (s0_count_max_int < 20) {
      s0_count_max_int = s0_count_max_int * 5;
      Faktor = Faktor / 5;
    }
    if (s0_count_max_int < 50) {
      s0_count_max_int = s0_count_max_int * 2;
      Faktor = Faktor / 2;
    }
  }

  // Beginn HTML
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/buttonxl.css\">");
  insertMenu_Diagramme(sResponse);                  // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);               // Überschrift Tabelle Zeile 2 einfügen

  sResponse += F("<tr><td class=\"CXB\">");
  if (year(DiagrammTimestamp) > FirstYear) {
    sResponse += F("<button type=\"submit\" name=\"Button\" value=\"1\">&#9664;</button>");
    //sResponse += "<button type=\"submit\" name=\"Button\" value=\"1\">&#9668;</button>";
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td>"
                 "<td class=\"CXB\" colspan=\"3\">Verbrauch im Jahr ");
  sResponse += (year(DiagrammTimestamp));
  sResponse += F("</td>"
                 "<td class=\"CXB\">");
  if (year(DiagrammTimestamp) < year()) {
    sResponse += F("<button type=\"submit\" name=\"Button\" value=\"2\">&#9654;</button>");
    //sResponse += "<button type=\"submit\" name=\"Button\" value=\"2\">&#9658;</button>";
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td></tr>"
                 "<tr><td class=\"c\" colspan=\"5\">"
                 // Beginn Diagramm
                 "<svg width=\"735\" height=\"350\">"
                 // Diagramm Hintergrund hellgrau und Rahmen schwarz Breite 1
                 "<rect x=\"50\" y=\"25\" width=\"672\" height=\"300\" fill=\"#f5f5f5\" stroke-width=\"1\" stroke=\"#000\"></rect>"
                 // Gitterlinien Farbe grau Breite 1
                 "<g stroke=\"#ccc\" stroke-width=\"1\">");
  // Gitterlinien vertikal
  for (int i = 1; i < 12; ++ i) {             // 12 Monate
    sResponse += F("<path d=\"M ");
    sResponse += i * 56 + 50;                 // x-position x-lines
    sResponse += F(" 25 v 305\"></path>");    // vertikale Linie ab y25 Länge 305
  }
  // Gitterlinien horizontal
  for (int i = 1; i < 10; i += 1) {
    sResponse += F("<path d=\"M 45 ");          // x-position y-lines
    sResponse += 325 - i * 30;                  // y-position y-lines (300 + 25)
    sResponse += F(" h 677\"></path>");         // vertikale Linie Länge 677
  }
  sResponse += F("</g>"                       // Ende Group
                 // Beschriftung Y-Achse
                 "<g text-anchor=\"end\">");
  for (int i = 0; i < 11; i += 1) {
    float yscala = 0.1 * Faktor * i;            // Beschriftung Y-Achse
    String ytext;
    if (Faktor >= 10) {
      ytext =  String(yscala, 0);              // using a float and 0 decimal places
    } else {
      ytext =  String(yscala, 1);             // using a float and 1 decimal places
    }
    sResponse += F("<text x=\"43\" y=\"");
    sResponse += 330 - i * 30;                // y-position y-text (300 + 25 + 5)
    sResponse += F("\">");
    sResponse += ytext;                       // text y-axis
    sResponse += F("</text>");
  }
  sResponse += F("</g>"                       // Ende Group
                 // Beschriftung X-Achse
                 "<g text-anchor=\"middle\">");
  for (int i = 0; i < 12; ++ i) {               // 12 Monate
    sResponse += F("<text x=\"");
    sResponse += i * 56 + 78;                   // x-position x-text (50+28)
    sResponse += F("\" y=\"345\">");
    sResponse += i + 1;                         // text x-axis
    sResponse += F("</text>");
  }
  sResponse += F("</g>"                       // Ende Group
                 // Säulen zeichnen
                 "<g fill=\"#ff0\" stroke-width=\"1\" stroke=\"#000\">");
  for (int i = 1; i <= 12; ++ i) {                              // 12 Monate
    int sh = s0_count_int_arr[i] / Faktor * 3;                  // Höhe Säule 3 Pixel pro Digit
    s0_count_avg_int = s0_count_avg_int + s0_count_int_arr[i];  // Durchschnitt addieren
    if (sh > 0) {
      sResponse += F("<rect x=\"");
      sResponse += i * 56 - 1;                    // x-position start Säule (50 + 5 - 56)
      sResponse += F("\" y=\"");
      sResponse += 325 - sh;                      // y-position start Balken (300 + 25)
      sResponse += F("\" width=\"46\" height=\"");
      sResponse += sh;                            // Säulenhöhe
      sResponse += F("\"></rect>");
    }
  }
  sResponse += F("</g>"                       // Ende Group
                 // Beschriftung oben
                 "<text x=\"80\" y=\"20\">Durchschnitt: ");
  float fl = s0_count_avg_int / 1200.0;              // Durchschnitt (12 Monate / 100)
  sResponse += fl;                                   // Durchschnitt
  sResponse += F(" m³/M</text>)"
                 "<text x=\"405\" y=\"20\" text-anchor=\"middle\">Maximum: ");  // Max
  sResponse += s0_count_max_fl;                      // Maximum
  sResponse += F(" m³/M</text>"
                 "<text x=\"690\" y=\"20\" text-anchor=\"end\">Gesamt: ");      // Gesamt
  fl = s0_count_ges_int / 100.0;                    // Gesamtverbrauch
  sResponse += fl;                                  // Gesamt
  sResponse += F(" m³</text>"
                 // SVG Ende
                 "</svg></td></tr>");                                           // end svg

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## Diagramm Jahre ##############################
void SiteDiagramm_jahre() {
  String sResponse = "";                        // Response HTML
  sResponse.reserve(8192);
  String Button = server.arg("Button");
  // erste vorhandene Logdatei Jahr ermitteln
  int FirstYear = year() - 1;
  String FileName = F("/log/y_");
  FileName += (FirstYear);
  FileName += F(".log");
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  SerialPrintLine();            // Trennlinie seriell ausgeben
  Serial.print(F("Search first year: "));
  Serial.println(FirstYear);
#endif
  while (LittleFS.exists(FileName)) {        // Returns true if a file with given path exists, false otherwise.
    FirstYear -= 1;
    FileName = F("/log/y_");
    FileName += (FirstYear);
    FileName += F(".log");
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
    Serial.print(F("Search first year: "));
    Serial.println(FirstYear);
#endif
  }
  FirstYear += 1;
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.print(F("First year: "));
  Serial.println(FirstYear);
#endif

  // Read Data from LittleFS
  String Line;
  int Space2;
  int Space3;
  int s0_count_int = 0;
  int s0_count_avg_int = 0;                         // S0-Counter Durchschnitt
  int s0_count_ges_int = 0;                         // S0-Counter gesamt
  unsigned int s0_count_max_int = 0;                // S0-Counter maximum
  float s0_count_max_fl = 0;                        // S0-Counter maximum
  int Faktor = 1;
  unsigned int s0_count_int_arr[100] = {0};         // S0-Counter für 100 Jahre
  int CountYear = year() - FirstYear + 1;           // Anzahl Jahre
  s0_count_int_arr[year() - 2000] = s0_count_month; // S0-Count aktueller Monat übernehmen
  s0_count_max_int = s0_count_month;                // S0-Count aktueller Monat in Maximum übernehmen
  s0_count_ges_int = s0_count_month;                // S0-Count aktueller Monat in Gesamt übernehmen
  // Daten aus Logs übernehmen
  for (int y = FirstYear; y < year() + 1; y++) {
    FileName = F("/log/y_");                            // Logdatei Jahr
    FileName += y;
    FileName += F(".log");
    if (LittleFS.exists(FileName)) {                      // Returns true if a file with given path exists, false otherwise.
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
      Serial.print(F("LittleFS Reading Data from: "));
      Serial.println(FileName);
#endif
      File LogFile = LittleFS.open(FileName, "r");        // Open text file for reading.
      while (LogFile.available()) {
        Line = LogFile.readStringUntil('\n');           // Lets read line by line from the file
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
        Serial.println(Line);
#endif
        Space2 = Line.indexOf(' ', 19);                       // nach Uhrzeit folgt Count absolut
        Space3 = Line.indexOf(' ', Space2 + 1 );              // nach absulut Count folgt Count
        s0_count_int = Line.substring(Space3 + 1).toInt();    // Integer S0-Count momentan
        s0_count_int_arr[y - 2000] += s0_count_int;           // S0-Count Monat addieren
        s0_count_ges_int = s0_count_ges_int + s0_count_int;   // S0-Counter gesamt
        if (s0_count_int_arr[y - 2000] > s0_count_max_int) {
          s0_count_max_int = s0_count_int_arr[y - 2000];      // S0-Counter Maximum
        }
      }
      LogFile.close();
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
      Serial.print(F("Gesamt: "));
      Serial.print(s0_count_int_arr[y - 2000]); // S0-Counter gesamt
      Serial.print(F(" im Jahr "));
      Serial.println(y);     // Jahreszahl
#endif
    } else {
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
      Serial.print(F("LittleFS Logile not exist: "));
      Serial.println(FileName);
#endif
    }
  }
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.print(F("Gesamt: "));
  Serial.print(s0_count_ges_int);     // S0-Counter gesamt
  Serial.print(F(" in "));
  Serial.print(CountYear);     // S0-Counter gesamt
  Serial.println(F(" Jahr(en)"));
#endif

  // Faktor für Skalierung Y-Achse ermitteln
  s0_count_max_fl = s0_count_max_int / 100.0;
  while (s0_count_max_int > 100) {
    s0_count_max_int = s0_count_max_int / 10;
    Faktor = Faktor * 10;
  }
  if (Faktor >= 10) {
    if (s0_count_max_int < 20) {
      s0_count_max_int = s0_count_max_int * 5;
      Faktor = Faktor / 5;
    }
    if (s0_count_max_int < 50) {
      s0_count_max_int = s0_count_max_int * 2;
      Faktor = Faktor / 2;
    }
  }

  // Beginn HTML
  insertHeaderCSS(sResponse);                       // Header und CCS einfügen
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/buttonxl.css\">");
  insertMenu_Diagramme(sResponse);                  // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);               // Überschrift Tabelle Zeile 2 einfügen
  sResponse += F("<tr><td class=\"CXB\" colspan=\"5\">Verbrauch pro Jahr</td></tr>"
                 "<tr><td class=\"c\" colspan=\"5\">");
  // Beginn Diagramm
  int xDistance = 682 / CountYear;                // Abstand = maximale Breite 682 / Anzahl Jahre
  int FrameWidth = xDistance * CountYear;         // Anpassung Rahmenbreite an Anzahl Jahre
  sResponse += F("<svg width=\"735\" height=\"350\">"
                 // Diagramm Hintergrund hellgrau und Rahmen schwarz Breite 1
                 "<rect x=\"50\" y=\"25\" width=\"");
  sResponse += FrameWidth;
  sResponse += F("\" height=\"300\" fill=\"#f5f5f5\" stroke-width=\"1\" stroke=\"#000\"></rect>"
                 // Gitterlinien Farbe grau Breite 1
                 "<g stroke=\"#ccc\" stroke-width=\"1\">");
  // Gitterlinien vertikal
  for (int i = 1; i < CountYear; ++ i) {        // Anzahl Jahre
    sResponse += F("<path d=\"M ");
    sResponse += i * xDistance + 50;            // x-position x-lines
    sResponse += F(" 25 v 305\"></path>");      // vertikale Linie ab y25 Länge 305
  }
  // Gitterlinien horizontal
  for (int i = 1; i < 10; i += 1) {
    sResponse += F("<path d=\"M 45 ");          // x-position y-lines
    sResponse += 325 - i * 30;                  // y-position y-lines (300 + 25)
    sResponse += F(" h ");
    sResponse += FrameWidth + 5;
    sResponse += F("\"></path>");               // vertikale Linie
  }
  sResponse += F("</g>"                       // Ende Group
                 // Beschriftung Y-Achse
                 "<g text-anchor=\"end\">");
  for (int i = 0; i < 11; i += 1) {
    float yscala = 0.1 * Faktor * i;           // Beschriftung Y-Achse
    String ytext;
    if (Faktor >= 10) {
      ytext =  String(yscala, 0);              // using a float and 0 decimal places
    } else {
      ytext =  String(yscala, 1);              // using a float and 1 decimal places
    }
    sResponse += F("<text x=\"43\" y=\"");
    sResponse += 330 - i * 30;                 // y-position y-text (300 + 25 + 5)
    sResponse += F("\">");
    sResponse += ytext;                        // text y-axis
    sResponse += F("</text>");
  }
  sResponse += F("</g>"                      // Ende Group
                 // Beschriftung X-Achse
                 "<g text-anchor=\"middle\">");
  for (int i = 0; i < CountYear; ++ i) {       // Anzahl Jahre
    int xt = i * xDistance + 50 + xDistance / 2; // x-position x-text
    sResponse += F("<text x=\"");
    sResponse += xt;                           // x-position
    sResponse += F("\" y=\"345\">");
    sResponse += FirstYear + i;                // text x-axis
    sResponse += F("</text>");
  }
  sResponse += F("</g>"                       // Ende Group
                 // Säulen zeichnen
                 "<g fill=\"#ff0\" stroke-width=\"1\" stroke=\"#000\">");
  for (int i = 0; i < CountYear; ++ i) {                                            // Anzahl Jahre
    int sh = s0_count_int_arr[FirstYear - 2000 + i] / Faktor * 3;                   // Höhe Säule 3 Pixel pro Digit
    s0_count_avg_int = s0_count_avg_int + s0_count_int_arr[FirstYear - 2000 + i];   // Durchschnitt addieren
    if (sh > 0) {
      sResponse += F("<rect x=\"");
      sResponse += i * xDistance + 55;        // x-position start Säule (50 + 5)
      sResponse += F("\" y=\"");
      sResponse += 325 - sh;                  // y-position start Säule (300 + 25)
      sResponse += F("\" width=\"");
      sResponse += xDistance - 10;            // Säulenbreite
      sResponse += F("\" height=\"");
      sResponse += sh;                        // Säulenhöhe
      sResponse += F("\"></rect>");
    }
  }
  sResponse += F("</g>"                       // Ende Group
                 // Beschriftung oben Durchschnitt, Maximum, Gesamt
                 "<text x=\"80\" y=\"20\">Durchschnitt: ");   // X-Position Text (Rahmenbreite + 50 + 30)
  float fl = s0_count_avg_int / CountYear / 100.0;            // Durchschnitt (Anzahl Jahre / 100)
  sResponse += fl;                                   // Durchschnitt
  sResponse += F(" m³/a</text>)"
                 "<text x=\"405\" y=\"20\" text-anchor=\"middle\">Maximum: ");  // X-Position Text
  sResponse += s0_count_max_fl;                      // Maximum
  sResponse += F(" m³/a</text>"
                 "<text x=\"690\" y=\"20\" text-anchor=\"end\">Gesamt: ");
  fl = s0_count_ges_int / 100.0;                    // Gesamtverbrauch
  sResponse += fl;                                  // Gesamt
  sResponse += F(" m³</text>"
                 // SVG Ende
                 "</svg></td></tr>");                                           // end svg
  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## index.htm ##############################
void SiteIndex()  {
  String sResponse = "";                        // Response HTML
  sResponse.reserve(4096); // ohne: Dauer: 2747 µS, 1968 Byte, Free Heap: 29.76 kB
  String Wochentag[8] = {"", "Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Sonnabend"};

  insertHeaderCSS(sResponse);                   // Header und CCS einfügen
  sResponse += F("<script>setTimeout(function(){window.location.href='index.htm'},60000);</script>");
  insertMenu_Index(sResponse);                  // Index Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);           // Überschrift Tabelle Zeile 2 einfügen

  // Tabelle Zeile 3 einfuegen
  sResponse += F("<tr><td class=\"CXB\" colspan=\"5\">");
  sResponse += Wochentag[weekday()];
  sResponse += F(", ");
  sResponse += DateToString(now());                   // return Date String from Timestamp
  sResponse += F(" - ");
  sResponse += TimeToStringHM(now());                 // return Time String from Timestamp
  sResponse += F(" Uhr</td></tr>"
                 // Tabelle Zeile 4 einfuegen
                 "<tr><td class=\"rxx\" colspan=\"2\">Zählerstand</td>"
                 "<td class=\"rxx\" colspan=\"2\">");
  sResponse += longToStr(s0_count_abs);               // Zählerstand absolut
  sResponse += F("</td>"
                 "<td class=\"lxx\">m³</td></tr>"
                 // Tabelle Zeile 5 einfuegen
                 "<tr><td class=\"CXB\" colspan=\"5\">Verbrauch</td></tr>"
                 "<tr><td class=\"rx\" colspan=\"2\">heute</td>"
                 "<td class=\"rx\" colspan=\"2\">");
  sResponse += longToStr(s0_count_day);               // Zählerstand Tag
  sResponse += F("</td>"
                 "<td class=\"lx\">m³</td></tr>"
                 // Tabelle Zeile 6 einfuegen
                 "<tr><td class=\"rx\" colspan=\"2\">diesen Monat</td>"
                 "<td class=\"rx\" colspan=\"2\">");
  sResponse += longToStr(s0_count_month);             // Zählerstand Monat
  sResponse += F("</td>"
                 "<td class=\"lx\">m³</td></tr>"
                 // Tabelle Zeile 7 einfuegen
                 "<tr><td class=\"rx\" colspan=\"2\">dieses Jahr</td>"
                 "<td class=\"rx\" colspan=\"2\">");
  sResponse += longToStr(s0_count_year);              // Zählerstand Jahr
  sResponse += F("<td class=\"lx\">m³</td></tr>"
                 // Tabelle Zeile 8 einfuegen
                 "<tr><td class=\"rx\" colspan=\"2\">seit ");
  unsigned long l = EEPROM_read_long(EEPROM_ADDR_S0TRIP1TIME);            // read timestamp
  sResponse += DateToString(l);     // return Date String from Timestamp
  sResponse += F(", ");
  sResponse += TimeToStringHM(l);     // return Time String from Timestamp
  sResponse += F(" Uhr</td>"
                 "<td class=\"rx\" colspan=\"2\">");
  l = EEPROM_read_long(EEPROM_ADDR_S0TRIP1);                  // read S0-Count Trip begin
  sResponse += longToStr(s0_count_abs - l);              // S0-Count Trip
  sResponse += F("</td>"
                 "<td class = \"lx\">m³</td></tr>"
                 // Tabelle Zeile 9 einfuegen
                 "<tr><td class=\"rx\" colspan=\"2\">seit ");
  l = EEPROM_read_long(EEPROM_ADDR_S0TRIP2TIME);            // read timestamp
  sResponse += DateToString(l);     // return Date String from Timestamp
  sResponse += F(", ");
  sResponse += TimeToStringHM(l);     // return Time String from Timestamp
  sResponse += F(" Uhr</td>"
                 "<td class=\"rx\" colspan=\"2\">");
  l = EEPROM_read_long(EEPROM_ADDR_S0TRIP2);                  // read S0-Count Trip begin
  sResponse += longToStr(s0_count_abs - l);              // S0-Count Trip
  sResponse += F("</td>"
                 "<td class = \"lx\">m³</td></tr>");

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

void Site404()  {
  String sResponse = F("<html><head><title>404 Not Found</title></head><body>"
                       "<h1>Not Found</h1>"
                       "<p>The requested URL was not found on this webserver.</p>"
                       "</body></html>");
  server.send(404, "text/html", sResponse);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.print(F("requested URL not found: "));
  Serial.println(server.uri());
  Serial.print(F("HTTP TX Bytes: "));
  Serial.println(sResponse.length());
#endif
  ulWifiTxBytes += sResponse.length();
}

void insertHeaderCSS(String & str) {                     // Header und CCS einfügen
  digitalWrite(LED_red, LOW) ;     // LED ein
  str += F("<!DOCTYPE html><html lang=\"de\"><head>" // <html> element must have a lang attribute: The <html> element does not have a lang attribute
           // 'charset' meta element should be specified using shorter '<meta charset="utf-8">' form.
           "<meta charset=\"utf-8\">"  // 'charset' meta element should be the first thing in '<head>'.
           // "<meta content=\"text/html\" http-equiv=\"Content-Type\">"
           // "<meta content=\"de\" http-equiv=\"Content-Language\">"
           // "<meta name=\"viewport\" content=\"width=device-width, user-scalable=yes\">"
           // The 'viewport' meta element 'content' attribute value should not contain 'user-scalable'.
           "<meta name=\"viewport\" content=\"width=device-width\">"
           "<title>Gaszähler ");
  str += OwnStationHostname;
  str += F("</title>"
           "<link rel=\"stylesheet\" type=\"text/css\" href=\"css/style.css\">");
}

void insertMenu_Index(String & str) {                     // Index Menu Tabelle Zeile 1 einfügen
  str += F("</head><body><form>"
           "<table class=\"table802\">"
           // Menue Tabelle Zeile 1 einfügen
           "<tr><td class=\"CLB\"><a href=\"index.htm\">"
           "<img src=\"pic/logo.svg\" height=\"32\" width=\"32\" alt=\"Home\"></a></td>"
           "<td class=\"CLB\"><a href=\"diagramm_d.htm\">Diagramme</a></td>"
           "<td class=\"CLB\"><a href=\"statistik.htm\">Statistiken</a></td>"
           "<td class=\"CLB\"><a href=\"info.htm\">Informationen</a></td>"
           "<td class=\"CLB\"><a href=\"setup.htm\">Einstellungen</a></td></tr>");
}

void insertMenu_Setup(String & str) {                     // Setup Menu Tabelle Zeile 1 einfügen
  str += F("</head><body><form method=\"post\">"
           //  str += F("</head><body><form method=\"get\">"
           "<table class=\"table802\">"
           "<tr><td class=\"CLB\"><a href=\"index.htm\">"
           "<img src=\"pic/logo.svg\" height=\"32\" width=\"32\" alt=\"Home\"></a></td>"
           "<td class=\"CLB\"><a href=\"setup.htm\">diverse</a></td>"
           "<td class=\"CLB\"><a href=\"setuptime.htm\">Zeit</a></td>"
           "<td class=\"CLB\"><a href=\"setuplan.htm\">Netzwerk</a></td>"
           "<td class=\"CLB\"><a href=\"setupmqtt.htm\">MQTT</a></td></tr>");
}

void insertMenu_Diagramme(String & str) {                 // Diagramme Menu Tabelle Zeile 1 einfügen
  String FileName = F("/log/y_");
  FileName += (year() - 1);
  FileName += F(".log");
  str += F("</head><body><form method=\"get\">"
           "<table class=\"table802\">"
           "<tr><td class=\"CLB\"><a href=\"index.htm\">"
           "<img src=\"pic/logo.svg\" height=\"32\" width=\"36\" alt=\"Home\"></a></td>"
           "<td class=\"CLB\"><a href=\"diagramm_d.htm\">Tag</a></td>"
           "<td class=\"CLB\"><a href=\"diagramm_m.htm\">Monat</a></td>"
           "<td class=\"CLB\"><a href=\"diagramm_j.htm\">Jahr</a></td>"
           "<td class=\"CLB\">");
  if (LittleFS.exists(FileName)) {                // Returns true if a file with given path exists, false otherwise.
    str += F("<a href=\"diagramm_jahre.htm\">Jahre</a>");
  } else {
    str += F("&nbsp;");                                 // Menuepunkt "Jahre" nicht anzeigen
  }
  str += F("</td></tr>");
}

void insertMenu_Log(String & str) {                     // Log Menu Tabelle Zeile 1 einfügen
  str += F("</head><body><form method=\"get\">"
           "<table class=\"table802\">"
           "<tr><td class=\"CLB\"><a href=\"info.htm\">"
           "<img src=\"pic/logo.svg\" height=\"32\" width=\"32\" alt=\"Info\"></a></td>"
           "<td class=\"CLB\"><a href=\"log_d.htm\">Tag</a></td>"
           "<td class=\"CLB\"><a href=\"log_m.htm\">Monat</a></td>"
           "<td class=\"CLB\"><a href=\"log_j.htm\">Jahr</a></td>"
           "<td class=\"CLB\"><a href=\"log.htm\">System</a></td></tr>");
}

void insertMenu_Ueberschrift(String & str) {                     // Überschrift Tabelle Zeile 2 einfügen
  str += F("<tr><td class=\"TDH1\" colspan=\"5\">"
           "<h1>Gaszähler GZ16</h1>"
           "</td></tr>");
}

void insertFooterSend(String & str) {                            // Footer anhängen und senden
  // Tabelle 5. Zeile
  str += F("<tr><td class=\"CSBFN\">WLAN RSSI: ");
  str += WiFi.RSSI();                                       // WLAN RSSI
  str += F(" dB</td>"
           "<td class=\"CSBFN\">Neuverbindungen: ");
  str += reconnCount;                                     // WLAN Neuverbindungen
  str += F("</td>"
           "<td class = \"CSBFN\">Uptime: ");
  str += UpTimeToString(uptime);                            // return Uptime String from Timestamp
  str += F("</td>"
           "<td class=\"CSBFN\">&nbsp;</td>"                // leere Zelle
           "<td class=\"CSBFN\">&copy; UB 2016</td></tr>"   // Copyright
           // Ende Tabelle, Form, Body, Html
           "</table></form></body></html>");
  ulWifiTxBytes = ulWifiTxBytes + str.length();
  server.sendHeader(F("cache-control"), F("private, no-cache"));  // A 'cache-control' header is missing or empty.
  server.sendHeader(F("x-content-type-options"), F("nosniff"));   // Response should include 'x-content-type-options' header.
  server.sendHeader(F("content-type"), F("text/html; charset=utf-8"));   // Response should include 'content-type' header.
  server.send ( 200, "text/html; charset=utf-8", str );           // 'content-type' header charset value should be 'utf-8'.
  digitalWrite(LED_red, HIGH) ;     // LED aus
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.print(F("Website "));
  Serial.print(server.uri());
  Serial.print(F(" HTTP TX Bytes: "));
  Serial.print(str.length());
  Serial.print(F(", Free Heap: "));
  Serial.println(formatBytes(ESP.getFreeHeap()).c_str());
#endif
}

String listDirectories(String dirName) {
  String sResponse = "";                        // Response HTML
  Dir dir = LittleFS.openDir("/" + dirName);
  while (dir.next()) {
    if (dir.isFile()) {
      sResponse += F("<tr><td>");
      sResponse += dirName;
      if (dirName.length()) {
        sResponse += '/';
      }
      sResponse += dir.fileName();
      sResponse += F("</td><td>&nbsp;");
      sResponse += DateTimeToString(dir.fileTime()); // return Date and Time String from Timestamp
      sResponse += F("&nbsp;</td><td class=\"r\" nowrap=\"nowrap\">");
      sResponse += formatBytes(dir.fileSize());
      if (dir.fileSize() < 1024) {
        sResponse += F("&nbsp;");
      }
      sResponse += F("</td></tr>");
    }
  }
  return sResponse;
}
