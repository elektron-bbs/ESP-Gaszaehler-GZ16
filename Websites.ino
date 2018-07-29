// ############################## Statistik ##############################
void SiteStatistik()  {
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    SerialPrintLine();
  }
  String sResponse = "";                        // Response HTML
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
    sResponse += F("<tr><td class=\"rx\" colspan=\"1\">pro Tag</td>"
                   "<td class=\"rx\" colspan=\"2\">");
    sResponse += (DateToString(l));
    sResponse += F("</td>"
                   "<td class=\"rx\" colspan=\"1\">");
    sResponse += longToStr(s0_count_max_day);                          // read max S0-Count day
    sResponse += F(" m³</td>"
                   "<td class=\"lx\" colspan=\"1\">&nbsp;</td></tr>");
  }
  l = DS1307_read_long(DS1307_ADDR_MAXS0MONTHTIME);                    // read timestamp Maximum month
  if (l != 0xFFFFFFFF) {                                               // Zeit bereits geschrieben
    c += 1;                                                            // Counter erhöhen
    sResponse += F("<tr><td class=\"rx\" colspan=\"1\">pro Monat</td>"
                   "<td class=\"rx\" colspan=\"2\">");
    sResponse += (month(l));
    sResponse += F("/");
    sResponse += (year(l));
    sResponse += F("</td>"
                   "<td class=\"rx\" colspan=\"1\">");
    sResponse += longToStr(s0_count_max_month);                         // read max S0-Count month
    sResponse += F(" m³</td>"
                   "<td class=\"lx\" colspan=\"1\">&nbsp;</td></tr>");
  }
  l = DS1307_read_long(DS1307_ADDR_MAXS0YEARTIME);                      // read timestamp Maximum year
  if (l != 0xFFFFFFFF) {                                                // Zeit bereits geschrieben
    c += 1;                                                             // Counter erhöhen
    sResponse += F("<tr><td class=\"rx\" colspan=\"1\">pro Jahr</td>"
                   "<td class=\"rx\" colspan=\"2\">");
    sResponse += (year(l));
    sResponse += F("</td>"
                   "<td class=\"rx\" colspan=\"1\">");
    sResponse += longToStr(s0_count_max_year);                          // read max S0-Count year
    sResponse += F(" m³</td>"
                   "<td class=\"lx\" colspan=\"1\">&nbsp;</td></tr>");
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
  if (SerialOutput == 1) {        // serielle Ausgabe eingeschaltet
    Serial.print(F("heute -11 Monate: "));
    Serial.println(DateToString(l));
  }
  while (month(l) % 3 != 0) {     // Prüfung Monat durch 3 teilbar
    if (day(l) < 15) {
      l = l + 2678400;            // + 1 Monat (31 Tage)
    } else {
      l = l + 2419200;            // + 1 Monat (28 Tage)
    }
  }
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.print(F("Beginn Jahreszeit: "));
    Serial.println(DateToString(l));
  }
  String Line;
  String da;                                // String Tag
  int daint = 0;                            // Integer Tag
  String mo;                                // String Monat
  int moint = 0;                            // Integer Monat
  String ye;                                // String Year
  int yeint = 0;                            // Integer Year
  String datum;                             // String Datum
  int Space2;
  int Space3;
  String s0_count_str;
  long s0_count_int = 0;
  long s0_count_avg_int = 0;                 // S0-Counter Durchschnitt
  long s0_count_ges_int = 0;                 // S0-Counter gesamt
  byte monat = month(l);                     // Monat Beginn
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.print(F("Monat Beginn: "));
    Serial.println(monat);
  }
  int tage = 0;
  String FileName = (F("/log/y_"));
  FileName += (year(l));
  FileName += (F(".log"));
nocheinmal:               // Sprungmarke für 2. Durchlauf bei 2 Jahren
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.print(F("Datei: "));
    Serial.println(FileName);
  }
  if (SPIFFS.exists(FileName)) {    // Returns true if a file with given path exists, false otherwise.
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(F("SPIFFS Reading Data from: "));
      Serial.println(FileName);
    }
    File LogFile = SPIFFS.open(FileName, "r");      // Open text file for reading.
    while (LogFile.available()) {
      Line = LogFile.readStringUntil('\n');         // Lets read line by line from the file
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        Serial.println(Line);
      }
      da = Line.substring(0, 2);                    // String Tag
      daint = da.toInt();                           // Integer Tag
      mo = Line.substring(3, 5);                    // Month
      moint = mo.toInt();                           // Integer Monat
      ye = Line.substring(6, 10);                   // String Year
      yeint = ye.toInt();                           // Integer Year
      datum = Line.substring(0, 10);                // String Datum
      Space2 = Line.indexOf(' ', 19);               // nach Uhrzeit folgt Count absolut
      Space3 = Line.indexOf(' ', Space2 + 1 );      // nach absulut Count folgt Count
      s0_count_str = Line.substring(Space3 + 1);    // liest String bis Zeilenende
      s0_count_int = s0_count_str.toInt();          // Integer
      while (moint > (monat + 2)) {
        monat += 3;
      }
      if (moint >= monat && moint <= monat + 2) {
        s0_count_ges_int = s0_count_ges_int + s0_count_int;
        tage = tage + daint;
      }
      if (moint == monat + 2) {                     // Jahreszeit
        sResponse = WebsiteStatistikJahreszeiten(sResponse, monat, s0_count_ges_int, tage, ye);
        // Ende Zeitraum einfügen
        sResponse += F(" - ");
        sResponse += datum;                                     // Datum einfügen
        sResponse += F("</td>"
                       "<td class=\"rx\" colspan=\"1\">");
        sResponse += longToStr(s0_count_ges_int);               // Gesamtverbrauch einfügen
        sResponse += F(" m³</td>"
                       "<td class=\"rx\" colspan=\"1\">");
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
    if (SPIFFS.exists(FileName)) {  // Returns true if a file with given path exists, false otherwise.
      goto nocheinmal;              // 2. Durchlauf starten
    } else {                        // Datei aktuelles Jahr existiert auch nicht
      monat = month();              // aktuellen Monat übernehmen
      ye = year();
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
  sResponse = WebsiteStatistikJahreszeiten(sResponse, month(), s0_count_ges_int, tage, ye);
  // Ende Zeitraum einfügen
  sResponse += F(" - ");
  sResponse += DateToString(now());                       // Datum einfügen
  sResponse += F("</td>"
                 "<td class=\"rx\" colspan=\"1\">");
  sResponse += longToStr(s0_count_ges_int);               // Gesamtverbrauch einfügen
  sResponse += F(" m³</td>"
                 "<td class=\"rx\" colspan=\"1\">");
  s0_count_avg_int = s0_count_ges_int / tage;
  sResponse += longToStr(s0_count_avg_int);               // Durchschnitt Verbrauch pro Tag einfügen
  sResponse += F(" m³/d&nbsp;</td></tr>");

  // Footer anhängen und senden
  insertFooterSend(sResponse);                                      // Footer anhängen und senden
}

// ############################## Einstellungen diverse anzeigen ##############################
void SiteSetup()  {
  String sResponse = "";                        // Response HTML
  String qs0count = server.arg("s0count");      // S0-Count
  String submit = server.arg("submit");         // welcher Button wurde betätigt
  int countargs = server.args();                // Anzahl Argumente
  if (countargs != 0) {
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      SerialPrintLine();            // Trennlinie seriell ausgeben
    }
#if DEBUG_OUTPUT == true
    Serial.print(F("S0-Count: "));
    Serial.println(qs0count);
    Serial.print(F("Button wurde betaetigt(submit): "));
    Serial.println(submit);
    Serial.print(F("Anzahl Argumente(countargs): "));
    Serial.println(countargs);
#endif
    if (submit == "saves0count") {                        // Button "Zählerstand speichern" betätigt
      // Prüfung auf Zahl einfügen
      qs0count.replace(",", ".");                         // Komma durch Punkt ersetzen
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        Serial.print(F("S0-Count: "));
        Serial.println(qs0count);
      }
      s0_count_abs = round(qs0count.toFloat() * 100.0);   // "toFloat" funktioniert nicht richtig, deshalb runden
      DS1307_write_long(DS1307_ADDR_S0COUNTABS, s0_count_abs);     // save S0-Counter to DS1307 RAM
      String logtext = F("Set Counter to: ");
      logtext += longToStr(s0_count_abs);
      logtext += F(" m³");
      appendLogFile(logtext);
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        Serial.println(logtext);
      }
    }
    if (submit == "resetMaxDay") {                                    // Button "Rückstellen" betätigt
      s0_count_max_day = 0;                                           // Maximum pro Tag löschen
      DS1307_write_long(DS1307_ADDR_MAXS0DAY, s0_count_max_day);      // Maximum pro Tag speichern
      DS1307_write_long(DS1307_ADDR_MAXS0DAYTIME, now());             // aktuelle Uhrzeit speichern
      String logtext = F("Reset max consumption per day");
      appendLogFile(logtext);
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        Serial.println(logtext);
      }
    }
    if (submit == "resetMaxMonth") {                                  // Button "Rückstellen" betätigt
      s0_count_max_month = 0;                                         // Maximum pro Monat löschen
      DS1307_write_long(DS1307_ADDR_MAXS0MONTH, s0_count_max_month);  // Maximum Monat speichern
      DS1307_write_long(DS1307_ADDR_MAXS0MONTHTIME, now());           // aktuelle Uhrzeit speichern
      String logtext = F("Reset max consumption per month");
      appendLogFile(logtext);
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        Serial.println(logtext);
      }
    }
    if (submit == "resetMaxYear") {                                   // Button "Rückstellen" betätigt
      s0_count_max_year = 0;                                          // Maximum pro Jahr löschen
      DS1307_write_long(DS1307_ADDR_MAXS0YEAR, s0_count_max_year);    // Maximum Jahr speichern
      DS1307_write_long(DS1307_ADDR_MAXS0YEARTIME, now());            // aktuelle Uhrzeit speichern
      String logtext = F("Reset max consumption per year");
      appendLogFile(logtext);
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        Serial.println(logtext);
      }
    }
    if (submit == "sets0trip1") {                                     // Button "Rückstellen" betätigt
      EEPROM_write_long(EEPROM_ADDR_S0TRIP1, s0_count_abs);           // S0-Count Trip begin set to S0-Count
      EEPROM_write_long(EEPROM_ADDR_S0TRIP1TIME, now());              // S0-Count Trip time set
      String logtext = F("Reset Counter 1 to: ");
      logtext += longToStr(s0_count_abs);
      logtext += F(" m³");
      appendLogFile(logtext);
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        Serial.println(logtext);
      }
    }
    if (submit == "sets0trip2") {                                     // Button "Rückstellen" betätigt
      EEPROM_write_long(EEPROM_ADDR_S0TRIP2, s0_count_abs);           // S0-Count Trip begin set to S0-Count
      EEPROM_write_long(EEPROM_ADDR_S0TRIP2TIME, now());              // S0-Count Trip time set
      String logtext = F("Reset Counter 2 to: ");
      logtext += longToStr(s0_count_abs);
      logtext += F(" m³");
      appendLogFile(logtext);
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        Serial.println(logtext);
      }
    }
    if (submit == "setser") {
      SerialOutput = !SerialOutput;// toggle boolean
      EEPROM_write_byte(EEPROM_ADDR_SERIALOUTPUT, SerialOutput);                        // write byte at address
      String logtext = F("Serial Output: ");
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        logtext += F("on");
      } else {                    // serielle Ausgabe ausgeschaltet
        logtext += F("off");
      }
      appendLogFile(logtext);
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        Serial.println(logtext);
      }
    }
  }
  // Beginn HTML
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"static/button.css\">");
  insertMenu_Setup(sResponse);                       // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);                // Überschrift Tabelle Zeile 2 einfügen
  // Zeile 3 einfügen
  sResponse += F("<tr><td class=\"CXB\" colspan=\"5\">diverse Einstellungen</td></tr>");
  // Tabelle Zeile 4 einfügen
  sResponse += F("<tr>"
                 "<td colspan=\"2\" class=\"r\">Zählerstand:</td>"
                 "<td colspan=\"1\" class=\"r\"><input type=number min=0.00 max=99999.99 step=0.01 name=\"s0count\" maxlength=\"8\" value=\"");
  sResponse += longToStr(s0_count_abs);
  sResponse += F("\"></td>"
                 "<td colspan=\"1\">m³</td>"
                 "<td colspan=\"1\"><button type=\"submit\" name=\"submit\" value=\"saves0count\">Speichern</button></td>"
                 "</tr>"
                 // Tabelle Zeile 5 einfügen
                 "<tr><td colspan=\"2\" class=\"r\">Maximalverbrauch pro Tag:</td>"
                 "<td colspan=\"1\" class=\"r\">");
  sResponse += longToStr(s0_count_max_day);    // Maximum pro Tag
  sResponse += F("</td>"
                 "<td colspan=\"1\">m³</td>"
                 "<td colspan=\"1\"><button type=\"submit\" name=\"submit\" value=\"resetMaxDay\">Rückstellen</button></td>"
                 "</tr>"
                 // Tabelle Zeile 6 einfügen
                 "<tr><td colspan=\"2\" class=\"r\">Maximalverbrauch pro Monat:</td>"
                 "<td colspan=\"1\" class=\"r\">");
  sResponse += longToStr(s0_count_max_month);    // Maximum pro Monat
  sResponse += F("</td>"
                 "<td colspan=\"1\">m³</td>"
                 "<td colspan=\"1\"><button type=\"submit\" name=\"submit\" value=\"resetMaxMonth\">Rückstellen</button></td>"
                 "</tr>"
                 // Tabelle Zeile 7 einfügen
                 "<tr><td colspan=\"2\" class=\"r\">Maximalverbrauch pro Jahr:</td>"
                 "<td colspan=\"1\" class=\"r\">");
  sResponse += longToStr(s0_count_max_year);    // Maximum pro Jahr
  sResponse += F("</td>"
                 "<td colspan=\"1\">m³</td>"
                 "<td colspan=\"1\"><button type=\"submit\" name=\"submit\" value=\"resetMaxYear\">Rückstellen</button></td>"
                 "</tr>"
                 // Tabelle Zeile 8 einfügen
                 "<tr><td colspan=\"2\" class=\"r\">Verbrauch seit ");
  unsigned long l = EEPROM_read_long(EEPROM_ADDR_S0TRIP1TIME);            // read timestamp
  sResponse += DateToString(l);     // return Date String from Timestamp
  sResponse += F(", ");
  sResponse += TimeToStringHM(l);     // return Time String from Timestamp
  sResponse += F(" Uhr:</td>"
                 "<td colspan=\"1\" class=\"r\">");
  l = EEPROM_read_long(EEPROM_ADDR_S0TRIP1);                  // read S0-Count Trip begin
  sResponse += longToStr(s0_count_abs - l);
  sResponse += F("</td>"
                 "<td colspan=\"1\">m³</td>"
                 "<td colspan=\"1\"><button type=\"submit\" name=\"submit\" value=\"sets0trip1\">Rückstellen</button></td>"
                 "</tr>"
                 // Tabelle Zeile 9 einfügen
                 "<tr><td colspan=\"2\" class=\"r\">Verbrauch seit ");
  l = EEPROM_read_long(EEPROM_ADDR_S0TRIP2TIME);            // read timestamp
  sResponse += DateToString(l);     // return Date String from Timestamp
  sResponse += F(", ");
  sResponse += TimeToStringHM(l);     // return Time String from Timestamp
  sResponse += F(" Uhr:</td>"
                 "<td colspan=\"1\" class=\"r\">");
  l = EEPROM_read_long(EEPROM_ADDR_S0TRIP2);                  // read S0-Count Trip begin
  sResponse += longToStr(s0_count_abs - l);
  sResponse += F("</td>"
                 "<td colspan=\"1\">m³</td>"
                 "<td colspan=\"1\"><button type=\"submit\" name=\"submit\" value=\"sets0trip2\">Rückstellen</button></td>"
                 "</tr>"
                 // Tabelle Zeile 10 einfügen
                 "<tr><td colspan=\"2\" class=\"r\">serielle Ausgabe:</td>"
                 "<td colspan=\"1\" class=\"r\">");
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    sResponse += F("ein");
  } else {                    // serielle Ausgabe ausgeschaltet
    sResponse += F("aus");
  }
  sResponse += F("</td>"
                 "<td colspan=\"1\">&nbsp;</td>"
                 "<td colspan=\"1\"><button type=\"submit\" name=\"submit\" value=\"setser\">");
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
  String qbroker = server.arg("broker");        // Broker
  String quser = server.arg("user");            // Username
  String qpass = server.arg("pass");            // Passwort
  String qport = server.arg("port");            // Port
  String qabs = server.arg("abs");              // Zählerstand
  String qmom = server.arg("mom");              // Momentanverbrauch
  String qrssi = server.arg("rssi");            // WLAN-RSSI
  String qivall = server.arg("ivall");          // Intervall
  String submit = server.arg("submit");         // welcher Button wurde betätigt
  int countargs = server.args();                // Anzahl Argumente
  if (countargs != 0) {
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      SerialPrintLine();            // Trennlinie seriell ausgeben
    }
#if DEBUG_OUTPUT == true
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
        if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
          Serial.println(logtext);
        }
        if (client.connected()) {                           // MQTT Broker connected
          client.disconnect();                                // Disconnects the client
        }
        EEPROM_write_boolean(EEPROM_ADDR_MQTTCONNECT, MqttConnect);   // write MQTT-Connect
      } else {
        MqttConnect = true;                           // MQTT einschalten
        if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
          Serial.print(F("MQTT: Connect to broker: "));
          Serial.print(qbroker);
          Serial.print(F(" Port: "));
          Serial.println(qport);
        }
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
          if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
            Serial.println(logtext);
          }
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
          if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
            Serial.println(logtext);
          }
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
      eMqttPublish_Intervall = qivall.toInt();          // Intervall
      EEPROM_write_byte(EEPROM_ADDR_MQTTINTERVALL, eMqttPublish_Intervall);          // write byte at address
      appendLogFile(F("MQTT publish settings saved"));
    }
  }
  // Beginn HTML
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"static/button.css\">");
  insertMenu_Setup(sResponse);                       // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);                // Überschrift Tabelle Zeile 2 einfügen
  // Zeile 3 einfügen
  sResponse += F("<tr><td class=\"CXB\" colspan=\"5\">Einstellungen MQTT Server</td></tr>"
                 // Zeile 4 einfügen
                 "<tr><td colspan=\"1\" class=\"r\">MQTT-Broker:</td>"
                 "<td colspan=\"1\" class=\"l\"><input name=\"broker\" type=\"text\" maxlength=\"64\" value=\"");
  sResponse += eMqttBroker;
  sResponse += F("\"></td>"
                 "<td colspan=\"3\" class=\"l\">(Name oder Adresse, leer um Dienst abzuschalten)</td></tr>"
                 // Zeile 5 einfügen
                 "<tr><td colspan=\"1\" class=\"r\">Username:</td>"
                 "<td colspan=\"4\" class=\"l\"><input name=\"user\" type=\"text\" maxlength=\"64\" value=\"");
  sResponse += eMqttUsername;
  sResponse += F("\"></td></tr>"
                 // Zeile 6 einfügen
                 "<tr><td colspan=\"1\" class=\"r\">Passwort:</td>"
                 "<td colspan=\"4\" class=\"l\"><input name=\"pass\" type=\"password\" maxlength=\"64\" value=\"");
  sResponse += eMqttPassword;
  sResponse += F("\"></td></tr>"
                 // Zeile 7 einfügen
                 "<tr><td colspan=\"1\" class=\"r\">Port:</td>"
                 "<td colspan=\"3\" class=\"l\"><input type=\"number\" name=\"port\" maxlength=\"5\" value=\"");
  sResponse += eMqttPort;
  sResponse += F("\"></td>"
                 "<td colspan=\"1\"><button type=\"submit\" name=\"submit\" value=\"connect\">Verbinden</button></td>"
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
  sResponse += F("<tr><td colspan=\"5\" class=\"CXB\">Publizierung durch Energiemonitor</td></tr>"
                 // Zeile 10 einfügen
                 "<tr><td colspan=\"1\" class=\"r\">Zählerstand:</td>"
                 "<td colspan=\"1\" class=\"l\"><input name=\"abs\" type=\"checkbox\" value=\"1\"");
  if (eMqttPublish_s0_count_abs == true) {
    sResponse += F(" checked=\"checked\"");
  }
  sResponse += F("></td><td colspan=\"3\" class=\"l\">");
  if (eMqttPublish_s0_count_abs == true) {
    sResponse += OwnStationHostname;
    sResponse += F("/CountAbs");
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td></tr>"
                 // Zeile 11 einfügen
                 "<tr><td colspan=\"1\" class=\"r\">Momentanverbrauch:</td>"
                 "<td colspan=\"1\" class=\"l\"><input name=\"mom\" type=\"checkbox\" value=\"1\"");
  if (eMqttPublish_s0_count_mom == true) {
    sResponse += F(" checked=\"checked\"");
  }
  sResponse += F("></td><td colspan=\"3\" class=\"l\">");
  if (eMqttPublish_s0_count_mom == true) {
    sResponse += OwnStationHostname;
    sResponse += F("/CountMom");
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td></tr>"
                 // Zeile 12 einfügen
                 "<tr><td colspan=\"1\" class=\"r\">RSSI:</td>"
                 "<td colspan=\"1\" class=\"l\"><input name=\"rssi\" type=\"checkbox\" value=\"1\"");
  if (eMqttPublish_rssi == true) {
    sResponse += F(" checked=\"checked\"");
  }
  sResponse += F("></td><td colspan=\"3\" class=\"l\">");
  if (eMqttPublish_rssi == true) {
    sResponse += OwnStationHostname;
    sResponse += F("/RSSI");
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td></tr>"
                 // Zeile 13 einfügen
                 "<tr><td colspan=\"1\" class=\"r\">Intervall:</td>"
                 "<td colspan=\"3\" class=\"l\">"
                 "<select name=\"ivall\">");
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
                 "<td colspan=\"1\"><button type=\"submit\" name=\"submit\" value=\"save\">Speichern</button></td>"
                 "</tr>");

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## Einstellungen WLAN anzeigen ##############################
void SiteSetupWifi()  {
  String sResponse = "";                        // Response HTML
  IPAddress ipadr;                              // IP address
  int errorIpAddr = 0;                          // Error IP address
  String qssid = server.arg("setssid");         // Auswahl SSID
  String hssid = server.arg("hiddenssid");      // Auswahl versteckte SSID
  String qpass = server.arg("pw");              // WLAN Passwort
  String qdhcp = server.arg("dhcp");            // 1-DHCP ein, 0-DHCP aus
  String qip = server.arg("ip");                // IP-Adresse statisch
  String qdns = server.arg("dns");              // Domain Name Server statisch
  String qsnm = server.arg("sn");               // Subnetzmaske  statisch
  String qsgw = server.arg("gw");               // Gateway statisch
  String submit = server.arg("submit");         // welcher Button wurde betätigt
  int countargs = server.args();                // Anzahl Argumente
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    SerialPrintLine();                            // Trennlinie seriell ausgeben
  }
  if (countargs == 0) {                         // erster Aufruf ohne Argumente
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.println(F("Scanne Netzwerke..."));
    }
    WifiNetworks = WiFi.scanNetworks();         // Scanne Netzwerke
  }
#if DEBUG_OUTPUT == true
  Serial.print(F("Auswahl SSID(qssid): "));
  Serial.println(qssid);
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
  Serial.print(F("Anzahl Argumente(countargs): "));
  Serial.println(countargs);
  Serial.print(F("Anzahl gefundener Netzwerke(networks): "));
  Serial.println(WifiNetworks);
#endif
  // Beginn HTML
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"static/button.css\">");
  insertMenu_Setup(sResponse);                       // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);                // Überschrift Tabelle Zeile 2 einfügen
  // Zeile 3 einfügen
  sResponse += (F("<tr><td class=\"CXB\" colspan=\"5\">Einstellungen WLAN</td></tr>"));
  // Zeile 4 einfügen
  if (WifiNetworks == 0) {                      // kein Netzwerk gefunden!
    sResponse += F("<tr><td colspan=\"5\" class=\"cred\">"
                   "<br><br>Kein Netzwerk gefunden!<br><br><br></td></tr>"
                   "<tr><td colspan=\"2\"></td>"
                   "<td colspan=\"1\"><button type=\"submit\" name=\"submit\" value=\"rescan\">Scannen</button></td>"
                   "<td colspan=\"2\"></td></tr>"
                   "<tr><td colspan=\"5\"><br><br></td></tr>");
  } else {
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
          if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
            Serial.println(F("ERROR static IP-Address"));
          }
        }
        if (str2ip((char*)qsgw.c_str(), ipaddress)) {           // convert String to Array of 4 Bytes
          esgw = ipaddress;                                     // Standard-Gateway übernehmen
          EEPROM_write_ipaddress(EEPROM_ADDR_GATEWAY, esgw);    // write IPAddress to EEPROM
        } else {
          errorIpAddr = 2;                                      // Error IP address
          if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
            Serial.println(F("ERROR Standard-Gateway"));
          }
        }
        if (str2ip((char*)qdns.c_str(), ipaddress)) {           // convert String to Array of 4 Bytes
          edns = ipaddress;                                     // statischen Domain Name Server übernehmen
          EEPROM_write_ipaddress(EEPROM_ADDR_DNS, edns);        // write IPAddress to EEPROM
        } else {
          errorIpAddr = 3;                                      // Error IP address
          if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
            Serial.println(F("ERROR Domain Name Server"));
          }
        }
        if (str2ip((char*)qsnm.c_str(), ipaddress)) {           // convert String to Array of 4 Bytes
          esnm = ipaddress;                                     // Subnetmask übernehmen
          EEPROM_write_ipaddress(EEPROM_ADDR_NETMASK, esnm);    // write IPAddress to EEPROM
        } else {
          errorIpAddr = 4;                                      // Error IP address
          if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
            Serial.println(F("ERROR Subnetmask"));
          }
        }
      }
      if (errorIpAddr == 0) {                                    // no Error IP address
        EEPROM_write_boolean(EEPROM_ADDR_DHCP, edhcp);          // write EEPROM DHCP on or off
      }
    }
    // neue Tabelle innerhalb Zeile 4
    sResponse += F("<tr><td colspan=\"5\" class=\"c\">"
                   "<table align=center style=\"width: 100%\">"
                   // neue Tabelle Zeile 1 Überschrift
                   "<tr><th style=\"width: 6%\">&nbsp;</th>"
                   "<th class=\"l\">Name (SSID)</th>"
                   "<th>MAC-Adresse</th>"
                   "<th>Kanal</th>"
                   "<th>RSSI</th>"
                   "<th>Verschlüsselung</th></tr>");
    // neue Tabelle Netzwerke auflisten ab Zeile 2
    for (int i = 0; i < WifiNetworks; ++i) {
      sResponse += F("<tr>");
      // Spalte 1
      sResponse += F("<td><input type=\"radio\" name=\"setssid\" value=\"");
      sResponse += WiFi.SSID(i);
      sResponse += F("\"");
      if (WiFi.SSID(i) == essid) sResponse += (F(" checked"));
      sResponse += F("></td>");
      // Spalte 2
      sResponse += (F("<td class=\"l\">"));
      sResponse += WiFi.SSID(i);
      sResponse += (F("</td>"));
      // Spalte 3
      sResponse += (F("<td>"));
      uint8_t* MAC  = WiFi.BSSID(i);           // Gets the MAC address of the router you are connected to
      char str[18];
      sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X", MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);
      sResponse += str;
      sResponse += (F("</td>"));
      // Spalte 4
      sResponse += (F("<td align=\"center\">"));
      sResponse += WiFi.channel(i);
      sResponse += (F("</td>"));
      // Spalte 5
      sResponse += (F("<td align=\"center\">"));
      sResponse += WiFi.RSSI(i);
      sResponse += (F(" dB</td>"));
      // Spalte 6
      sResponse += (F("<td align=\"center\">"));
      switch (WiFi.encryptionType(i)) {
        case 2:
          sResponse += (F("WPA (TKIP)"));
          break;
        case 4:
          sResponse += (F("WPA2 (CCMP)"));
          break;
        case 5:
          sResponse += (F("WEP"));
          break;
        case 7:
          sResponse += (F("NONE"));
          break;
        case 8:
          sResponse += (F("AUTO"));
          break;
        default:
          sResponse += (F("unknown"));
          break;
      }
      sResponse += (F("</td></tr>"));
    }
    // neue Tabelle letzte Zeile Eingabefeld SSID
    sResponse += F("<tr><td><input name=\"setssid\" type=\"radio\" value=\"hidden\"></td>"
                   "<td class=\"l\"><input style=\"width:80%\" name=\"hiddenssid\" type=\"text\" maxlength=\"32\"></td>"
                   "<td>&nbsp;</td>"
                   "<td>&nbsp;</td>"
                   "<td>&nbsp;</td>"
                   "<td>&nbsp;</td></tr>"
                   // neue Tabelle Ende und Ende Zeile 4
                   "</table></td></tr>");
    // Zeile 5 einfügen
    sResponse += F("<tr><td colspan=\"1\"><button type=\"submit\" name=\"submit\" value=\"wps\">WPS</button></td>"
                   "<td colspan=\"1\" class=\"r\">Passwort:</td>"
                   "<td colspan=\"2\" class=\"l\"><input style=\"width:70%\" name=\"pw\" type=\"password\" maxlength=\"64\"></td>"
                   "<td colspan=\"1\"><button type=\"submit\" name=\"submit\" value=\"connect\">Verbinden</button></td></tr>");
    if (submit == "connect") {
      if (qssid.length() > 0 && qpass.length() > 0) {
        if (webtype == 1) {                             // Accesspoint
          WiFi.softAPdisconnect(false);                 // Accesspoint nicht abschalten
          WiFi.mode(WIFI_AP_STA);                       // Accesspoint und Station
        } else {
          WiFi.mode(WIFI_STA);                          // Station
        }
        if (qssid == "hidden") {                        // verstecktes Netzwerk
          qssid = hssid;                                // versteckte SSID übernehmen
        }
        WiFi.disconnect();
        String logtext = "Wifi try new connect to: ";
        logtext += qssid;
        appendLogFile(logtext);
        if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
          Serial.println("");
          Serial.println(logtext);
        }
        if (edhcp == false) {                           // DHCP aus
          //WiFi.config(eip, edns, esgw, esnm);         // Style Arduino
          WiFi.config(eip, esgw, esnm, edns);           // Style ESP8266
          if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
            Serial.println(F("DHCP off, use static IP"));
          }
        } else {                                        // DHCP ein
          wifi_station_dhcpc_start();
          if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
            Serial.println(F("DHCP on"));
          }
        }
        WiFi.begin(qssid.c_str(), qpass.c_str());       // Connect to new Network
        if (testWifi()) {
          essid = qssid;                                // SSID übernehmen
          epass = qpass;                                // Wifi Passwort übernehmen
          EEPROM_write_string(EEPROM_ADDR_SSID, essid);   // write String to EEPROM
          EEPROM_write_string(EEPROM_ADDR_PASS, epass);   // write String to EEPROM
          appendLogFile(F("Wifi settings saved"));
          // zusätzliche Zeile einfügen bei Connect
          sResponse += (F("<tr><td colspan=\"5\" class=\"c\">Verbunden mit Netzwerk: "));
          sResponse += essid;
          sResponse += (F("</td></tr>"));
        } else {
          WiFi.begin(essid.c_str(), epass.c_str());     // Connect to saved Network
          if (testWifi()) {
            // zusätzliche Zeile einfügen bei Connect
            sResponse += (F("<tr><td colspan=\"5\" class=\"c\">Verbunden mit Netzwerk: "));
            sResponse += essid;
            sResponse += (F("</td></tr>"));
          }
        }
      } else {
        // zusätzliche Zeile einfügen bei Fehler Name (SSID) oder Passwort zu kurz
        sResponse += (F("<tr><td colspan=\"5\" class=\"cred\">Name (SSID) oder Passwort zu kurz!</td></tr>"));
      }
    }
    if (submit == "wps") {
      sResponse += (F("<tr><td colspan=\"5\" class=\"c\">Starte WPS (Wi-Fi Protected Setup)</td></tr>"));
      // Footer anhängen und senden
      insertFooterSend(sResponse);                       // Footer anhängen und senden

      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        Serial.println(F("WPS-Button pressed"));
      }
      // starte WiFi Protected Setup (WPS)
      if (!start_WPS_connect()) {                         // Wifi WPS Connection failed
        //estart += 1;                                       // Start-Zähler erhöhen
        String logText = F("Wifi WPS connection failed #");
        logText += estart;
        appendLogFile(logText);
        if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
          Serial.println(logText);
        }
        //if (estart > 3) {                                     // max. 3 Versuche
        // 3 x Fehler WPS - starte Accesspoint
        //EEPROM_write_byte(EEPROM_ADDR_START, estart);      // write Start Count to EEPROM
        delay(1000);
        /* After programming your ESP, the ESP.restart(); does not always work.
           You have to reset it manually, after programming.
           Because after it reliable Works
        */
        //ESP.reset();
        ESP.restart();
        //}
      }
    }
    // Zeile 6 einfügen
    sResponse += (F("<tr><td class=\"CXB\" colspan=\"5\">Netzwerk</td></tr>"));
    // Zeile 7 einfügen
    sResponse += (F("<tr><td colspan=\"1\" class=\"r\"><input type=\"radio\" name=\"dhcp\" value=\"1\""));
    if (edhcp == true) sResponse += (F(" checked"));  // DHCP ein
    sResponse += (F("></td>"
                    "<td colspan = \"2\">IP-Adresse automatisch (DHCP)</td>"
                    "<td colspan=\"2\">"));
    if (edhcp == true) {                              // DHCP ein
      ipadr = WiFi.localIP();
      sResponse += String(ipadr[0]) + '.' + String(ipadr[1]) + '.' + String(ipadr[2]) + '.' + String(ipadr[3]);
    } else {                                          // DHCP aus
      sResponse += (F("&nbsp;"));
    }
    sResponse += (F("</td></tr>"));
    // Zeile 8 einfügen
    sResponse += (F("<tr><td colspan=\"1\" class=\"r\"><input type=\"radio\" name=\"dhcp\" value=\"0\""));
    if (edhcp == false) sResponse += (F(" checked")); // DHCP aus
    sResponse += (F("></td>"
                    "<td colspan=\"2\">IP-Adresse statisch</td>"
                    "<td colspan=\"2\">"
                    "<input name=\"ip\" type=\"text\" id=\"ip\" maxlength=\"15\" value=\""));
    sResponse += String(eip[0]) + '.' + String(eip[1]) + '.' + String(eip[2]) + '.' + String(eip[3]);
    sResponse += (F("\"></td></tr>"));
    // Zeile 9 einfügen
    sResponse += (F("<tr><td colspan=\"3\" class=\"r\">Domain Name Server:</td>"
                    "<td colspan=\"2\">"
                    "<input name=\"dns\" type=\"text\" id=\"dns\"  maxlength=\"15\" value=\""));
    if (edhcp == false) {                             // DHCP aus
      ipadr = edns;                                   // Domain Name Server übernehmen
    } else {                                          // DHCP ein
      ipadr = WiFi.gatewayIP();
    }
    sResponse += String(ipadr[0]) + '.' + String(ipadr[1]) + '.' + String(ipadr[2]) + '.' + String(ipadr[3]);
    sResponse += (F("\"></td></tr>"));
    // Zeile 10 einfügen
    sResponse += (F("<tr><td colspan=\"3\" class=\"r\">Standardgateway:</td>"
                    "<td colspan=\"2\">"
                    "<input name=\"gw\" type=\"text\" id=\"gw\"  maxlength=\"15\" value=\""));
    if (edhcp == false) {                             // DHCP aus
      ipadr = esgw;                                   // eigenes Standard-Gateway übernehmen
    } else {                                          // DHCP ein
      ipadr = WiFi.gatewayIP();
    }
    sResponse += String(ipadr[0]) + '.' + String(ipadr[1]) + '.' + String(ipadr[2]) + '.' + String(ipadr[3]);
    sResponse += (F("\"></td></tr>"));
    // Zeile 11 einfügen
    sResponse += (F("<tr><td colspan=\"3\" class=\"r\">Subnetzmaske:</td>"
                    "<td colspan=\"1\">"
                    "<input name=\"sn\" type=\"text\" id=\"sn\"  maxlength=\"15\" value=\""));
    if (edhcp == false) {                             // DHCP aus
      ipadr = esnm;                                   // eigene Subnetzkaske übernehmen
    } else {                                          // DHCP ein
      ipadr = WiFi.subnetMask();
    }
    sResponse += String(ipadr[0]) + '.' + String(ipadr[1]) + '.' + String(ipadr[2]) + '.' + String(ipadr[3]);
    sResponse += (F("\"></td><td colspan=\"1\"><button type=\"submit\" name=\"submit\" value=\"save\">Speichern</button></td></tr>"));
  }
  // zusätzliche Zeile einfügen bei Fehler IP Adresse
  if (errorIpAddr == 1) {                                    // Error IP address
    sResponse += (F("<tr><td colspan=\"5\" class=\"cred\">IP-Adresse statisch ungültig!</td></tr>"));
  }
  if (errorIpAddr == 2) {                                    // Error Standardgateway
    sResponse += (F("<tr><td colspan=\"5\" class=\"cred\">Standardgateway ungültig!</td></tr>"));
  }
  if (errorIpAddr == 3) {                                    // Error Domain Name Server
    sResponse += (F("<tr><td colspan=\"5\" class=\"cred\">Domain Name Server ungültig!</td></tr>"));
  }
  if (errorIpAddr == 4) {                                    // Error Subnetzmaske
    sResponse += (F("<tr><td colspan=\"5\" class=\"cred\">Subnetzmaske ungültig!</td></tr>"));
  }

  // Footer anhängen und senden
  insertFooterSend(sResponse);                       // Footer anhängen und senden
}

// ############################## Einstellungen Zeit anzeigen ##############################
void SiteSetupTime()  {
  String sResponse = "";                        // Response HTML
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
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        Serial.print(F("Set Date: "));
        Serial.println(DateToString(now()));        // return Date String from Timestamp
        Serial.print(F("Set Time: "));
        Serial.println(TimeToString(now()));        // return Time String from Timestamp
        Serial.print(F("Set DST: "));
        Serial.println(DST);
      }
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
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"static/button.css\">");
  insertMenu_Setup(sResponse);                       // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);                // Überschrift Tabelle Zeile 2 einfügen
  // Zeile 3 einfügen
  sResponse += F("<tr><td class=\"CXB\" colspan=\"5\">Datum und Uhrzeit</td></tr>");
  // Tabelle Zeile 4 einfügen
  sResponse += F("<tr><td colspan=\"1\" class=\"r\">Datum:</td>"
                 "<td colspan=\"4\" nowrap><input type=\"number\" name=\"da\" min=\"1\" max=\"31\" size=\"4\" maxlength=\"2\" value=\"");
  sResponse += day();
  sResponse += F("\">."
                 "<input type=\"number\" name=\"mo\" min=\"1\" max=\"12\" size=\"4\" maxlength=\"2\" value=\"");
  sResponse += month();
  sResponse += F("\">."
                 "<input type=\"number\" name=\"ye\" min=\"1970\" max=\"2038\" size=\"4\" maxlength=\"4\" value=\"");
  sResponse += year();
  sResponse += F("\"></td></tr>");
  // Tabelle Zeile 5 einfügen
  sResponse += F("<tr><td colspan=\"1\" class=\"r\">Uhrzeit:</td>"
                 "<td colspan=\"3\"><input type=\"number\" name=\"ho\" min=\"0\" max=\"23\" size=\"4\" maxlength=\"2\" value=\"");
  sResponse += hour();
  sResponse += F("\">:"
                 "<input type=\"number\" name=\"mi\" min=\"0\" max=\"59\" size=\"4\" maxlength=\"2\" value=\"");
  sResponse += minute();
  sResponse += F("\">:"
                 "<input type=\"number\" name=\"se\" min=\"0\" max=\"59\" size=\"4\" maxlength=\"2\" value=\"");
  sResponse += second();
  sResponse += F("\"></td>"
                 "<td colspan=\"1\"><button type=\"submit\" name=\"submit\" value=\"settime\">Stellen</button></td></tr>");
  // Tabelle Zeile 6 einfügen
  sResponse += F("<tr><td colspan=\"1\" class=\"r\">Sommerzeit:</td>"
                 "<td colspan=\"1\" class=\"c\"><input name=\"dst\" type=\"checkbox\" id=\"dst\"");
  if (DST == true) {
    sResponse += F(" checked");
  }
  sResponse += F("></td><td colspan=\"3\">&nbsp;</td></tr>");
  // Tabelle Zeile 7 einfügen
  sResponse += F("<tr><td class=\"CXB\" colspan=\"5\">Internetzeit</td></tr>");
  // Tabelle Zeile 8 einfügen
  sResponse += F("<tr><td colspan=\"1\" class=\"r\"><input type=\"radio\" name=\"ntp\" value=\"0\"");
  if (ntpServerNr == 0) {
    sResponse += F(" checked=\"checked\"");
  }
  sResponse += F("></td><td colspan=\"4\">");
  sResponse += ntpServerName[0];
  sResponse += F("</td></tr>");
  // Tabelle Zeile 9 einfügen
  sResponse += F("<tr><td colspan=\"1\" class=\"r\"><input type=\"radio\" name=\"ntp\" value=\"1\"");
  if (ntpServerNr == 1) {
    sResponse += F(" checked=\"checked\"");
  }
  sResponse += F("></td><td colspan=\"4\">");
  sResponse += ntpServerName[1];
  sResponse += F("</td></tr>");
  // Tabelle Zeile 10 einfügen
  sResponse += F("<tr><td colspan=\"1\" class=\"r\"><input type=\"radio\" name=\"ntp\" value=\"2\"");
  if (ntpServerNr == 2) {
    sResponse += F(" checked=\"checked\"");
  }
  sResponse += F("></td>"
                 "<td colspan=\"4\"><input name=\"own\" type=\"text\" id=\"own\"  maxlength=\"64\" value=\"");
  sResponse += ntpServerName[2];
  sResponse += F("\"></td></tr>"
                 // Tabelle Zeile 11 einfügen
                 "<tr><td colspan=\"1\" class=\"r\"><input type=\"radio\" name=\"ntp\" value=\"3\"");
  if (ntpServerNr == 3) {
    sResponse += F(" checked=\"checked\"");
  }
  sResponse += F("></td><td colspan=\"3\">keine Synchronisierung</td>"
                 "<td colspan=\"1\"><button type=\"submit\" name=\"submit\" value=\"setntp\">Speichern</button></td></tr>");
  // Tabelle Zeile 12 einfügen
  if (errorNTP == 1) {                             // Error NTP Settings
    sResponse += F("<tr><td colspan=\"5\" class=\"cred\">Fehler bei Zeitsynchronisation!</td></tr>");
  } else {
    sResponse += F("<tr><td colspan=\"1\">&nbsp;</td>"
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
  IPAddress ipadr;                             // IP address
  uint8_t* MAC;                                // MAC address
  uint8_t MAC_softAP[6];    //not sure why we have to pass a MAC address to get a MAC address.
  uint8_t MAC_STA[6];
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  sResponse += F("</head><body><form method=\"get\">"
                 "<table align=\"center\" style=\"width: 802px\">"
                 "<tr><td class=\"CLB\" style=\"width: 156px; \"><a href=\"index.htm\">"
                 "<img src=\"pic/logo.svg\" height=\"32\" width=\"32\" alt=\"Home\"></a></td>"
                 "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"info.htm\">Informationen</a></td>"
                 "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"log.htm\">Log</a></td>"
                 "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"files.htm\">Dateien</a></td>"
                 "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"static/doku.htm\">Dokumentation</a></td></tr>");
  insertMenu_Ueberschrift(sResponse);              // Überschrift Tabelle Zeile 2 einfügen
  // Zeile 3 einfügen
  sResponse += F("<tr><td class=\"CXB\" colspan=\"5\">Systeminformationen</td></tr>"
                 // Zeile 4 einfügen
                 "<tr><th colspan=\"2\">Netzwerk</th>"
                 "<td colspan=\"1\">&nbsp;</td>"
                 "<th colspan=\"2\">Hardware</th></tr>"
                 // Zeile 5 einfügen
                 "<tr><td colspan=\"1\">Hostname:</td>"
                 "<td colspan=\"1\">");
  if (webtype == 0) {                           // Station
    sResponse += WiFi.hostname();
  } else {                                      // Access Point
    sResponse += OwnStationHostname;
  }
  sResponse += F("</td>"
                 "<td colspan=\"1\">&nbsp;</td>"
                 "<td colspan=\"1\">ESP8266 Chip-ID:</td>"
                 "<td colspan=\"1\">");
  uint8_t ChipID[3];
  ChipID[0] = (ESP.getChipId() >> 0)  & 0xFF;
  ChipID[1] = (ESP.getChipId() >> 8)  & 0xFF;
  ChipID[2] = (ESP.getChipId() >> 16) & 0xFF;
  char str[20];
  sprintf(str, "%02X%02X%02X", ChipID[2], ChipID[1], ChipID[0]);
  sResponse += str;
  sResponse += F("</td></tr>");
  // Zeile 6 einfügen
  sResponse += F("<tr><td colspan=\"1\">MAC:</td>"
                 "<td colspan=\"1\">");
  if (webtype == 0) {                           // Station
    MAC = WiFi.macAddress(MAC_STA);             //get MAC address of Station interface
  } else {                                      // Access Point
    MAC  = WiFi.softAPmacAddress(MAC_softAP);   //get MAC address of softAP interface
  }
  sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X", MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);
  sResponse += str;
  sResponse += F("</td>"
                 "<td colspan=\"1\">&nbsp;</td>"
                 "<td colspan=\"1\">Flash Chip-ID:</td>"
                 "<td colspan=\"1\">");
  ChipID[0] = (ESP.getFlashChipId() >> 0)  & 0xFF;
  ChipID[1] = (ESP.getFlashChipId() >> 8)  & 0xFF;
  ChipID[2] = (ESP.getFlashChipId() >> 16) & 0xFF;
  sprintf(str, "%02X%02X%02X", ChipID[2], ChipID[1], ChipID[0]);
  sResponse += str;
  sResponse += F("</td></tr>");
  // Zeile 7 einfügen
  sResponse += F("<tr><td colspan=\"1\">IP:</td>"
                 "<td colspan=\"1\">");
  if (webtype == 0) {                         // Station
    ipadr = WiFi.localIP();
  } else {                                    // Access Point
    ipadr = WiFi.softAPIP();
  }
  sResponse += String(ipadr[0]) + '.' + String(ipadr[1]) + '.' + String(ipadr[2]) + '.' + String(ipadr[3]);
  sResponse += F("</td>"
                 "<td colspan=\"1\">&nbsp;</td>"
                 "<td colspan=\"1\">Flash Chip Size:</td>"
                 "<td colspan=\"1\">");
  sResponse += formatBytes(ESP.getFlashChipRealSize());
  sResponse += F("</td></tr>");
  // Zeile 8 einfügen
  sResponse += F("<tr><td colspan=\"1\">Gateway:</td>"
                 "<td colspan=\"1\">");
  ipadr = WiFi.gatewayIP();
  sResponse += String(ipadr[0]) + '.' + String(ipadr[1]) + '.' + String(ipadr[2]) + '.' + String(ipadr[3]);
  sResponse += F("</td>"
                 "<td colspan=\"1\">&nbsp;</td>"
                 "<td colspan=\"1\">Flash Chip Speed:</td>"
                 "<td colspan=\"1\">");
  sResponse += formatHertz(ESP.getFlashChipSpeed());
  sResponse += F("</td></tr>");
  // Zeile 9 einfügen
  sResponse += F("<tr><td colspan=\"1\">Netmask:</td>"
                 "<td colspan=\"1\">");
  ipadr = WiFi.subnetMask();
  sResponse += String(ipadr[0]) + '.' + String(ipadr[1]) + '.' + String(ipadr[2]) + '.' + String(ipadr[3]);
  sResponse += F("</td>"
                 "<td colspan=\"1\">&nbsp;</td>");
#ifdef HW_VERSION_1                                 // Hardware Version 1 mit ESP-12E
  sResponse += F("<td colspan=\"1\">Versorgungspannung:</td>"
                 "<td colspan=\"1\">");
  sResponse += String(ESP.getVcc() / 1000.0, 2);    // using a float and 2 decimal places
#else                                               // Hardware Version 2 mit NodeMCU
  //Serial.println(analogRead(A0));
  sResponse += F("<td colspan=\"1\">Batteriepannung:</td>"
                 "<td colspan=\"1\">");
  sResponse += String(analogRead(A0) / ADC_DIV, 2); // using a float and 2 decimal places
#endif
  sResponse += F(" Volt</td></tr>");
  // Zeile 10 einfügen
  sResponse += F("<tr><td colspan=\"1\">SSID:</td>"
                 "<td colspan=\"1\">");
  if (webtype == 0) {                           // Station
    sResponse += WiFi.SSID();
  } else {                                      // Access Point
    sResponse += OwnStationHostname;
  }
  sResponse += F("</td>"
                 "<td colspan=\"1\">&nbsp;</td>"
                 "<th colspan=\"2\">Firmware</th></tr>");
  // Zeile 11 einfügen
  sResponse += F("<tr><td colspan=\"1\">Router MAC:</td>"
                 "<td colspan=\"1\">");
  if (webtype == 0) {                           // Station
    MAC = WiFi.BSSID();                // Gets the MAC address of the router you are connected to
    sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X", MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);
    sResponse += str;
  } else {                                      // Access Point
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td>"
                 "<td colspan=\"1\">&nbsp;</td>"
                 "<td colspan=\"1\">Version:</td>"
                 "<td colspan=\"1\">");
  sResponse += VERSION;
  sResponse += F(".");
  sResponse += BUILD;
  sResponse += F("</td></tr>");
  // Zeile 12 einfügen
  sResponse += F("<tr><td colspan=\"1\">RSSI:</td>"
                 "<td colspan=\"1\">");
  sResponse += WiFi.RSSI();
  sResponse += F(" dB</td>"
                 "<td colspan=\"1\">&nbsp;</td>"
                 "<td colspan=\"1\">Datum:</td>"
                 "<td colspan=\"1\">");
  sResponse += compile_date;
  sResponse += F("</td></tr>");
  // Zeile 13 einfügen
  sResponse += F("<tr><td colspan=\"1\">Wifi RX/TX:</td>"
                 "<td colspan=\"1\">");
  sResponse += formatBytes(ulWifiTxBytes);
  sResponse += F("</td>"
                 "<td colspan=\"1\">&nbsp;</td>"
                 "<td colspan=\"1\">Größe:</td>"
                 "<td colspan=\"1\">");
  sResponse += formatBytes(ESP.getSketchSize());
  sResponse += F("</td></tr>");

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## Dateien anzeigen ##############################
void SiteFiles()  {
  String sResponse = "";                        // Response HTML
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  sResponse += (F("</head><body><form method=\"get\">"
                  "<table align=\"center\" style=\"width: 802px\">"
                  "<tr><td class=\"CLB\" style=\"width: 156px; \"><a href=\"index.htm\">"
                  "<img src=\"pic/logo.svg\" height=\"32\" width=\"32\" alt=\"Home\"></a></td>"
                  "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"info.htm\">Informationen</a></td>"
                  "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"log.htm\">Log</a></td>"
                  "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"files.htm\">Dateien</a></td>"
                  "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"static/doku.htm\">Dokumentation</a></td></tr>"));
  insertMenu_Ueberschrift(sResponse);              // Überschrift Tabelle Zeile 2 einfügen
  // Tabelle Zeile 3 einfügen
  sResponse += (F("<tr><td class=\"CXB\" colspan=\"5\">Dateien</td></tr>"
                  // Tabelle Zeile 4 einfügen
                  "<tr><td colspan=\"5\"><span class=\"font_c\">"

                  // neue Tabelle in Tabelle
                  "<table align=\"center\">"
                  "<tr><th align=\"left\">Name</th>"
                  "<th>&nbsp;</th>"
                  "<th align=\"center\">Größe</th></tr>"));

  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    sResponse += F("<tr><td>");
    String fileName = dir.fileName();
    fileName.remove(0, 1);
    sResponse += fileName;
    sResponse += F("</td><td>&nbsp;</td><td align=\"right\" nowrap=\"nowrap\">");
    sResponse += formatBytes(dir.fileSize());
    if (dir.fileSize() < 1024) {
      sResponse += F("&nbsp;");
    }
    sResponse += F("</td></tr>");
  }

  FSInfo fs_info;
  SPIFFS.info(fs_info);

  sResponse += F("<tr><td align=\"right\"><strong>verwendet:</strong></td>"
                 "<td>&nbsp;</td><td align=\"right\" nowrap=\"nowrap\"><strong>");
  sResponse += formatBytes(fs_info.usedBytes);
  sResponse += F("</strong></td></tr>");

  sResponse += F("<tr><td align=\"right\"><strong>Gesamtgröße:</strong></td>"
                 "<td>&nbsp;</td><td align=\"right\" nowrap=\"nowrap\"><strong>");
  sResponse += formatBytes(fs_info.totalBytes);
  sResponse += F("</strong></td></tr>");

  size_t freeBytes = fs_info.totalBytes - fs_info.usedBytes;
  sResponse += F("<tr><td align=\"right\"><strong>frei:</strong></td>"
                 "<td>&nbsp;</td><td align=\"right\" nowrap=\"nowrap\"><strong>");
  sResponse += formatBytes(freeBytes);
  sResponse += F("</strong></td></tr>"
                 "</table></span></td></tr>");

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## Log Tag anzeigen ##############################
void SiteLogTag()  {
  String sResponse = "";                        // Response HTML
  int rows = 0;                                 // Anzahl Zeilen
  int countargs = server.args();
  String Button = server.arg("Button");
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    SerialPrintLine();            // Trennlinie seriell ausgeben
  }
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
  if (SPIFFS.exists(FileName)) {        // Returns true if a file with given path exists, false otherwise.
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(F("SPIFFS Reading Data from: "));
      Serial.println(FileName);
    }
    File LogFile = SPIFFS.open(FileName, "r"); // Open text file for reading.
    while (LogFile.available()) {
      //Lets read line by line from the file
      String line = LogFile.readStringUntil('\n');
      rows += 1;
    }
    LogFile.close();
  } else {
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(F("SPIFFS Logfile not available: "));
      Serial.println(FileName);
    }
  }

  // Beginn HTML
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"static/buttonxl.css\">");
  insertMenu_Log(sResponse);                       // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);              // Überschrift Tabelle Zeile 2 einfügen

  // Tabelle Zeile 3 einfügen
  sResponse += F("<tr><td class=\"CXB\" colspan=\"1\">");
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
  sResponse += F("</td><td class=\"CXB\" colspan=\"1\">");
  if (DiagrammTimestamp < LastDayOfMonth(month(), year())) {
    sResponse += F("<button type=\"submit\" name=\"Button\" value=\"2\">&#9654;</button>");
    //sResponse += "<button type=\"submit\" name=\"Button\" value=\"2\">&#9658;</button>";
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td></tr>");
  // Tabelle Zeile 4 einfügen
  sResponse += F("<tr><td colspan=\"5\">"
                 "<table align=\"center\"><tr><td><span class=\"font_c\">");
  if (rows > 0) {
    File LogFile = SPIFFS.open(FileName, "r");         // Open text file for reading.
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
  sResponse += F("</span></td></tr></table></td></tr>");

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## Log Monat anzeigen ##############################
void SiteLogMonat()  {
  String sResponse = "";                        // Response HTML
  int rows = 0;                                 // Anzahl Zeilen
  String Monat[13] = {"", "Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"};
  int countargs = server.args();
  String Button = server.arg("Button");
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    SerialPrintLine();            // Trennlinie seriell ausgeben
  }
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
  if (SPIFFS.exists(FileName)) {        // Returns true if a file with given path exists, false otherwise.
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(F("SPIFFS Reading Data from: "));
      Serial.println(FileName);
    }
    File LogFile = SPIFFS.open(FileName, "r"); // Open text file for reading.
    while (LogFile.available()) {
      //Lets read line by line from the file
      String line = LogFile.readStringUntil('\n');
      rows += 1;
    }
    LogFile.close();
  } else {
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(F("SPIFFS Logfile not available: "));
      Serial.println(FileName);
    }
  }

  // Beginn HTML
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"static/buttonxl.css\">");
  insertMenu_Log(sResponse);                       // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);              // Überschrift Tabelle Zeile 2 einfügen

  // Tabelle Zeile 3 einfügen
  sResponse += F("<tr><td class=\"CXB\" colspan=\"1\">");
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
  sResponse += F("</td><td class=\"CXB\" colspan=\"1\">");
  if (DiagrammTimestamp < 12) {
    sResponse += F("<button type=\"submit\" name=\"Button\" value=\"2\">&#9654;</button>");
    //sResponse += "<button type=\"submit\" name=\"Button\" value=\"2\">&#9658;</button>";
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td></tr>");

  // Tabelle Zeile 4 einfügen
  sResponse += F("<tr><td colspan=\"5\">"
                 "<table align=\"center\"><tr><td><span class=\"font_c\">");
  if (rows > 0) {
    File LogFile = SPIFFS.open(FileName, "r");         // Open text file for reading.
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
  sResponse += F("</span></td></tr></table></td></tr>");

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## Log Jahr anzeigen ##############################
void SiteLogJahr()  {
  String sResponse = "";                        // Response HTML
  int rows = 0;                                 // Anzahl Zeilen
  int countargs = server.args();
  String Button = server.arg("Button");
  // erste vorhandene Logdatei Jahr ermitteln
  int FirstYear = year() - 1;
  String FileName = F("/log/y_");
  FileName += FirstYear;
  FileName += F(".log");
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    SerialPrintLine();            // Trennlinie seriell ausgeben
  }
#if DEBUG_OUTPUT == true
  Serial.print(F("Search first year: "));
  Serial.println(FirstYear);
#endif
  while (SPIFFS.exists(FileName)) {        // Returns true if a file with given path exists, false otherwise.
    FirstYear -= 1;
    FileName = F("/log/y_");
    FileName += (FirstYear);
    FileName += F(".log");
#if DEBUG_OUTPUT == true
    Serial.print(F("Search first year: "));
    Serial.println(FirstYear);
#endif
  }
  FirstYear += 1;
#if DEBUG_OUTPUT == true
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
  if (SPIFFS.exists(FileName)) {        // Returns true if a file with given path exists, false otherwise.
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(F("SPIFFS Reading Data from: "));
      Serial.println(FileName);
    }
    File LogFile = SPIFFS.open(FileName, "r"); // Open text file for reading.
    while (LogFile.available()) {
      //Lets read line by line from the file
      String line = LogFile.readStringUntil('\n');
      rows += 1;
    }
    LogFile.close();
  } else {
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(F("SPIFFS Logfile not available: "));
      Serial.println(FileName);
    }
  }
  // Beginn HTML
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"static/buttonxl.css\">");
  insertMenu_Log(sResponse);                       // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);              // Überschrift Tabelle Zeile 2 einfügen

  // Tabelle Zeile 3 einfügen
  sResponse += F("<tr><td class=\"CXB\" colspan=\"1\">");
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
  sResponse += F("</td><td class=\"CXB\" colspan=\"1\">");
  if (DiagrammTimestamp < year()) {
    sResponse += F("<button type=\"submit\" name=\"Button\" value=\"2\">&#9654;</button>");
    //sResponse += "<button type=\"submit\" name=\"Button\" value=\"2\">&#9658;</button>";
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td></tr>");
  // Tabelle Zeile 4 einfügen
  sResponse += F("<tr><td colspan=\"5\">"
                 "<table align=\"center\"><tr><td><span class=\"font_c\">");
  if (rows > 0) {
    File LogFile = SPIFFS.open(FileName, "r"); // Open text file for reading.
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
  sResponse += F("</span></td></tr></table></td></tr>");

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## Log System anzeigen ##############################
void SiteLogSystem()  {
  String sResponse = "";                        // Response HTML
  // Anzahl Zeilen ermitteln
  File logFile = SPIFFS.open("/log/system.log", "r");  // Open text file for reading.
  int i = 0;
  while (logFile.available()) {
    //Lets read line by line from the file
    String line = logFile.readStringUntil('\n');
    i += 1;
  }
  logFile.close();

  // Beginn HTML
  insertHeaderCSS(sResponse);                        // Header und CCS einfügen
  // Menu Tabelle Zeile 1 einfügen
  sResponse += F("</head><body><form method=\"get\">"
                 "<table align=\"center\" style=\"width: 802px\">"
                 "<tr><td class=\"CLB\" style=\"width: 156px; \"><a href=\"info.htm\">"
                 "<img src=\"pic/logo.svg\" height=\"32\" width=\"32\" alt=\"Info\"></a></td>"
                 "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"log_d.htm\">Tag</a></td>"
                 "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"log_m.htm\">Monat</a></td>"
                 "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"log_j.htm\">Jahr</a></td>"
                 "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"log.htm\">System</a></td></tr>");
  insertMenu_Ueberschrift(sResponse);               // Überschrift Tabelle Zeile 2 einfügen
  // Tabelle Zeile 3 einfuegen
  sResponse += F("<tr><td class=\"CXB\" colspan=\"5\">Systemlog</td></tr>");
  // Tabelle Zeile 4 einfügen
  sResponse += F("<tr><td colspan=\"5\">"
                 "<table align=\"center\"><tr><td><span class=\"font_c\">");
  logFile = SPIFFS.open("/log/system.log", "r");  // Open text file for reading.
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
    if (j < i) {
      sResponse += F("<br>");
    }
  }
  logFile.close();
  sResponse += F("</span></td></tr></table></td></tr>");

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## Diagramm Tag ##############################
void SiteDiagramm_d() {
  String Wochentag[8] = {"", "Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Sonnabend"};
  String sResponse = "";                                     // Response HTML
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

  // Read Data from SPIFFS
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    SerialPrintLine();            // Trennlinie seriell ausgeben
  }
  String Line;
  String ho;                                // String Stunde
  int hoint = 0;                            // Integer Stunde
  String ye = "";                           // String Year
  int yeint = 0;                            // Integer Year
  int Space2;
  int Space3;
  String s0_count_str;
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
  if (SPIFFS.exists(FileName)) {                  // Returns true if a file with given path exists, false otherwise.
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(F("SPIFFS Reading Data from: "));
      Serial.println(FileName);
    }
    File LogFile = SPIFFS.open(FileName, "r"); // Open text file for reading.
    while (LogFile.available()) {
      Line = LogFile.readStringUntil('\n');    //Lets read line by line from the file
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        Serial.println(Line);
      }
      ye = Line.substring(6, 10);               // String Year
      yeint = ye.toInt();                       // Integer Year
      ho = Line.substring(11, 13);                // String Stunde
      hoint = ho.toInt();                         // Integer Stunde
      Space2 = Line.indexOf(' ', 19);             // nach Uhrzeit folgt Count absolut
      Space3 = Line.indexOf(' ', Space2 + 1 );
      s0_count_str = Line.substring(Space3 + 1);  // liest String bis Zeilenende
      s0_count_int = s0_count_str.toInt();        // Integer
      if (yeint == year(DiagrammTimestamp)) {
        s0_count_int_arr[hoint] = s0_count_int_arr[hoint] + s0_count_int;
        s0_count_ges_int = s0_count_ges_int + s0_count_int;   // S0-Counter gesamt
      }
      if (s0_count_int_arr[hoint] > s0_count_max_int) {
        s0_count_max_int = s0_count_int_arr[hoint];           // S0-Counter hour maximum
      }
    }
    LogFile.close();
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(F("Gesamt: "));
      Serial.println(s0_count_ges_int);     // S0-Counter gesamt
    }
  } else {
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(F("SPIFFS Logile not exist: "));
      Serial.println(FileName);
    }
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
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"static/buttonxl.css\">");
  insertMenu_Diagramme(sResponse);                  // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);               // Überschrift Tabelle Zeile 2 einfügen

  sResponse += F("<tr><td class = \"CXB\" colspan=\"1\">");
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
  sResponse += F("</td>");
  sResponse += F("<td class=\"CXB\" colspan=\"1\">");
  if (day(DiagrammTimestamp) < day() || month(DiagrammTimestamp) < month() || year(DiagrammTimestamp) < year()) {
    sResponse += F("<button type=\"submit\" name=\"Button\" value=\"2\">&#9654;</button>");
    //sResponse += "<button type=\"submit\" name=\"Button\" value=\"2\">&#9658;</button>";
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td></tr>");
  // Tabelle Zeile 4 einfügen
  sResponse += F("<tr><td class=\"c\" colspan=\"5\">");

  // Beginn Diagramm
  sResponse += F("<svg width=\"735\" height=\"350\">");
  // Diagramm Hintergrund hellgrau und Rahmen schwarz Breite 1
  sResponse += F("<rect x=\"50\" y=\"25\" width=\"672\" height=\"300\" fill=\"#f5f5f5\" stroke-width=\"1\" stroke=\"#000\"></rect>");

  // Gitterlinien Farbe grau Breite 1
  sResponse += F("<g stroke=\"#ccc\" stroke-width=\"1\">");
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
  sResponse += F("</g>");                       // Ende Group

  // Beschriftung Y-Achse
  sResponse += F("<g text-anchor=\"end\">");
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
  sResponse += F("</g>");                       // Ende Group

  // Beschriftung X-Achse
  sResponse += F("<g text-anchor=\"middle\">");
  for (int i = 0; i < 24; ++ i) {               // 24h
    sResponse += F("<text x=\"");
    sResponse += i * 28 + 64;                   // x-position x-text (50 + 14)
    sResponse += F("\" y=\"345\">");
    sResponse += i;                             // text x-axis
    sResponse += F("</text>");
  }
  sResponse += F("</g>");                       // Ende Group

  // Säulen zeichnen
  sResponse += F("<g fill=\"#ff0\" stroke-width=\"1\" stroke=\"#000\">");
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
  sResponse += F("</g>");                           // Ende Group

  // Beschriftung oben
  sResponse += F("<text x=\"80\" y=\"20\">Durchschnitt: ");
  float fl = s0_count_avg_int / 2400.0;              // Durchschnitt (24h /100)
  sResponse += fl;                                   // Durchschnitt
  sResponse += F(" m³/h</text>)"
                 "<text x=\"405\" y=\"20\" text-anchor=\"middle\">Maximum: ");  // Max
  sResponse += s0_count_max_fl;                      // Maximum
  sResponse += F(" m³/h</text>"
                 "<text x=\"690\" y=\"20\" text-anchor=\"end\">Gesamt: ");      // Gesamt
  fl = s0_count_ges_int / 100.0;                    // Gesamtverbrauch
  sResponse += fl;                                  // Gesamt
  sResponse += F(" m³</text>");

  // SVG Ende
  sResponse += F("</svg></td></tr>");                                           // end svg

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## Diagramm Monat ##############################
void SiteDiagramm_m() {
  String sResponse = "";                        // Response HTML
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
#if DEBUG_OUTPUT == true
  Serial.print("1. Saturday in month: ");
  Serial.println(Saturday);
  Serial.print("1. Sunday im month:   ");
  Serial.println(Sunday);
#endif

  // Read Data from SPIFFS
  if (SerialOutput == 1) {        // serielle Ausgabe eingeschaltet
    SerialPrintLine();            // Trennlinie seriell ausgeben
  }
  String Line;
  String da;                                // String Tag
  int daint = 0;                            // Integer Tag
  String ye = "";                           // String Year
  int yeint = 0;                            // Integer Year
  int Space2;
  int Space3;
  String s0_count_str;
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
  if (SPIFFS.exists(FileName)) {                  // Returns true if a file with given path exists, false otherwise.
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(F("SPIFFS Reading Data from: "));
      Serial.println(FileName);
    }
    File LogFile = SPIFFS.open(FileName, "r");    // Open text file for reading.
    while (LogFile.available()) {
      Line = LogFile.readStringUntil('\n');       //Lets read line by line from the file
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        Serial.println(Line);
      }
      ye = Line.substring(6, 10);                 // String Year
      yeint = ye.toInt();                         // Integer Year
      da = Line.substring(0, 2);                  // String Tag
      daint = da.toInt();                         // Integer Tag
      Space2 = Line.indexOf(' ', 19);             // nach Uhrzeit folgt Count absolut
      Space3 = Line.indexOf(' ', Space2 + 1 );
      s0_count_str = Line.substring(Space3 + 1);  // liest String bis Zeilenende
      s0_count_int = s0_count_str.toInt();        // Integer
      if (yeint == year(DiagrammTimestamp)) {
        s0_count_int_arr[daint] = s0_count_int_arr[daint] + s0_count_int;
        s0_count_ges_int = s0_count_ges_int + s0_count_int;   // S0-Counter gesamt
      }
      if (s0_count_int_arr[daint] > s0_count_max_int) {
        s0_count_max_int = s0_count_int_arr[daint];        // S0-Counter hour maximum
      }
    }
    LogFile.close();
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(F("Gesamt: "));
      Serial.println(s0_count_ges_int);     // S0-Counter gesamt
    }
  } else {
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(F("SPIFFS Logile not exist: "));
      Serial.println(FileName);
    }
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
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"static/buttonxl.css\">");
  insertMenu_Diagramme(sResponse);                  // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);               // Überschrift Tabelle Zeile 2 einfügen
  sResponse += F("<tr><td class=\"CXB\" colspan=\"1\">");
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
  sResponse += F("</td><td class=\"CXB\" colspan=\"1\">");
  if (month(DiagrammTimestamp) < month() || year(DiagrammTimestamp) < year()) {
    sResponse += F("<button type=\"submit\" name=\"Button\" value=\"2\">&#9654;</button>");
    //sResponse += "<button type=\"submit\" name=\"Button\" value=\"2\">&#9658;</button>";
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td></tr>");
  sResponse += F("<tr><td class=\"c\" colspan=\"5\">");
  // Beginn Diagramm
  // AnzahlTage 28,  29,  30,  31
  int db[4] = {672, 667, 660, 682};                   // Diagrammbreite
  int xa[4] = {24, 23, 22, 22};                       // Abstand X-Raster
  int sb[4] = {14, 13, 12, 12};                       // Säulenbreite
  int arindex = AnzahlTage - 28;
  // Beginn SVG Breite maximal
  sResponse += F("<svg width=\"735\" height=\"350\">");  // Beginn SVG Breite maximal
  // Diagramm Hintergrund hellgrau und Rahmen schwarz Breite 1
  sResponse += F("<rect x=\"50\" y=\"25\" width=\"");  // x-position y-position
  sResponse += db[arindex];                           // Diagrammbreite
  sResponse += F("\" height=\"300\" fill=\"#f5f5f5\" stroke-width=\"1\" stroke=\"#000\"></rect>");

  // Gitterlinien Farbe grau Breite 1
  sResponse += F("<g stroke=\"#ccc\" stroke-width=\"1\">");
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
  sResponse += F("</g>");                     // Ende Group

  // Beschriftung Y-Achse
  sResponse += F("<g text-anchor=\"end\">");
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
  sResponse += F("</g>");                     // Ende Group

  // Beschriftung X-Achse
  sResponse += F("<g text-anchor=\"middle\">");
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
  sResponse += F("</g>");                       // Ende Group

  // Säulen zeichnen
  sResponse += F("<g fill=\"#ff0\" stroke-width=\"1\" stroke=\"#000\">");
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
  sResponse += F("</g>");                     // Ende Group

  // Beschriftung oben
  sResponse += F("<text x=\"80\" y=\"20\">Durchschnitt: ");
  float fl = s0_count_avg_int / AnzahlTage / 100.0;  // Durchschnitt (AnzahlTage / 100)
  sResponse += fl;                                   // Durchschnitt
  sResponse += F(" m³/d</text>)"
                 "<text x=\"405\" y=\"20\" text-anchor=\"middle\">Maximum: ");  // Max
  sResponse += s0_count_max_fl;                      // Maximum
  sResponse += F(" m³/d</text>"
                 "<text x=\"690\" y=\"20\" text-anchor=\"end\">Gesamt: ");      // Gesamt
  fl = s0_count_ges_int / 100.0;                    // Gesamtverbrauch
  sResponse += fl;                                  // Gesamt
  sResponse += F(" m³</text>");

  // SVG Ende
  sResponse += F("</svg></td></tr>");                                           // end svg

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## Diagramm Jahr ##############################
void SiteDiagramm_j() {
  String sResponse = "";                        // Response HTML
  int countargs = server.args();
  String Button = server.arg("Button");
  // erste vorhandene Logdatei Jahr ermitteln
  int FirstYear = year() - 1;
  String FileName = F("/log/y_");
  FileName += (FirstYear);
  FileName += F(".log");
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    SerialPrintLine();            // Trennlinie seriell ausgeben
  }
#if DEBUG_OUTPUT == true
  Serial.print(F("Search first year: "));
  Serial.println(FirstYear);
#endif
  while (SPIFFS.exists(FileName)) {        // Returns true if a file with given path exists, false otherwise.
    FirstYear -= 1;
    FileName = F("/log/y_");
    FileName += (FirstYear);
    FileName += F(".log");
#if DEBUG_OUTPUT == true
    Serial.print(F("Search first year: "));
    Serial.println(FirstYear);
#endif
  }
  FirstYear += 1;
#if DEBUG_OUTPUT == true
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

  // Read Data from SPIFFS
  String Line;
  String mo;                                // String Monat
  int moint = 0;                            // Integer Monat
  int Space2;
  int Space3;
  String s0_count_str;
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
  if (SPIFFS.exists(FileName)) {                // Returns true if a file with given path exists, false otherwise.
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(F("SPIFFS Reading Data from: "));
      Serial.println(FileName);
    }
    File LogFile = SPIFFS.open(FileName, "r");      // Open text file for reading.
    while (LogFile.available()) {
      Line = LogFile.readStringUntil('\n');         // Lets read line by line from the file
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        Serial.println(Line);
      }
      mo = Line.substring(3, 5);                    // Month
      moint = mo.toInt();                           // Integer Monat
      Space2 = Line.indexOf(' ', 19);               // nach Uhrzeit folgt Count absolut
      Space3 = Line.indexOf(' ', Space2 + 1 );      // nach absulut Count folgt Count
      s0_count_str = Line.substring(Space3 + 1);    // liest String bis Zeilenende
      s0_count_int = s0_count_str.toInt();          // Integer
      s0_count_int_arr[moint] = s0_count_int_arr[moint] + s0_count_int;
      s0_count_ges_int = s0_count_ges_int + s0_count_int;   // S0-Counter gesamt
      if (s0_count_int_arr[moint] > s0_count_max_int) {
        s0_count_max_int = s0_count_int_arr[moint];         // S0-Counter Maximum
      }
    }
    LogFile.close();
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(F("Gesamt: "));
      Serial.println(s0_count_ges_int);     // S0-Counter gesamt
    }
  } else {
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(F("SPIFFS Logile not exist: "));
      Serial.println(FileName);
    }
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
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"static/buttonxl.css\">");
  insertMenu_Diagramme(sResponse);                  // Menu Tabelle Zeile 1 einfügen
  insertMenu_Ueberschrift(sResponse);               // Überschrift Tabelle Zeile 2 einfügen

  sResponse += F("<tr><td class=\"CXB\" colspan=\"1\">");
  if (year(DiagrammTimestamp) > FirstYear) {
    sResponse += F("<button type=\"submit\" name=\"Button\" value=\"1\">&#9664;</button>");
    //sResponse += "<button type=\"submit\" name=\"Button\" value=\"1\">&#9668;</button>";
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td>");
  sResponse += F("<td class=\"CXB\" colspan=\"3\">Verbrauch im Jahr ");
  sResponse += (year(DiagrammTimestamp));
  sResponse += F("</td>");
  sResponse += F("<td class=\"CXB\" colspan=\"1\">");
  if (year(DiagrammTimestamp) < year()) {
    sResponse += F("<button type=\"submit\" name=\"Button\" value=\"2\">&#9654;</button>");
    //sResponse += "<button type=\"submit\" name=\"Button\" value=\"2\">&#9658;</button>";
  } else {
    sResponse += F("&nbsp;");
  }
  sResponse += F("</td></tr>");
  sResponse += F("<tr><td class=\"c\" colspan=\"5\">");

  // Beginn Diagramm
  sResponse += F("<svg width=\"735\" height=\"350\">"
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
  sResponse += F("</g>");                       // Ende Group
  // Beschriftung Y-Achse
  sResponse += F("<g text-anchor=\"end\">");
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
  sResponse += F("</g>");                       // Ende Group

  // Beschriftung X-Achse
  sResponse += F("<g text-anchor=\"middle\">");
  for (int i = 0; i < 12; ++ i) {               // 12 Monate
    sResponse += F("<text x=\"");
    sResponse += i * 56 + 78;                   // x-position x-text (50+28)
    sResponse += F("\" y=\"345\">");
    sResponse += i + 1;                         // text x-axis
    sResponse += F("</text>");
  }
  sResponse += F("</g>");                       // Ende Group

  // Säulen zeichnen
  sResponse += F("<g fill=\"#ff0\" stroke-width=\"1\" stroke=\"#000\">");
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
  sResponse += F("</g>");                       // Ende Group

  // Beschriftung oben
  sResponse += F("<text x=\"80\" y=\"20\">Durchschnitt: ");
  float fl = s0_count_avg_int / 1200.0;              // Durchschnitt (12 Monate / 100)
  sResponse += fl;                                   // Durchschnitt
  sResponse += F(" m³/M</text>)"
                 "<text x=\"405\" y=\"20\" text-anchor=\"middle\">Maximum: ");  // Max
  sResponse += s0_count_max_fl;                      // Maximum
  sResponse += F(" m³/M</text>"
                 "<text x=\"690\" y=\"20\" text-anchor=\"end\">Gesamt: ");      // Gesamt
  fl = s0_count_ges_int / 100.0;                    // Gesamtverbrauch
  sResponse += fl;                                  // Gesamt
  sResponse += F(" m³</text>");

  // SVG Ende
  sResponse += F("</svg></td></tr>");                                           // end svg

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## Diagramm Jahre ##############################
void SiteDiagramm_jahre() {
  String sResponse = "";                        // Response HTML
  String Button = server.arg("Button");
  // erste vorhandene Logdatei Jahr ermitteln
  int FirstYear = year() - 1;
  String FileName = F("/log/y_");
  FileName += (FirstYear);
  FileName += F(".log");
  if (SerialOutput == 1) {        // serielle Ausgabe eingeschaltet
    SerialPrintLine();            // Trennlinie seriell ausgeben
  }
#if DEBUG_OUTPUT == true
  Serial.print(F("Search first year: "));
  Serial.println(FirstYear);
#endif
  while (SPIFFS.exists(FileName)) {        // Returns true if a file with given path exists, false otherwise.
    FirstYear -= 1;
    FileName = F("/log/y_");
    FileName += (FirstYear);
    FileName += F(".log");
#if DEBUG_OUTPUT == true
    Serial.print(F("Search first year: "));
    Serial.println(FirstYear);
#endif
  }
  FirstYear += 1;
#if DEBUG_OUTPUT == true
  Serial.print(F("First year: "));
  Serial.println(FirstYear);
#endif
  //  if (countargs == 0) {
  //  DiagrammTimestamp = now();                                // aktuelle Uhrzeit übernehmen
  //  } else {
  //    if (Button == "1") {                                      // Button links
  //      if (year(DiagrammTimestamp) > FirstYear) {
  //        if (month(DiagrammTimestamp) > 6) {
  //          DiagrammTimestamp = DiagrammTimestamp - 31622400;   // - 1 Jahr (366 Tage)
  //        } else {
  //          DiagrammTimestamp = DiagrammTimestamp - 31536000;   // - 1 Jahr (365 Tage)
  //        }
  //      }
  //    }
  //    if (Button == "2") {                                      // Button rechts
  //      if (year(DiagrammTimestamp) < year()) {
  //        if (month(DiagrammTimestamp) < 6) {
  //          DiagrammTimestamp = DiagrammTimestamp + 31622400;   // + 1 Jahr (366 Tage)
  //        } else {
  //          DiagrammTimestamp = DiagrammTimestamp + 31536000;   // + 1 Jahr (365 Tage)
  //        }
  //      }
  //    }
  //  }

  // Read Data from SPIFFS
  String Line;
  int Space2;
  int Space3;
  String s0_count_str;
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
    if (SPIFFS.exists(FileName)) {                      // Returns true if a file with given path exists, false otherwise.
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        Serial.print(F("SPIFFS Reading Data from: "));
        Serial.println(FileName);
      }
      File LogFile = SPIFFS.open(FileName, "r");        // Open text file for reading.
      while (LogFile.available()) {
        Line = LogFile.readStringUntil('\n');           // Lets read line by line from the file
        if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
          Serial.println(Line);
        }
        Space2 = Line.indexOf(' ', 19);                       // nach Uhrzeit folgt Count absolut
        Space3 = Line.indexOf(' ', Space2 + 1 );              // nach absulut Count folgt Count
        s0_count_str = Line.substring(Space3 + 1);            // liest String bis Zeilenende
        s0_count_int = s0_count_str.toInt();                  // Integer S0-Count momentan
        s0_count_int_arr[y - 2000] += s0_count_int;           // S0-Count Monat addieren
        s0_count_ges_int = s0_count_ges_int + s0_count_int;   // S0-Counter gesamt
        if (s0_count_int_arr[y - 2000] > s0_count_max_int) {
          s0_count_max_int = s0_count_int_arr[y - 2000];      // S0-Counter Maximum
        }
      }
      LogFile.close();
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        Serial.print(F("Gesamt: "));
        Serial.print(s0_count_int_arr[y - 2000]); // S0-Counter gesamt
        Serial.print(F(" im Jahr "));
        Serial.println(y);     // Jahreszahl
      }
    } else {
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        Serial.print(F("SPIFFS Logile not exist: "));
        Serial.println(FileName);
      }
    }
  }
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.print(F("Gesamt: "));
    Serial.print(s0_count_ges_int);     // S0-Counter gesamt
    Serial.print(F(" in "));
    Serial.print(CountYear);     // S0-Counter gesamt
    Serial.println(F(" Jahr(en)"));
  }

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
  sResponse += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"static/buttonxl.css\">");
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
  sResponse += F("</g>");                       // Ende Group
  // Beschriftung Y-Achse
  sResponse += F("<g text-anchor=\"end\">");
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
  sResponse += F("</g>");                      // Ende Group
  // Beschriftung X-Achse
  sResponse += F("<g text-anchor=\"middle\">");
  for (int i = 0; i < CountYear; ++ i) {       // Anzahl Jahre
    int xt = i * xDistance + 50 + xDistance / 2; // x-position x-text
    sResponse += F("<text x=\"");
    sResponse += xt;                           // x-position
    sResponse += F("\" y=\"345\">");
    sResponse += FirstYear + i;                // text x-axis
    sResponse += F("</text>");
  }
  sResponse += F("</g>");                       // Ende Group
  // Säulen zeichnen
  sResponse += F("<g fill=\"#ff0\" stroke-width=\"1\" stroke=\"#000\">");
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
  sResponse += F("</g>");                       // Ende Group
  // Beschriftung oben Durchschnitt, Maximum, Gesamt
  sResponse += F("<text x=\"80\" y=\"20\">Durchschnitt: ");   // X-Position Text (Rahmenbreite + 50 + 30)
  float fl = s0_count_avg_int / CountYear / 100.0;            // Durchschnitt (Anzahl Jahre / 100)
  sResponse += fl;                                   // Durchschnitt
  sResponse += F(" m³/a</text>)"
                 "<text x=\"405\" y=\"20\" text-anchor=\"middle\">Maximum: ");  // X-Position Text
  sResponse += s0_count_max_fl;                      // Maximum
  sResponse += F(" m³/a</text>"
                 "<text x=\"690\" y=\"20\" text-anchor=\"end\">Gesamt: ");
  fl = s0_count_ges_int / 100.0;                    // Gesamtverbrauch
  sResponse += fl;                                  // Gesamt
  sResponse += F(" m³</text>");
  // SVG Ende
  sResponse += F("</svg></td></tr>");                                           // end svg
  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

// ############################## index.htm ##############################
void SiteIndex()  {
  String sResponse = "";                        // Response HTML
  String Wochentag[8] = {"", "Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Sonnabend"};

  insertHeaderCSS(sResponse);                   // Header und CCS einfügen
  sResponse += F("<meta http-equiv=\"refresh\" content=\"60\">");
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
                 "<td class=\"lxx\" colspan=\"1\">m³</td></tr>"
                 // Tabelle Zeile 5 einfuegen
                 "<tr><td class=\"CXB\" colspan=\"5\">Verbrauch</td></tr>"
                 "<tr><td class=\"rx\" colspan=\"2\">heute</td>"
                 "<td class=\"rx\" colspan=\"2\">");
  sResponse += longToStr(s0_count_day);               // Zählerstand Tag
  sResponse += F("</td>"
                 "<td class=\"lx\" colspan=\"1\">m³</td></tr>"
                 // Tabelle Zeile 6 einfuegen
                 "<tr><td class=\"rx\" colspan=\"2\">diesen Monat</td>"
                 "<td class=\"rx\" colspan=\"2\">");
  sResponse += longToStr(s0_count_month);             // Zählerstand Monat
  sResponse += F("</td>"
                 "<td class=\"lx\" colspan=\"1\">m³</td></tr>"
                 // Tabelle Zeile 7 einfuegen
                 "<tr><td class=\"rx\" colspan=\"2\">dieses Jahr</td>"
                 "<td class=\"rx\" colspan=\"2\">");
  sResponse += longToStr(s0_count_year);              // Zählerstand Jahr
  sResponse += F("<td class=\"lx\" colspan=\"1\">m³</td></tr>"
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
                 "<td class = \"lx\" colspan=\"1\">m³</td></tr>"
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
                 "<td class = \"lx\" colspan=\"1\">m³</td></tr>");

  // Footer anhängen und senden
  insertFooterSend(sResponse);                    // Footer anhängen und senden
}

void Site404()  {
  String sResponse = F("<html><head><title>404 Not Found</title></head><body>"
                       "<h1>Not Found</h1>"
                       "<p>The requested URL was not found on this webserver.</p>"
                       "</body></html>");
  server.send(404, "text/html", sResponse);
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.print(F("requested URL not found: "));
    Serial.println(server.uri());
    Serial.print(F("HTTP TX Bytes: "));
    Serial.println(sResponse.length());
  }
  ulWifiTxBytes = ulWifiTxBytes + sResponse.length();
}

void insertHeaderCSS(String & str) {                     // Header und CCS einfügen
  digitalWrite(LED_red, LOW) ;     // LED ein
  str += F("<!DOCTYPE html><html><head>"
           "<meta content=\"de\" http-equiv=\"Content-Language\">"
           "<meta content=\"text/html; charset=utf-8\" http-equiv=\"Content-Type\">"
           "<meta name=\"viewport\" content=\"width=device-width, user-scalable=yes\">"
           "<title>Gaszähler ");
  str += OwnStationHostname;
  str += F("</title>"
           "<link rel=\"stylesheet\" type=\"text/css\" href=\"static/style.css\">");
}

void insertMenu_Index(String & str) {                     // Index Menu Tabelle Zeile 1 einfügen
  str += F("</head><body><form>"
           "<table align=\"center\" style=\"width: 802px\">"
           // Menue Tabelle Zeile 1 einfügen
           "<tr><td class=\"CLB\" style=\"width: 20%\"><a href=\"index.htm\">"
           "<img src=\"pic/logo.svg\" height=\"32\" width=\"32\" alt=\"Home\"></a></td>"
           "<td class=\"CLB\" style=\"width: 20%\"><a href=\"diagramm_d.htm\">Diagramme</a></td>"
           "<td class=\"CLB\" style=\"width: 20%\"><a href=\"statistik.htm\">Statistiken</a></td>"
           "<td class=\"CLB\" style=\"width: 20%\"><a href=\"info.htm\">Informationen</a></td>"
           "<td class=\"CLB\" style=\"width: 20%\"><a href=\"setup.htm\">Einstellungen</a></td></tr>");
}

void insertMenu_Setup(String & str) {                     // Setup Menu Tabelle Zeile 1 einfügen
  str += F("</head><body><form method=\"get\">"
           "<table align=\"center\" style=\"width: 802px\">"
           "<tr><td class=\"CLB\" style=\"width: 156px; \"><a href=\"index.htm\">"
           "<img src=\"pic/logo.svg\" height=\"32\" width=\"32\" alt=\"Home\"></a></td>"
           "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"setup.htm\">diverse</a></td>"
           "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"setuptime.htm\">Zeit</a></td>"
           "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"setuplan.htm\">Netzwerk</a></td>"
           "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"setupmqtt.htm\">MQTT</a></td></tr>");
}

void insertMenu_Diagramme(String & str) {                 // Diagramme Menu Tabelle Zeile 1 einfügen
  String FileName = F("/log/y_");
  FileName += (year() - 1);
  FileName += F(".log");
  str += F("</head><body><form method=\"get\">"
           "<table align=\"center\" style=\"width: 802px\">"
           "<tr><td class=\"CLB\" style=\"width: 156px; \"><a href=\"index.htm\">"
           "<img src=\"pic/logo.svg\" height=\"32\" width=\"36\" alt=\"Home\"></a></td>"
           "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"diagramm_d.htm\">Tag</a></td>"
           "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"diagramm_m.htm\">Monat</a></td>"
           "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"diagramm_j.htm\">Jahr</a></td>"
           "<td class=\"CLB\" style=\"width: 156px; \">");
  if (SPIFFS.exists(FileName)) {                // Returns true if a file with given path exists, false otherwise.
    str += F("<a href=\"diagramm_jahre.htm\">Jahre</a>");
  } else {
    str += F("&nbsp;");                                 // Menuepunkt "Jahre" nicht anzeigen
  }
  str += F("</td></tr>");
}

void insertMenu_Log(String & str) {                     // Log Menu Tabelle Zeile 1 einfügen
  str += F("</head><body><form method=\"get\">"
           "<table align=\"center\" style=\"width: 802px\">"
           "<tr><td class=\"CLB\" style=\"width: 156px; \"><a href=\"info.htm\">"
           "<img src=\"pic/logo.svg\" height=\"32\" width=\"32\" alt=\"Info\"></a></td>"
           "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"log_d.htm\">Tag</a></td>"
           "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"log_m.htm\">Monat</a></td>"
           "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"log_j.htm\">Jahr</a></td>"
           "<td class=\"CLB\" style=\"width: 156px; \"><a href=\"log.htm\">System</a></td></tr>");
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
  str += ulReconncount;                                     // WLAN Neuverbindungen
  str += F("</td>"
           "<td class = \"CSBFN\">Uptime: ");
  str += UpTimeToString(uptime);                            // return Uptime String from Timestamp
  str += F("</td>"
           "<td class=\"CSBFN\">&nbsp;</td>"                // leere Zelle
           "<td class=\"CSBFN\">&copy; UB 2016</td></tr>"   // Copyright
           // Ende Tabelle, Form, Body, Html
           "</table></form></body></html>");
  ulWifiTxBytes = ulWifiTxBytes + str.length();
  server.send ( 200, "text/html", str );
  digitalWrite(LED_red, HIGH) ;     // LED aus
  // ESP.getFreeHeap() returns the free heap size.
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.print (F("HTTP TX Bytes: "));
    Serial.print (str.length());
    Serial.print(F(", Free Heap: "));
    Serial.println(formatBytes(ESP.getFreeHeap()).c_str());
  }
}

//void sendLogo() {                            // Logo SVG senden
//  String sResponse = F("<svg version=\"1\" xmlns=\"http://www.w3.org/2000/svg\" "
//                       "width=\"42.667\" height=\"42.667\" viewBox=\"0 0 32 32\" "
//                       "preserveAspectRatio=\"xMidYMid meet\">"
//                       "<path d=\"M13 2.35c0 .7-1.8 4.75-4 8.9-5.45 10.35-5.1 16.5 1.1 19.8 1.95 1.05 1.95 "
//                       "1 .5-1.9-1.25-2.4-1.35-3.65-.45-6.3 1.6-4.75 2.5-5.4 3.9-2.75 1.55 2.85 2.75.8 "
//                       "2.9-4.85.05-4.15 1.35-4.1 3.95.1 2.75 4.45 2.75 10.55.05 14-2.9 3.7-.2 3.55 3.35-.15 "
//                       "5.35-5.55 3.45-14.3-5.05-23.35C14.6.9 13 0 13 2.35z\" "
//                       "fill=\"orange\" stroke=\"none\"/></svg>");
//  ulWifiTxBytes = ulWifiTxBytes + sResponse.length();
//  server.send (200, "image/svg+xml", sResponse);
//  digitalWrite(LED_red, HIGH) ;     // LED aus
//#if DEBUG_OUTPUT == true
//  Serial.print(F("HTTP TX Bytes: "));
//  Serial.print(sResponse.length());
//  Serial.print(F(", Free Heap: "));
//  Serial.println(formatBytes(ESP.getFreeHeap()).c_str());
//#endif
//}

