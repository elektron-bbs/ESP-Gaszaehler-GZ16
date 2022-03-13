unsigned int LittleFSReadS0Count(String FileName, byte zeitraum) {
  SerialPrintLine();            // Trennlinie seriell ausgeben
  Serial.println(F("Read S0-Counts from LittleFS"));
  Serial.print(F("Filename: "));
  Serial.println(FileName);
  String Line = "";
  String da = "";             // Day
  String mo = "";             // Month
  String ye = "";             // Year
  int daint = 0;              // Day
  int moint = 0;              // Month
  int yeint = 0;              // Year
  int Space1 = 0;
  int Space2 = 0;
  int Space3 = 0;
  String s0_count_str = "";
  int s0_count_int = 0;
  //String s0_count_abs_str = "";
  int S0_Count = 0;
  if (LittleFS.exists(FileName)) {                // Returns true if a file with given path exists, false otherwise.
    File LogFile = LittleFS.open(FileName, "r");  // Open text file for reading.
    while (LogFile.available()) {
      Line = LogFile.readStringUntil('\n');     // Lets read line by line from the file
      da = Line.substring(0, 2);                // Day
      mo = Line.substring(3, 5);                // Month
      ye = Line.substring(6, 10);               // Year
      daint = da.toInt();                       // Day
      moint = mo.toInt();                       // Month
      yeint = ye.toInt();                       // Year
      Space1 = Line.indexOf(' ');               // Datum Uhrzeit
      Space2 = Line.indexOf(' ', Space1 + 1 );  // Uhrzeit Count_abs
      Space3 = Line.indexOf(' ', Space2 + 1 );  // Count_abs Count
      //s0_count_abs_str = Line.substring(Space2 + 1, Space3);
      //s0_count_abs = s0_count_abs_str.toInt();
      s0_count_str = Line.substring(Space3 + 1);
      s0_count_int = s0_count_str.toInt();
      if (zeitraum == 1) {                      // Tag
        if (yeint == year() && moint == month() && daint == day()) {
          S0_Count = S0_Count + s0_count_int;
        }
      }
      if (zeitraum == 2) {                      // Monat
        if (yeint == year() && moint == month()) {
          S0_Count = S0_Count + s0_count_int;
        }
      }
      if (zeitraum == 3) {                      // Jahr
        if (yeint == year()) {
          S0_Count = S0_Count + s0_count_int;
        }
      }
      Serial.println(Line);
    }
    LogFile.close();
  }
  Serial.print(F("S0-Pulse count:   "));
  Serial.println(S0_Count);
  //Serial.print(F("S0-Pulse max count:  "));
  //Serial.println(s0_count_abs);
  return S0_Count;
}

void LittleFSWriteS0Count(String FileName, unsigned int S0_Count) {
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.println(F("Write S0-Counts to LittleFS"));
    Serial.print(F("Filename: "));
    Serial.println(FileName);
    Serial.print(F("S0-Count: "));
    Serial.println(S0_Count);
  }
  //File LogFile;
  //  if (year() < 2016) {
  //    LogFile = LittleFS.open(FileName, "w");   // Open for truncate and write
  //  } else {
  //    LogFile = LittleFS.open(FileName, "a");   // Open for appending (writing at end of file).
  //  }
  File LogFile = LittleFS.open(FileName, "a");   // Open for appending (writing at end of file).
  if (!LogFile) {
    String logtext = F("GZ16 Logfile ");
    logtext += (FileName);
    logtext += F(" open failed");
    appendLogFile(logtext);
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.println(logtext);
    }
  } else {
    LogFile.print(DateTimeToString(now()));    // return Date and Time String from Timestamp
    LogFile.print(' ');
    LogFile.print(s0_count_abs);               // save S0-Count absolute
    LogFile.print(' ');
    LogFile.println(S0_Count);                 // save S0-Count
  }
  LogFile.close();
}

/*
  EEPROM - Adressen 128, 256, 512, 1024 und 2048 nach Reset auf 0!
  Beg.  Beg.
  dez   hex       Bytes Anzahl  Ende Summe  Variable            Verwendung                     Typ
  ------------------------------------------------------------------------------------------------------
  0     0000        4     1      3      4   eip                 eigene IP                      IPAddress
  4     0004        4     1      7      4   esgw                Standard Gateway               IPAddress
  8     0008        4     1     11      4   edns                Domain Name Server             IPAddress
  12    000C        4     1     15      4   esnm                Subnetzmaske                   IPAddress
  16    0010       48     1     63     48
  64    0040        2     1     65      2   eMqttPort           MQTT Port                      Integer
  66    0042       30     1     95     30
  96    0060        1     1     96      1   ntpServerNr         NTP Server Nummer              Byte
  97    0061        1     1     97      1   estart              Start Count for WPS            Byte
  98    0062       14     1    111     14
  112   0070        1     1    112      1   DST                 Daylight Saving Time           boolean
  113   0071        1     1    113      1   edhcp               DHCP ein/aus                   boolean
  114   0072        1     1    114      1   MqttConnect         MQTT Connect                   boolean
  115   0073       13     1    127     13
  128   0080       16     1    143     16
  144   0090        4     1    147      4   max S0-Count day    Maximalverbrauch pro Tag       long
  148   0094        4     1    151      4   Timestamp           aktuelle Uhrzeit               long
  152   0098        4     1    155      4   max S0-Count month  Maximalverbrauch pro Monat     long
  156   009C        4     1    159      4   Timestamp           aktuelle Uhrzeit               long
  160   00A0        4     1    163      4   max S0-Count year   Maximalverbrauch pro Jahr      long
  164   00A4        4     1    167      4   Timestamp           aktuelle Uhrzeit               long
  168   00A8       88     1    255     88
  256   0100       16     1    271     16
  272   0110       32     1    303     32   essid               SSID                           String
  304   0130       64     1    367     64   epass               Wifi Passwort                  String
  368   0170       64     1    431     64   ntpServerName       NTP Server Name                String
  432   01B0       64     1    495     64   eMqttBroker         MQTT Broker                    String
  496   01F0       16     1    511     16
  512   0200       16     1    527     16
  528   0210       64     5    847    320   eMqttTopic[5]       MQTT Topic                     String
  848   0350        1     5    852      5   eMqttSubscribe[5]   MQTT Subscribe                 boolean
*/

void eeprom_alldata_read() {                                        // read all data from EEPROM
  SerialPrintLine();            // Trennlinie seriell ausgeben
  Serial.println(F("Read settings from EEPROM:"));
  essid = EEPROM_read_string(EEPROM_ADDR_SSID);                          // read eeprom for ssid
  //essid = "Elektron BBS";
  Serial.print(F("EEPROM SSID: "));
  Serial.println(essid);
  epass = EEPROM_read_string(EEPROM_ADDR_PASS);                   // read eeprom for wifi password
  //epass = "8527934757161181";
  Serial.print(F("EEPROM Password: "));
  Serial.println(epass);
  edhcp = EEPROM.read(EEPROM_ADDR_DHCP);                                     //read DHCP on or off
  if (edhcp > 1) {
    edhcp = 1;                                                                          // DHCP on
    EEPROM_write_boolean(EEPROM_ADDR_DHCP, 1);                         // write boolean at address
  }
  Serial.print(F("EEPROM DHCP on/off: "));
  Serial.println(edhcp);
  eip = EEPROM_read_ipaddress(EEPROM_ADDR_IP);                // read IP-Address  static IP eip[4]
  Serial.print(F("EEPROM IP-Address: "));
  Serial.println(eip);
  edns = EEPROM_read_ipaddress(EEPROM_ADDR_DNS);         // read IPAddress Domain Name Server edns
  Serial.print(F("EEPROM Domain Name Server: "));
  Serial.println(edns);
  esgw = EEPROM_read_ipaddress(EEPROM_ADDR_GATEWAY);       // read IPAddress Standard-Gateway esgw
  Serial.print(F("EEPROM Standard-Gateway: "));
  Serial.println(esgw);
  esnm = EEPROM_read_ipaddress(EEPROM_ADDR_NETMASK);            // read IPAddress SubNetMask  esnm
  Serial.print(F("EEPROM Subnetzmaske: "));
  Serial.println(esnm);
  ntpServerName[2] = EEPROM_read_string(EEPROM_ADDR_NTPSERVERNAME); // read eeprom NTP Server Name
  Serial.print(F("EEPROM NTP Server: "));
  Serial.println(ntpServerName[2]);
  ntpServerNr = EEPROM.read(EEPROM_ADDR_NTPSERVERNUMBER);       // read selected NTP Server Number
  if (ntpServerNr > 3) {
    ntpServerNr = 3;
    EEPROM_write_byte(EEPROM_ADDR_NTPSERVERNUMBER, 3);                    // write byte at address
  }
  Serial.print(F("EEPROM NTP Server select Number: "));
  Serial.println(ntpServerNr);
  Serial.print(F("EEPROM NTP Server selected: "));
  Serial.println(ntpServerName[ntpServerNr]);
  DST = EEPROM.read(EEPROM_ADDR_DST);                                 // read Daylight Saving Time
  if (DST > 1) {
    DST = 0;
    EEPROM_write_boolean(EEPROM_ADDR_DST, 0);                          // write boolean at address
  }
  Serial.print(F("EEPROM DST: "));
  Serial.println(DST);
  eMqttBroker = EEPROM_read_string(EEPROM_ADDR_MQTTBROKER);             // read MQTT Broker from EEPROM
  if (eMqttBroker.length() == 0) {
    MqttConnect = false;
    EEPROM_write_boolean(EEPROM_ADDR_MQTTCONNECT, 0);                   // write boolean at address
  }
  Serial.print(F("EEPROM MQTT-Broker: "));
  Serial.println(eMqttBroker);
  eMqttUsername = EEPROM_read_string(EEPROM_ADDR_MQTTUSERNAME);         // read MQTT Username from EEPROM
  Serial.print(F("EEPROM MQTT Username: "));
  Serial.println(eMqttUsername);
  eMqttPassword = EEPROM_read_string(EEPROM_ADDR_MQTTPASSWORD);         // read MQTT Password from EEPROM
  Serial.print(F("EEPROM MQTT Password: "));
  Serial.println(eMqttPassword);
  eMqttPort = EEPROM_read_int(EEPROM_ADDR_MQTTPORT);                    // read MQTT Port from EEPROM
  if (eMqttPort == 0xFFFF) {
    eMqttPort = 1883;
    EEPROM_write_int(EEPROM_ADDR_MQTTPORT, eMqttPort);                  // write MQTT Server Port to EEPROM
  }
  Serial.print(F("EEPROM MQTT-Port: "));
  Serial.println(eMqttPort);
  MqttConnect = EEPROM.read(EEPROM_ADDR_MQTTCONNECT);                   // read MQTT-Connect from EEPROM
  if (MqttConnect > 1) {
    MqttConnect = false;
    EEPROM_write_boolean(EEPROM_ADDR_MQTTCONNECT, 0);                   // write boolean at address
  }
  Serial.print(F("EEPROM MQTT-Connect: "));
  Serial.println(MqttConnect);

  eMqttPublish_s0_count_abs = EEPROM.read(EEPROM_ADDR_MQTTPUBLISHABS);     // read MQTT publish S0-Counter absolut
  if (eMqttPublish_s0_count_abs > 1) {
    eMqttPublish_s0_count_abs = 0;
    EEPROM_write_boolean(EEPROM_ADDR_MQTTPUBLISHABS, 0);                   // write boolean at address
  }
  Serial.print(F("EEPROM MQTT publish S0-Counter absolut: "));
  Serial.println(eMqttPublish_s0_count_abs);
  eMqttPublish_s0_count_mom = EEPROM.read(EEPROM_ADDR_MQTTPUBLISHMOM);     // read MQTT publish S0-Counter Moment
  if (eMqttPublish_s0_count_mom > 1) {
    eMqttPublish_s0_count_mom = 0;
    EEPROM_write_boolean(EEPROM_ADDR_MQTTPUBLISHMOM, 0);                   // write boolean at address
  }
  Serial.print(F("EEPROM MQTT publish S0-Counter Momentan: "));
  Serial.println(eMqttPublish_s0_count_mom);
  eMqttPublish_rssi = EEPROM.read(EEPROM_ADDR_MQTTPUBLISHRSSI);             // read MQTT publish WLAN RSSI
  if (eMqttPublish_rssi > 1) {
    eMqttPublish_rssi = 0;
    EEPROM_write_boolean(EEPROM_ADDR_MQTTPUBLISHRSSI, 0);                   // write boolean at address
  }
  Serial.print(F("EEPROM MQTT publish WLAN RSSI: "));
  Serial.println(eMqttPublish_rssi);
  eMqttPublish_recon = EEPROM.read(EEPROM_ADDR_MQTTPUBLISHRECON);             // read MQTT publish WLAN Reconnects
  if (eMqttPublish_recon > 1) {
    eMqttPublish_recon = 0;
    EEPROM_write_boolean(EEPROM_ADDR_MQTTPUBLISHRECON, 0);                   // write boolean at address
  }
  Serial.print(F("EEPROM MQTT publish WLAN Reconnects: "));
  Serial.println(eMqttPublish_recon);

  eMqttPublish_Intervall = EEPROM.read(EEPROM_ADDR_MQTTINTERVALL);         // read EEPROM MQTT publish intervall
  if (eMqttPublish_Intervall > 60) {
    eMqttPublish_Intervall = 5;
    EEPROM_write_byte(EEPROM_ADDR_MQTTINTERVALL, 5);                        // write byte at address
  }
  Serial.print(F("EEPROM MQTT publish interval: "));
  Serial.println(eMqttPublish_Intervall);

  SerialOutput = EEPROM.read(EEPROM_ADDR_SERIALOUTPUT);           // read EEPROM serial output on/off
  if (SerialOutput > 1) {
    SerialOutput = 0;                                                       // serial output off
    EEPROM_write_byte(EEPROM_ADDR_SERIALOUTPUT, 0);                        // write byte at address
  }
  Serial.print(F("EEPROM Serial Output: "));
  Serial.println(SerialOutput);

  estart = EEPROM.read(EEPROM_ADDR_START);                                  // read EEPROM Start Counter
  if (estart == 255) {
    estart = 0;
    EEPROM_write_byte(EEPROM_ADDR_START, 0);                                // write byte at address
  }
  Serial.print(F("EEPROM Neustarts: "));
  Serial.println(estart);

  SerialPrintLine();            // Trennlinie seriell ausgeben
  Serial.println(F("Read data from EEPROM:"));

  // S0-Count Start Trip 1 lesen
  unsigned long l = EEPROM_read_long(EEPROM_ADDR_S0TRIP1);                  // read S0-Count Trip begin
  if (l != 0xFFFFFFFF) {                                     // Prüfung ob gelöscht
    Serial.print(F("EEPROM S0-Count start trip 1:   "));
    Serial.print(l);
    Serial.print(F(" - "));
    l = EEPROM_read_long(EEPROM_ADDR_S0TRIP1TIME);            // read timestamp
    Serial.println(DateToString(l));
  } else {
    EEPROM_write_long(EEPROM_ADDR_S0TRIP1, s0_count_abs);     // S0-Count Trip begin set to S0-Count
    EEPROM_write_long(EEPROM_ADDR_S0TRIP1TIME, now());        // S0-Count Trip time set
  }

  // S0-Count Start Trip 2 lesen
  l = EEPROM_read_long(EEPROM_ADDR_S0TRIP2);                  // read S0-Count Trip begin
  if (l != 0xFFFFFFFF) {                                     // Prüfung ob gelöscht
    Serial.print(F("EEPROM S0-Count start trip 2:   "));
    Serial.print(l);
    Serial.print(F(" - "));
    l = EEPROM_read_long(EEPROM_ADDR_S0TRIP2TIME);            // read timestamp
    Serial.println(DateToString(l));
  } else {
    EEPROM_write_long(EEPROM_ADDR_S0TRIP2, s0_count_abs);     // S0-Count Trip begin set to S0-Count
    EEPROM_write_long(EEPROM_ADDR_S0TRIP2TIME, now());        // S0-Count Trip time set
  }
}

void ds1307_alldata_read() {                                        // read all data from DS1307 RAM
  Serial.println(F("Read data from DS1307 RAM:"));
  unsigned long l = DS1307_read_long(DS1307_ADDR_S0COUNTHOUR);   // read S0-Counter hour from DS1307 RAM
  if (l != 0xFFFFFFFF) {                                         // Prüfung ob gelöscht
    s0_count_hour = l;                                            // S0-Counter hour from DS1307 RAM
  } else {
    s0_count_hour = 0;
    DS1307_write_long(DS1307_ADDR_S0COUNTHOUR, 0);               // save S0-Counter hour to DS1307 RAM
  }
  l = DS1307_read_long(DS1307_ADDR_S0COUNTABS);                  // read S0-Counter abs from DS1307 RAM
  if (l != 0xFFFFFFFF) {                                         // Prüfung ob gelöscht
    s0_count_abs = l;                                             // S0-Counter abs from DS1307 RAM
  } else {
    s0_count_abs = 0;
    DS1307_write_long(DS1307_ADDR_S0COUNTABS, 0);                // save S0-Counter abs to DS1307 RAM
  }

  // Maximum Tag lesen
  l = DS1307_read_long(DS1307_ADDR_MAXS0DAY);                    // read max S0-Count day
  if (l != 0xFFFFFFFF) {                                         // Prüfung ob gelöscht
    s0_count_max_day = l;                                       // bisheriges Maximum übernehmen
    Serial.print(F("DS1307 S0-Counts max per day:   "));
    Serial.print(l);
    Serial.print(F(" - "));
    l = DS1307_read_long(DS1307_ADDR_MAXS0DAYTIME);             // read timestamp
    Serial.println(DateToString(l));
  } else {
    s0_count_max_day = 0;
    DS1307_write_long(DS1307_ADDR_MAXS0DAY, 0);                 // Maximum pro Tag löschen
  }

  // Maximum Monat lesen
  l = DS1307_read_long(DS1307_ADDR_MAXS0MONTH);                 // read max S0-Count month
  if (l != 0xFFFFFFFF) {                                         // Prüfung ob gelöscht
    s0_count_max_month = l;                                     // bisheriges Maximum übernehmen
    Serial.print(F("DS1307 S0-Counts max per month: "));
    Serial.print(l);
    Serial.print(F(" - "));
    l = DS1307_read_long(DS1307_ADDR_MAXS0MONTHTIME);           // read timestamp
    Serial.print(month(l));
    Serial.print(F("/"));
    Serial.println(year(l));
  } else {
    s0_count_max_month = 0;
    DS1307_write_long(DS1307_ADDR_MAXS0MONTH, 0);               // Maximum pro Monat löschen
  }

  // Maximum Jahr lesen
  l = DS1307_read_long(DS1307_ADDR_MAXS0YEAR);                  // read max S0-Count year
  if (l != 0xFFFFFFFF) {                                         // Prüfung ob gelöscht
    s0_count_max_year = l;                                      // bisheriges Maximum übernehmen
    Serial.print(F("DS1307 S0-Counts max per year:  "));
    Serial.print(l);
    Serial.print(F(" - "));
    l = DS1307_read_long(DS1307_ADDR_MAXS0YEARTIME);            // read timestamp
    Serial.println(year(l));
  } else {
    s0_count_max_year = 0;   // bisheriges Maximum löschen
    DS1307_write_long(DS1307_ADDR_MAXS0YEAR, 0);                // Maximum pro Jahr löschen
  }
}

long EEPROM_read_long(int address) {
  //Read the 4 bytes from the eeprom memory.
  long byte4 = EEPROM.read(address);
  long byte3 = EEPROM.read(address + 1);
  long byte2 = EEPROM.read(address + 2);
  long byte1 = EEPROM.read(address + 3);
  //Return the recomposed long by using bitshift.
  return (byte4 & 0xFF) + ((byte3 << 8) & 0xFFFF) + ((byte2 << 16) & 0xFFFFFF) + ((byte1 << 24) & 0xFFFFFFFF);
}

//This function will write a 4 byte (32bit) long to the eeprom at
//the specified address to address + 3.
void EEPROM_write_long(int address, long value) {
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.print(F("EEPROM write long at address: "));
    Serial.print(address);
    Serial.print(F(" - "));
    Serial.println(value);
  }
  //Decomposition from a long to 4 bytes by using bitshift.
  //Byte1 = Most significant -> Byte4 = Least significant byte
  byte byte4 = (value & 0xFF);
  byte byte3 = ((value >> 8) & 0xFF);
  byte byte2 = ((value >> 16) & 0xFF);
  byte byte1 = ((value >> 24) & 0xFF);
  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, byte4);
  EEPROM.write(address + 1, byte3);
  EEPROM.write(address + 2, byte2);
  EEPROM.write(address + 3, byte1);
  EEPROM.commit();
}

int EEPROM_read_int(int address) {               // read Integer (2 Bytes) from EEPROM
  byte low, high;
  low = EEPROM.read(address);
  high = EEPROM.read(address + 1);
  return low + ((high << 8) & 0xFF00);
}

void EEPROM_write_int(int address, int value) {   // write Integer (2 Bytes) to EEPROM
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.print(F("EEPROM write integer at address: "));
    Serial.print(address);
    Serial.print(F(" - "));
    Serial.println(value);
  }
  EEPROM.write(address, lowByte(value));
  EEPROM.write(address + 1, highByte(value));
  EEPROM.commit();
}

//This function will write a byte to the eeprom at the specified address
void EEPROM_write_byte(int address, byte value) {     // write byte at address
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.print(F("EEPROM write byte at address: "));
    Serial.print(address);
    Serial.print(F(" - "));
    Serial.println(value);
  }
  EEPROM.write(address, value);
  EEPROM.commit();
}

//This function will write a boolean to the eeprom at the specified address
void EEPROM_write_boolean(int address, boolean value) {     // write boolean at address
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.print(F("EEPROM write boolean at address: "));
    Serial.print(address);
    Serial.print(F(" - "));
    Serial.println(value);
  }
  EEPROM.write(address, value);
  EEPROM.commit();
}

String EEPROM_read_string(int address) {                  // read String from EEPROM (Address)
  String str = "";
  for (int i = address; i < EEPROM_MAX_ADDR; ++i) {
    byte value = EEPROM.read(i);
    if (value == 0 || value == 255) {
      return str;
    }
    str += char(value);
  }
  return str;
}

void EEPROM_write_string(int address, String str) {       // write String to EEPROM
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.print(F("EEPROM write string at address: "));
    Serial.print(address);
    Serial.print(F(" - "));
  }
  for (unsigned int i = 0; i < str.length(); ++i) {
    EEPROM.write(address + i, str[i]);
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(str[i]);
    }
  }
  EEPROM.write(address + str.length(), 0);            // Stringende schreiben
  EEPROM.commit();
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.println();
  }
}

IPAddress EEPROM_read_ipaddress(int address) {   // read IP-Address from EEPROM
  IPAddress ipaddress;
  ipaddress[0] = EEPROM.read(address);
  ipaddress[1] = EEPROM.read(address + 1);
  ipaddress[2] = EEPROM.read(address + 2);
  ipaddress[3] = EEPROM.read(address + 3);
  return ipaddress;
}

//This function will write IPAddress (4 byte) to the eeprom at
//the specified address to address + 3.
void EEPROM_write_ipaddress(int address, IPAddress ip) {
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.print(F("EEPROM write IPAddress at address: "));
    Serial.print(address);
    Serial.print(F(" - "));
    Serial.println(ip);
  }
  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, ip[0]);
  EEPROM.write(address + 1, ip[1]);
  EEPROM.write(address + 2, ip[2]);
  EEPROM.write(address + 3, ip[3]);
  EEPROM.commit();
}

// Speicherbereich auslesen
// gibt einen Dump des Speicherinhalts in tabellarischer Form über den seriellen Port aus.
void eeprom_read_table() {
  SerialPrintLine();            // Trennlinie seriell ausgeben
  Serial.println(F("EEPROM read all bytes"));
  int bytesPerRow = 16;
  int i;
  int j = 0;
  byte b;
  char buf[10];
  Serial.print(F("Addr. "));
  for (i = 0; i < bytesPerRow; i++) {
    sprintf(buf, "%02X ", i);
    Serial.print(buf);
  }
  Serial.println();
  SerialPrintLine();            // Trennlinie seriell ausgeben
  for (i = EEPROM_MIN_ADDR; i < EEPROM_MAX_ADDR; i++) {
    if (j == 0) {
      sprintf(buf, "%04X: ", i);
      Serial.print(buf);
    }
    b = EEPROM.read(i);
    sprintf(buf, "%02X ", b);
    j++;
    if (j == bytesPerRow) {
      j = 0;
      Serial.println(buf);
    } else Serial.print(buf);
  }
  SerialPrintLine();            // Trennlinie seriell ausgeben
}

bool handleFileRead(String path) {
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.println(F("handleFileRead: ") + path);
  }
  if (path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (LittleFS.exists(pathWithGz) || LittleFS.exists(path)) {
    if (LittleFS.exists(pathWithGz))
      path += ".gz";
    File file = LittleFS.open(path, "r");
    //size_t sent = server.streamFile(file, contentType);
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void appendLogFile(String logText) {
  File logFile = LittleFS.open("/log/system.log", "a");  // Open for appending (writing at end of file).
  logFile.print(DateTimeToString(now()));           // return Date and Time String from Timestamp
  logFile.print(' ');
  logFile.println(logText);
  logFile.close();
}

void rotateLog() {
  File logFile = LittleFS.open("/log/system.log", "r");            // Open for reading.
  size_t logFileSize = logFile.size();                       // files size
  logFile.close();
  if (logFileSize > 8192) {
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(F("Size Logfile: "));
      Serial.print(logFileSize);
      Serial.print(F(" B"));
    }
    //calculate lines
    logFile = LittleFS.open("/log/system.log", "r");            // Open for reading.
    int logFileLines = 0;
    while (logFile.available()) {
      //Lets read line by line from the file
      String line = logFile.readStringUntil('\n');
      logFileLines += 1;
    }
    logFile.close();
    int newLogFileLines = logFileLines - 10;
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print(F("  Lines: "));
      Serial.print(logFileLines);
      Serial.print(F("  Start: "));
      Serial.println(newLogFileLines);
    }
    // Create new File
    // this opens the file in read-mode
    File logFileNew = LittleFS.open("/lognew.txt", "r");
    if (!logFileNew) {
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        Serial.println(F("LittleFS new log file doesn't exist yet. Creating it"));
      }
      // open the file in write mode
      File logFileNew = LittleFS.open("/lognew.txt", "w");     // Truncate file to zero length or create text file for writing.
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        if (!logFileNew) {
          Serial.println(F("LittleFS new logfile creation failed"));
        }
      }
      // now write one line in key/value style with  end-of-line characters
      //logFileNew.println("---------- CREATE FILE ----------");
    }
    logFileNew.close();
    logFile = LittleFS.open("/log/system.log", "r");                  // Open for reading.
    logFileNew = LittleFS.open("/lognew.txt", "a");            // Open for appending (writing at end of file).
    logFileLines = 0;
    while (logFile.available()) {
      //Lets read line by line from the file
      String line = logFile.readStringUntil('\n');
      logFileLines += 1;
      if (logFileLines > newLogFileLines) {
        logFileNew.println(line);
      }
    }
    logFile.close();
    logFileNew.close();
    LittleFS.remove("/log/system.log");
    LittleFS.rename("/lognew.txt", "/log/system.log");
    appendLogFile(F("Logfile rotated"));
  }
}

int countLogLines(String FileName) {
  int rows = 0;
  File LogFile = LittleFS.open(FileName, "r"); // Open text file for reading.
  while (LogFile.available()) {
    String line = LogFile.readStringUntil('\n'); // read line by line from the file
    rows += 1;
  }
  LogFile.close();
  return rows;
}
