/*
  Messung Dauer in Millisekunden einer Ausführung
  //vor Beginn einfügen:
  unsigned long start = millis();

  //nach Ende einfügen
  unsigned long dauer = millis() - start;
  Serial.print(F("Dauer: "));
  Serial.print(dauer);
  Serial.println(F(" mSek"));
*/

// Messung Dauer in Millisekunden einer Ausführung
//unsigned long startOperation = millis();   // benötigte Rechenzeit für Operation ermitteln (vor Beginn einfügen)
//TimeOfOperation(startOperation);           // benötigte Rechenzeit für Operation (nach Ende einfügen)
#if DEBUG_OUTPUT == true
void TimeOfOperation(unsigned long startOperation) {
  String logText = F("Time for operation: ");
  logText += millis() - startOperation;
  logText += F(" mSec");
  Serial.println(logText);
  appendLogFile(logText);
}
#endif

void DS1307_setTime() {
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write((byte)0);                    // start at location 0
  Wire.write(bin2bcd(second()));
  Wire.write(bin2bcd(minute()));
  Wire.write(bin2bcd(hour()));
  Wire.write(weekday());
  Wire.write(bin2bcd(day()));
  Wire.write(bin2bcd(month()));
  Wire.write(bin2bcd(year() - 2000));
  Wire.endTransmission();
}

void DS1307_getTime() { // get date and time from DS1307
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADDRESS, 7);
  uint8_t se = bcd2bin(Wire.read() & 0x7F); // Bit 7 (CH) auf 0 setzen
  uint8_t mi = bcd2bin(Wire.read());
  uint8_t ho = bcd2bin(Wire.read());
  Wire.read();                              // weekday
  uint8_t da = bcd2bin(Wire.read());
  uint8_t mo = bcd2bin(Wire.read());
  uint16_t ye = bcd2bin(Wire.read()) + 2000;

  setTime(ho, mi, se, da, mo, ye);
}

uint8_t DS1307_isrunning(void) {
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADDRESS, 1);
  uint8_t ss = Wire.read();
  return !(ss >> 7);
}

long DS1307_read_long(byte address) {
  //Read the 4 bytes from the DS1307 memory.
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 4);
  long byte1 = Wire.read();
  long byte2 = Wire.read();
  long byte3 = Wire.read();
  long byte4 = Wire.read();
  //Return the recomposed long by using bitshift.
  return (byte4 & 0xFF) + ((byte3 << 8) & 0xFFFF) + ((byte2 << 16) & 0xFFFFFF) + ((byte1 << 24) & 0xFFFFFFFF);
}

void DS1307_write_long(byte address, long value) {
#if DEBUG_OUTPUT == true
  Serial.print(F("DS1307 write long at address: "));
  Serial.print(address);
  Serial.print(F(" - "));
  Serial.println(value);
#endif
  //Decomposition from a long to 4 bytes by using bitshift.
  //Byte1 = Most significant -> Byte4 = Least significant byte
  byte byte1 = ((value >> 24) & 0xFF);
  byte byte2 = ((value >> 16) & 0xFF);
  byte byte3 = ((value >> 8) & 0xFF);
  byte byte4 = (value & 0xFF);
  //Write the 4 bytes into the DS1307 memory.
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(address);                      // register address
  Wire.write(byte1);
  Wire.write(byte2);
  Wire.write(byte3);
  Wire.write(byte4);
  Wire.endTransmission();
}

void DS1307_read_table() {
  SerialPrintLine();            // Trennlinie seriell ausgeben
  Serial.println(F("DS1307 read all bytes"));
  int bytesPerRow = 16;
  int i = 0;
  int j = 0;
  byte b = 0;
  char buf[10];
  Serial.print(F("Addr. "));
  for (i = 0; i < bytesPerRow; i++) {
    sprintf(buf, "%02X ", i);
    Serial.print(buf);
  }
  Serial.println();
  SerialPrintLine();            // Trennlinie seriell ausgeben

  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(0x00);             // Address
  Wire.endTransmission();
  for (i = 0; i < 64; i++) {
    if (j == 0) {
      sprintf(buf, "%04X: ", i);
      Serial.print(buf);
    }
    //The Wire library has a buffer of 32 bytes!!!
    Wire.requestFrom(DS1307_ADDRESS, 1); // maximal 32 Byte !!!
    if (Wire.available()) {
      b = Wire.read();
    };
    sprintf(buf, "%02X ", b);
    j++;
    if (j == bytesPerRow) {
      j = 0;
      Serial.println(buf);
    } else Serial.print(buf);
  }
  SerialPrintLine();            // Trennlinie seriell ausgeben
}

void DS1307_clear_ram() {
  Serial.println(F("DS1307 clear RAM"));
  for (int x = 8; x < 64; x++) {
    //The Wire library has a buffer of 32 bytes!!!
    Wire.beginTransmission (DS1307_ADDRESS);
    Wire.write(x);              // Address
    Wire.write (0xFF);
    Wire.endTransmission ();
  }
  // scheibt maximal 32 Byte
  //  Wire.beginTransmission(DS1307_ADDRESS);
  //  Wire.write(0x08);
  //  for (uint8_t pos = 0; pos < 56; ++pos) {
  //    Wire.write(0x02);
  //    delay(1);
  //  }
  //  Wire.endTransmission();
}

////////////////////////////////////////////////////////////////////////////////
// utility code, some of this could be exposed in the DateTime API if needed
uint8_t bcd2bin (uint8_t val) {
  return val - 6 * (val >> 4);
}
uint8_t bin2bcd (uint8_t val) {
  return val + 6 * (val / 10);
}

/********************************************************************************************\
  Time stuff
  \*********************************************************************************************/
//void convcompileDate(char const *cdate, char *buff) {
//    int vmonth, vday, vyear;
//    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
//    sscanf(cdate, "%s %d %d", buff, &vday, &vyear);
//    vmonth = (strstr(month_names, buff)-month_names)/3+1;
//    sprintf(buff, "%d%02d%02dt", vyear, vmonth, vday);
//}

// return Uptime String from Timestamp (t is time in seconds)
String UpTimeToString(unsigned long t) {
  char str[10];
  if (t < 86400) {
    sprintf(str, "%02d:%02d:%02d", hour(t), minute(t), second(t));
  } else if (t < 172800) {
    int d = t / 86400;
    sprintf(str, "%d Tag", d);
  } else {
    int d = t / 86400;
    sprintf(str, "%d Tage", d);
  }
  return str;
}

// return Time String from Timestamp (t is time in seconds)
String TimeToString(unsigned long t) {
  char str[8];
  sprintf(str, "%02d:%02d:%02d", hour(t), minute(t), second(t));
  return str;
}

// return Time String from Timestamp (t is time in seconds)
String TimeToStringHM(unsigned long t) {
  char str[5];
  sprintf(str, "%02d:%02d", hour(t), minute(t));
  return str;
}

// return Date String from Timestamp (t is time in seconds)
String DateToString(unsigned long t) {
  char str[10];
  sprintf(str, "%02d.%02d.%04d", day(t), month(t), year(t));
  return str;
}

// return Anzahl Tage im Monat
int LastDayOfMonth (int monat, int jahr) {
  int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  // Check if it is a leap year, this is confusing business
  // See: https://support.microsoft.com/en-us/kb/214019
  if (jahr % 4  == 0) {
    if (jahr % 100 != 0) {
      daysInMonth[2] = 29;
    }
    else {
      if (jahr % 400 == 0) {
        daysInMonth[2] = 29;
      }
    }
  }
  return daysInMonth[monat];
}

void checkDst() {
  // check the beginn of daylight saving time
  if (weekday() == 1 && month() == 3 && day() >= 25 && day() <= 31 && hour() == 2 && DST == false) {
    //  //setclockto 3 am;
    adjustTime(3600); // Adjust system time by adding the adjustment value
    DST = true;
    EEPROM_write_boolean(EEPROM_ADDR_DST, DST);      // write boolean at address
    appendLogFile("Summertime");
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.println("Switch to summertime!");
    }
  }
  // check the end of daylight saving time
  if (weekday() == 1 && month() == 10 && day() >= 25 && day() <= 31 && hour() == 3 && DST == true) {
    //setclockto 2 am;
    adjustTime(-3600); // Adjust system time by adding the adjustment value
    DST = false;
    EEPROM_write_boolean(EEPROM_ADDR_DST, DST);      // write boolean at address
    appendLogFile("Normaltime");
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.println("It is Normaltime!");
    }
  }
}

boolean setTimeNtp() {
  unsigned long t = getNtpTime();
  if (t != 0) {
    if (DST) {
      t += 3600;                                // add one hour if DST active
    }
    ntpSyncTimeDeviation = now() - t;           // Abweichung Uhrzeit
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print("Systime: ");
      Serial.print(now());
      Serial.println(" Seconds since 01.01.1970");
    }
    setTime(t);                                 // Set Time
    DS1307_setTime();                          // set date and time to DS1307
    lastSetTime = now();
    if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
      Serial.print("NTPtime: ");
      Serial.print(t);
      Serial.println(" Seconds since 01.01.1970");
      Serial.print("Deviation: ");
      Serial.print(ntpSyncTimeDeviation);
      Serial.println(" Seconds");
      Serial.print("Date: ");
      Serial.println(DateToString(now()));        // return Date String from Timestamp
      Serial.print("Day of week: ");
      Serial.println(weekday());                  // Day of the week, Sunday is day 1
      Serial.print("Time: ");
      Serial.println(TimeToString(now()));        // return Time String from Timestamp
    }
    // ntpSyncInterval = 7200;                     // 2 Stunden
    // ntpSyncInterval = 21600;                    // 6 Stunden
    ntpSyncInterval = 86400;                    // 24 Stunden
    String logtext = "NTP time sync OK (";
    logtext += (ntpSyncTimeDeviation);
    logtext += (" Sek)");
    appendLogFile(logtext);
    return 1;
  } else {
    ntpSyncInterval = 3600;                      // 60 Minuten
    appendLogFile("NTP time sync Error");
    return 0;
  }
}

unsigned long getNtpTime() {
  WiFiUDP udp;
  udp.begin(123);
  //String log = F("NTP: NTP sync requested");
  //addLog(LOG_LEVEL_DEBUG_MORE, log);
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.println("NTP: sync requested");
  }

  const int NTP_PACKET_SIZE = 48;         // NTP time is in the first 48 bytes of message
  byte packetBuffer[NTP_PACKET_SIZE];     //buffer to hold incoming & outgoing packets

  IPAddress timeServerIP;
  WiFi.hostByName(ntpServerName[ntpServerNr].c_str(), timeServerIP);

  char host[20];
  sprintf_P(host, PSTR("%u.%u.%u.%u"), timeServerIP[0], timeServerIP[1], timeServerIP[2], timeServerIP[3]);
  //log = F("NTP: NTP send to ");
  //log += host;
  //addLog(LOG_LEVEL_DEBUG_MORE, log);
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.print("NTP: send to ");
    Serial.println(host);
  }
  while (udp.parsePacket() > 0) ;               // discard any previously received packets

  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;                 // LI, Version, Mode
  packetBuffer[1] = 0;                          // Stratum, or type of clock
  packetBuffer[2] = 6;                          // Polling Interval
  packetBuffer[3] = 0xEC;                       // Peer Clock Precision
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  udp.beginPacket(timeServerIP, 123);           //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();

  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      //log = F("NTP: NTP replied!");
      //addLog(LOG_LEVEL_DEBUG_MORE, log);
      if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
        Serial.println("NTP: replied!");
      }
      return secsSince1900 - 2208988800UL + TimeZone * 3600;
    }
  }
  //log = F("NTP: No reply");
  //addLog(LOG_LEVEL_DEBUG_MORE, log);
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.println("NTP: no reply");
  }
  return 0;
}

