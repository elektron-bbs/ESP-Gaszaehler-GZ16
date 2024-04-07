// warning: comparison is always true due to limited range of data type [-Wtype-limits] 12 |   if ((monat >= 0 && monat <= 2) || monat >= 12) {
//String WebsiteStatistikJahreszeiten(String str, byte monat, long s0_count_ges_int, int tage, String ye) {
//String WebsiteStatistikJahreszeiten(String str, int monat, long s0_count_ges_int, int tage, String ye) {
//String WebsiteStatistikJahreszeiten(String str, int monat, String ye) {
String WebsiteStatistikJahreszeiten(String str, int monat, int ye) {
  str += F("<tr><td class=\"rx\" colspan=\"1\">");
  // Jahreszeit und Zeitraum einfügen
  if ((monat >= 0 && monat <= 2) || monat >= 12) {
    str += F("Winter</td><td class=\"rx\" colspan=\"2\">01.12.");
    if (month() == 12) {
      str += year();
    } else {
      str += year() - 1;
    }
  }
  if (monat >= 3 && monat <= 5) {
    str += F("Frühling</td><td class=\"rx\" colspan=\"2\">01.03.");
    str += ye;
  }
  if (monat >= 6 && monat <= 8) {
    str += F("Sommer</td><td class=\"rx\" colspan=\"2\">01.06.");
    str += ye;
  }
  if (monat >= 9 && monat <= 11) {
    str += F("Herbst</td><td class=\"rx\" colspan=\"2\">01.09.");
    str += ye;
  }
  return str;
}

void SerialPrintLine() {
  Serial.println(F("------------------------------------------------------------"));
}

// convert long to String (long/100)
String longToStr(long l) {
  float fl = l / 100.0;         // S0-Counter absolut
  String str = String(fl, 2);
  str.replace(".", ",");        // Punkt durch Komma ersetzen
  return str;
}

//format bytes
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + " B";
  } else if (bytes < (1048576)) {
    return String(bytes / 1024.0) + " kB";
  } else if (bytes < (1073741824)) {
    return String(bytes / 1048576.0) + " MB";
  } else {
    return String(bytes / 1073741824.0) + " GB";
  }
}

//format frequenz
String formatHertz(size_t hertz) {
  if (hertz < 1000) {
    return String(hertz) + " Hz";
  } else if (hertz < (1000000)) {
    return String(hertz / 1000.0) + " kHz";
  } else if (hertz < (1000000000)) {
    return String(hertz / 1000000.0) + " MHz";
  } else {
    return String(hertz / 1000000000.0) + " GHz";
  }
}

// Convert a char string to IP byte array
boolean str2ip(char *string, byte* IP)
{
  byte c;
  byte part = 0;
  int value = 0;

  for (unsigned int x = 0; x <= strlen(string); x++)
  {
    c = string[x];
    if (isdigit(c))
    {
      value *= 10;
      value += c - '0';
    }

    else if (c == '.' || c == 0) // next octet from IP address
    {
      if (value <= 255)
        IP[part++] = value;
      else
        return false;
      value = 0;
    }
    else if (c == ' ') // ignore these
      ;
    else // invalid token
      return false;
  }
  if (part == 4) // correct number of octets
    return true;
  return false;
}

/**
   Get ESP8266 infomation.
   @Author  AppStack.CC
   @Website http://www.appstack.cc
*/

// Thank K.Settakan Suwannawat for explain flash chip id. https://www.facebook.com/groups/ChiangMaiMakerClub/permalink/781221611997934/
// Find flash chip from http://code.coreboot.org/svn/flashrom/trunk/flashchips.h

void print_info() {
#ifdef DEBUG_OUTPUT_SERIAL_ESP8266
  SerialPrintLine();            // Trennlinie seriell ausgeben
  Serial.println(F("Hardware-Info ESP8266:"));
  //ESP.getChipId() returns the ESP8266 chip ID as a 32-bit integer.
  Serial.print(F("ESP8266 Chip ID: "));
  Serial.println(ESP.getChipId(), HEX);
  //ESP.getFlashChipId() returns the flash chip ID as a 32-bit integer.
  Serial.print(F("Flash Chip ID: "));
  Serial.println(ESP.getFlashChipId(), HEX);
  //ESP.getFlashChipSize() returns the flash chip size, in bytes, as seen by the SDK (may be less than actual size).
  Serial.print(F("Flash Chip SDK Size: "));
  uint32_t ideSize = ESP.getFlashChipSize();
  Serial.println(formatBytes(ideSize).c_str());
  //ESP.getFlashChipRealSize() returns the flash chip size, in bytes, as seen than actual size).
  Serial.print(F("Flash Chip Real Size: "));
  uint32_t realSize = ESP.getFlashChipRealSize();
  Serial.println(formatBytes(realSize).c_str());
  if (ideSize != realSize) {
    Serial.println(F("Flash Chip configuration wrong!"));
  } else {
    Serial.println(F("Flash Chip configuration ok."));
  }
  //ESP.getFlashChipSpeed(void) returns the flash chip frequency, in Hz.
  Serial.print(F("Flash Chip Speed: "));
  Serial.println(formatHertz(ESP.getFlashChipSpeed()).c_str());
  FlashMode_t ideMode = ESP.getFlashChipMode();
  Serial.printf("Flash ide mode: %s\r\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
  //ESP.getCycleCount() returns the cpu instruction cycle count since start as an unsigned 32-bit.
  //This is useful for accurate timing of very short actions like bit banging.
  Serial.print(F("Cycle Count: "));
  Serial.println(ESP.getCycleCount());

#ifdef HW_VERSION_1                                 // Hardware Version 1 mit ESP-12E
  float voltage = ESP.getVcc() / 1000.0;
  Serial.print(F("Supply Voltage: "));
#else                                               // Hardware Version 2 mit NodeMCU
  float voltage = analogRead(A0) / ADC_DIV;
  Serial.print(F("Battery Voltage: "));
#endif
  Serial.print(voltage);
  Serial.println(F(" Volt"));

  SerialPrintLine();            // Trennlinie seriell ausgeben
  Serial.println(F("Firmware-Info ESP8266:"));

  Serial.print(F("Sketch-File: "));
  Serial.println(compile_file);
  Serial.print(F("Compile: "));
  Serial.print(compile_date);
  Serial.print(F(", "));
  Serial.println(compile_time);
  Serial.print(F("Version: "));
  Serial.print(VERSION);
  Serial.print(F("."));
  Serial.println(BUILD);

  // Retrieve the string representation of the SDK being used.
  Serial.print(F("SDK version: "));
  Serial.println(ESP.getSdkVersion());
  //ESP.getSketchSize(void) returns the Size of Sketch
  Serial.print(F("Sketch size: "));
  Serial.println(formatBytes(ESP.getSketchSize()).c_str());
  //ESP.getFreeSketchSpace(void) returns the free Size for Sketch
  Serial.print(F("Sketch Free size: "));
  Serial.println(formatBytes(ESP.getFreeSketchSpace()).c_str());
  // ESP.getFreeHeap() returns the free heap size.
  Serial.print(F("Free Heap: "));
  Serial.println(formatBytes(ESP.getFreeHeap()).c_str());
#endif

#ifdef DEBUG_OUTPUT_SERIAL_LITTLEFS
  SerialPrintLine();            // Trennlinie seriell ausgeben
  Serial.println(F("Filesystem ESP8266:"));

  //LittleFS
  FSInfo fs_info;
  LittleFS.info(fs_info);
  Serial.print(F("LittleFS total size: "));
  Serial.print(formatBytes(fs_info.totalBytes));
  Serial.print(F(" ("));
  Serial.print(fs_info.totalBytes);
  Serial.println(F(" Bytes)"));
  
  Serial.print(F("LittleFS used bytes: "));
  Serial.print(formatBytes(fs_info.usedBytes));
  Serial.print(F(" ("));
  Serial.print(fs_info.usedBytes);
  Serial.println(F(" Bytes)"));

  Serial.print(F("LittleFS block size: "));
  Serial.print(formatBytes(fs_info.blockSize));
  Serial.print(F(" ("));
  Serial.print(fs_info.blockSize);
  Serial.println(F(" Bytes)"));
  
  Serial.print(F("LittleFS page size: "));
  Serial.print(formatBytes(fs_info.pageSize));
  Serial.print(F(" ("));
  Serial.print(fs_info.pageSize);
  Serial.println(F(" Bytes)"));

  Serial.println(F("Filelist:"));
  File dir = LittleFS.open("/", "r"); // Open dir folder
  printDirectory(dir, 0); // Cycle all the content
#endif
}
