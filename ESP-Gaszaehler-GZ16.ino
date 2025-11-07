#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <EEPROM.h>
#include <LittleFS.h>
#include <TimeLib.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SPI.h>

#define HW_VERSION_1                // Version 1 (UB): ESP-12E, auskommentieren für Version 2 (MR) mit NodeMCU

#ifdef HW_VERSION_1
ADC_MODE(ADC_VCC);                  // vcc read
#else
const float ADC_DIV = 190.0;        // Divisor für Batteriespannung bei HW-Version 2 (MR)
#endif

#define HOSTNAME                    "GZ16-ESP-" // Hostname, 3 Byte Chip-ID werden angehangen
#define VERSION                     1
#define BUILD                       98
//#define DEBUG_OUTPUT_SERIAL
//#define DEBUG_OUTPUT_SERIAL_DS1307
//#define DEBUG_OUTPUT_SERIAL_EEPROM
//#define DEBUG_OUTPUT_SERIAL_ESP8266
//#define DEBUG_OUTPUT_SERIAL_MQTT
//#define DEBUG_OUTPUT_SERIAL_LITTLEFS
//#define DEBUG_OUTPUT_SERIAL_WIFI

#ifdef DEBUG_OUTPUT_SERIAL_WIFI
const char* strWifiStatus[8] PROGMEM = { "WL_IDLE_STATUS", "WL_NO_SSID_AVAIL", "unknown", "WL_CONNECTED", "WL_CONNECT_FAILED", "unknown", "WL_CONNECT_WRONG_PASSWORD", "WL_DISCONNECTED" };
#endif

// EEPROM Size
#define EEPROM_MIN_ADDR             0
#define EEPROM_MAX_ADDR             1024
// EEPROM Settings
// Strings
#define EEPROM_ADDR_SSID            272
#define EEPROM_ADDR_PASS            304
#define EEPROM_ADDR_NTPSERVERNAME   368
#define EEPROM_ADDR_MQTTBROKER      432
#define EEPROM_ADDR_MQTTUSERNAME    528
#define EEPROM_ADDR_MQTTPASSWORD    592
// IPAddress (4 Bytes)
#define EEPROM_ADDR_IP              0
#define EEPROM_ADDR_GATEWAY         4
#define EEPROM_ADDR_DNS             8
#define EEPROM_ADDR_NETMASK         12
// int (2 Bytes)
#define EEPROM_ADDR_MQTTPORT        64
// Bytes
#define EEPROM_ADDR_NTPSERVERNUMBER  96
#define EEPROM_ADDR_START            97
#define EEPROM_ADDR_MQTTINTERVALL    98
// Boolean
#define EEPROM_ADDR_DST              112
#define EEPROM_ADDR_DHCP             113
#define EEPROM_ADDR_MQTTCONNECT      114
#define EEPROM_ADDR_MQTTPUBLISHABS   115
#define EEPROM_ADDR_MQTTPUBLISHMOM   116
#define EEPROM_ADDR_MQTTPUBLISHRSSI  117
#define EEPROM_ADDR_MQTTPUBLISHRECON 118
#define EEPROM_ADDR_SERIALOUTPUT     119

// EEPROM Data
// long (4 Bytes)
#define EEPROM_ADDR_S0TRIP1          168
#define EEPROM_ADDR_S0TRIP1TIME      172
#define EEPROM_ADDR_S0TRIP2          176
#define EEPROM_ADDR_S0TRIP2TIME      180

const char compile_date[] = __DATE__;
const char compile_time[] = __TIME__;
const char compile_file[] = __FILE__;

//Pins for output
#define LED_red     16          // Pin connected to LED red
#define LED_yellow  14          // Pin connected to LED yellow
#define LED_green   12          // Pin connected to LED green

//Pins for input, GPIO15 geht nicht
#define Input_Button      2     // Pin connected to WPS-Button
#define Input_S0_Count    13    // Pin connected to S0 Input

// Button
volatile int Button_Press_Count;              // count button is pressed
volatile int Button_Press_Time;               // time for pressing button
volatile int Button_Input_State = 1;          // Holds the current input state.
volatile long Button_last_debounce_time = 0;  // Holds the last time debounce was evaluated (in millis).
#define Button_debounce_delay   30            // The delay threshold for debounce checking (in millis).

// RTC DS1307
#define I2C_SDA_Pin                 5
#define I2C_SCL_Pin                 4
#define DS1307_ADDRESS              0x68  // 7 Bit
#define DS1307_ADDR_S0COUNTABS      0x08
#define DS1307_ADDR_S0COUNTHOUR     0x0C
#define DS1307_ADDR_MAXS0DAY        0x10
#define DS1307_ADDR_MAXS0DAYTIME    0x14
#define DS1307_ADDR_MAXS0MONTH      0x18
#define DS1307_ADDR_MAXS0MONTHTIME  0x1C
#define DS1307_ADDR_MAXS0YEAR       0x20
#define DS1307_ADDR_MAXS0YEARTIME   0x24

ESP8266WebServer server (80);
ESP8266HTTPUpdateServer httpUpdater;
WiFiClient espClient;

// Netzwerk
String essid = "";                        // Wifi SSID
String epass = "";                        // Wifi Password
//boolean edhcp = true;                     // IP-Adresse mittels DHCP
byte edhcp = true;                        // IP-Adresse mittels DHCP
IPAddress eip;                            // IP-Adresse static IP
IPAddress esnm;                           // Subnetzmaske static
IPAddress esgw;                           // Standard-Gateway static
IPAddress edns;                           // Domain Name Server static
boolean ConnectWifi = false;
String OwnStationHostname = HOSTNAME;
int reconnCount;                          // Counter Reconnects
int reconnCountOld;                       // Counter Reconnects
unsigned long ulWifiRxBytes;              // Count Received Bytes
unsigned long ulWifiTxBytes;              // Count Transmitted Bytes
byte webtype = 255;
byte estart;                              // Start-Zähler für WPS, AP etc.
bool wpsSuccess = false;
int8_t WifiNetworks;                      // Anzahl Wifi-Netzwerke

// MQTT
PubSubClient client(espClient);
String eMqttBroker = "";                      // MQTT-Broker
String eMqttUsername = "";                    // MQTT-Username
String eMqttPassword = "";                    // MQTT-Password
int eMqttPort = 0;                            // MQTT-Port
byte eMqttPublish_Intervall = 3;              // MQTT publish intervall
byte eMqttPublish_s0_count_abs = false;       // MQTT publish S0-Counter absolut
byte eMqttPublish_s0_count_mom = false;       // MQTT publish S0-Counter Moment
byte eMqttPublish_rssi = false;               // MQTT publish WLAN RSSI
byte eMqttPublish_recon = false;              // MQTT publish WLAN reconnects
byte MqttConnect = false;                     // MQTT connect on/off
boolean sendMQTT = false;

// Zeit
const int8_t TimeZone = 1;                    // Europe/Berlin
String ntpServerName[3] = {"pool.ntp.org", "de.pool.ntp.org", ""};
byte ntpServerNr = 0;
//boolean DST = true; // warning: comparison of constant '1' with boolean expression is always false
byte DST = true; // warning: comparison of constant '1' with boolean expression is always false
unsigned long ntpSyncInterval = 0;            // time sync will be attempted after this many seconds
unsigned long ntpSyncSecCount = 0;            // Counter Seconds since last NTP sync
long ntpSyncTimeDeviation = 0;                // Abweichung Zeit intern zu NTP
unsigned long lastSetTime;
int sec_old;
unsigned long uptime;
int numberLogLinesDay = 24;

// Gas counter
volatile long s0_count_abs;                  // S0-Counter absolut
volatile long s0_count_mqtt;                 // S0-Counter Moment for MQTT
volatile long s0_count_hour;                 // S0-Counter hour
volatile long s0_count_day;                  // S0-Counter day
volatile long s0_count_month;                // S0-Counter month
volatile long s0_count_year;                 // S0-Counter year
volatile boolean S0_count_changed = false;   // S0-Counts changed
volatile long S0_last_debounce_time = 0;     // last time debounce was evaluated (in millis).
volatile int S0_Input_State = 1;             // Holds the current input state.
#define S0_debounce_delay   300              // The delay threshold for debounce checking (in millis).
long s0_count_max_day;                       // Maximum pro Tag
long s0_count_max_month;                     // Maximum pro Monat
long s0_count_max_year;                      // Maximum pro Jahr

// Log
boolean saveLogYear = false;
boolean saveLogMonth = false;
boolean saveLogDayBool = false;

// Misc
//boolean SerialOutput;   // serielle Ausgabe ein/aus
byte SerialOutput;   // serielle Ausgabe ein/aus
long DiagrammTimestamp;

void IRAM_ATTR Interrupt_Button() {
  int reading = digitalRead(Input_Button);      // Get the pin reading.
  if (reading == Button_Input_State) return;    // Ignore dupe readings.
  boolean debounce = false;
  // Check to see if the change is within a debounce delay threshold.
  if ((millis() - Button_last_debounce_time) <= Button_debounce_delay) {
    debounce = true;
  }
  // This update to the last debounce check is necessary regardless of debounce state.
  Button_last_debounce_time = millis();
  if (debounce) return;                   // Ignore reads within a debounce delay threshold.
  Button_Input_State = reading;           // All is good, persist the reading as the state.
  if (Button_Input_State == 0) {          // Botton pressed
    Button_Press_Time = 0;
    Button_Press_Count++;                 // increase button press count
    digitalWrite(LED_red, LOW) ;          // LED ein
#ifdef DEBUG_OUTPUT_SERIAL
    Serial.print(F("Button pressed #"));
    Serial.println(Button_Press_Count);
#endif
  }
}

void IRAM_ATTR Interrupt_S0() {
  int reading = digitalRead(Input_S0_Count);  // Get the pin reading.
  if (reading == S0_Input_State) return;      // Ignore dupe readings.
  boolean debounce = false;
  // Check to see if the change is within a debounce delay threshold.
  if ((millis() - S0_last_debounce_time) <= S0_debounce_delay) {
    debounce = true;
  }
  // This update to the last debounce check is necessary regardless of debounce state.
  S0_last_debounce_time = millis();
  if (debounce) return;             // Ignore reads within a debounce delay threshold.
  S0_Input_State = reading;         // All is good, persist the reading as the state.
  if (S0_Input_State == 0) {        // S0-Pulse finished
    s0_count_abs++;                 // increase S0-Counter
    s0_count_mqtt++;                // increase S0-Counter MQTT
    s0_count_hour++;                // increase S0-Counter hour
    s0_count_day++;                 // increase S0-Counter day
    s0_count_month++;               // increase S0-Counter month
    s0_count_year++;                // increase S0-Counter year
    S0_count_changed = true;        // S0-Counts changed
  }
}

void setup (void) {
  // set pins to output
  pinMode(LED_yellow, OUTPUT);
  pinMode(LED_green, OUTPUT);
  pinMode(LED_red, OUTPUT);
  delay(1000);                      // LEDs 1 Sek ein
  digitalWrite(LED_yellow, HIGH) ;  // LED aus
  digitalWrite(LED_green, HIGH) ;   // LED aus
  digitalWrite(LED_red, HIGH) ;     // LED aus

  // set pins to input
  pinMode(Input_Button, INPUT);     // Eingang Taster
  attachInterrupt(Input_Button, Interrupt_Button, CHANGE);
  pinMode(Input_S0_Count, INPUT);   // Eingang S0-Count
  attachInterrupt(Input_S0_Count, Interrupt_S0, CHANGE);

  // start serial
  Serial.begin(115200);

  // start I2C
  Wire.begin(I2C_SDA_Pin, I2C_SCL_Pin);
#if defined DEBUG_OUTPUT_SERIAL_DS1307
  SerialPrintLine();            // Trennlinie seriell ausgeben
#endif
  if (! DS1307_isrunning()) {
#if defined DEBUG_OUTPUT_SERIAL_DS1307
    Serial.println(F("RTC is NOT running! Please set date and time."));
#endif
    digitalWrite(LED_red, LOW) ;     // LED ein
    DS1307_clear_ram();
    //Serial.println("Set date and time to compile time");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(__DATE__, __TIME__));
    //rtc.adjust(DateTime("Feb 28 2017","23:59:45"));
  } else {
    DS1307_getTime();                          // get date and time from DS1307
    lastSetTime = now();
#if defined DEBUG_OUTPUT_SERIAL_DS1307
    Serial.println(F("RTC read date and time:"));
    Serial.print(F("Date: "));
    Serial.println(DateToString(now()));        // return Date String from Timestamp
    Serial.print(F("Day of week: "));
    Serial.println(weekday());                  // Day of the week, Sunday is day 1
    Serial.print(F("Time: "));
    Serial.println(TimeToString(now()));        // return Time String from Timestamp
#endif
  }

  // Initialize file system.
#ifdef DEBUG_OUTPUT_SERIAL_LITTLEFS
  SerialPrintLine();            // Trennlinie seriell ausgeben
#endif
  LittleFS.setTimeCallback(LittleFsTimeCallback); // for LittleFS filesystems file timestamp
  if (!LittleFS.begin()) {
    //filesystem = false;
#ifdef DEBUG_OUTPUT_SERIAL_LITTLEFS
    Serial.println(F("LittleFS Failed to mount file system"));
#endif
  } else {
    //filesystem = true;
    // always use this to "mount" the filesystem
    //bool result = LittleFS.begin();
#ifdef DEBUG_OUTPUT_SERIAL_LITTLEFS
    Serial.println(F("LittleFS Filesystem mounted"));
#endif
    File logFile = LittleFS.open("/log/system.log", "r");     //Open text file for reading.
    if (!logFile) {
#ifdef DEBUG_OUTPUT_SERIAL_LITTLEFS
      Serial.println(F("LittleFS file /log/system.log doesn't exist yet. Creating it"));
#endif
      File logFile = LittleFS.open("/log/system.log", "w");   // Truncate file to zero length or create text file for writing.
      if (!logFile) {
#ifdef DEBUG_OUTPUT_SERIAL_LITTLEFS
        Serial.println(F("LittleFS file creation failed"));
#endif
      } else {
        logFile.println(F("---------- CREATE LOG FILE ----------"));
      }
    }
    logFile.close();
  }

  // Get ESP8266 and FS infomation
  print_info();

  // disconnect wifi
  WiFi.disconnect();
  delay(100);

  // start EEPROM
  EEPROM.begin(EEPROM_MAX_ADDR);                // max 4096 Bytes
  eeprom_alldata_read();                         // read all data from EEPROM

  String logText = F("GZ16 ESP8266 Restart (");
  logText += ESP.getResetReason();
  logText += F(")");
  if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
    Serial.println(logText);
  }
  appendLogFile(logText);

  // Read S0-Data from LittleFS
  String FileName = (F("/log/d_"));                    // lese aktuellen Tag
  FileName += (day());
  FileName += (F(".log"));     // Dateiende ".log" anhängen
  s0_count_day = LittleFSReadS0Count(FileName, 1);
  FileName = (F("/log/m_"));                           // lese aktuellen Monat
  FileName += (month());
  FileName += (F(".log"));     // Dateiende ".log" anhängen
  s0_count_month = LittleFSReadS0Count(FileName, 2);
  FileName = (F("/log/y_"));                           // lese aktuelles Jahr
  FileName += (year());
  FileName += (F(".log"));     // Dateiende ".log" anhängen
  s0_count_year = LittleFSReadS0Count(FileName, 3);

  // Read S0-Data from DS1307
  ds1307_alldata_read();                         // read all data from DS1307 RAM
  s0_count_day += s0_count_hour;
  s0_count_month += s0_count_day;
  s0_count_year += s0_count_month;
#if defined DEBUG_OUTPUT_SERIAL_DS1307
  SerialPrintLine();            // Trennlinie seriell ausgeben
  Serial.print(F("S0-Pulse hour count:     "));
  Serial.println(s0_count_hour);
  Serial.print(F("S0-Pulse day count:      "));
  Serial.println(s0_count_day);
  Serial.print(F("S0-Pulse month count:    "));
  Serial.println(s0_count_month);
  Serial.print(F("S0-Pulse year count:     "));
  Serial.println(s0_count_year);
  Serial.print(F("S0-Pulse absolut count:  "));
  Serial.println(s0_count_abs);
#endif

  // Hostnamen bilden
  char buf[10];
#if defined DEBUG_OUTPUT_SERIAL_WIFI
  SerialPrintLine();            // Trennlinie seriell ausgeben
#endif
  sprintf(buf, "%06X", ESP.getChipId());
  OwnStationHostname += buf;
  OwnStationHostname.replace("_", "-");       // Unterstrich ersetzen, nicht zulässig im Hostnamen
  WiFi.hostname(OwnStationHostname);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
  Serial.print(F("Hostname: "));
  Serial.println(OwnStationHostname);
#endif

  if (estart > 3) {                                         // max. 4 Versuche WPS
    // 3 x Fehler WPS - starte Accesspoint
    setupAP();                                              // Accesspoint starten
  } else {
    if ((essid.length() != 0) && (epass.length() != 0)) {   // SSID und Passwort im EEPROM
      startStation();                                     //connect to Wifi in Station Mode
    } else {                                      // keine SSID und keine Passwort im EEPROM
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
      Serial.println(F("No SSID or Password in EEPROM"));
#endif
      // starte WiFi Protected Setup (WPS)
      if (!start_WPS_connect()) {                         // Wifi WPS Connection failed
        estart += 1;                                       // Start-Zähler erhöhen
        logText = F("Wifi WPS connection failed #");
        logText += estart;
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
        Serial.println(logText);
#endif
        appendLogFile(logText);
        EEPROM_write_byte(EEPROM_ADDR_START, estart);      // write Start Count to EEPROM
        delay(1000);
        /* After programming your ESP, the ESP.restart(); does not always work.
           You have to reset it manually, after programming.
           Because after it reliable Works
        */
        //ESP.reset();
        ESP.restart();
      }
    }
  }
}

void loop ( void ) {
  server.handleClient();  // Webserver
  client.loop();          // MQTT

  if (S0_count_changed == true) {    // S0-Counts changed
    S0_count_changed = false;
    digitalWrite(LED_red, LOW) ;     // LED ein
    if (SerialOutput == 1) {         // serielle Ausgabe eingeschaltet
      Serial.print(F("S0-Pulse detected "));
      Serial.print(TimeToString(now()));      // return Time String from Timestamp
      Serial.print(F(" Nr.: "));
      Serial.println(s0_count_abs);
    }
    DS1307_write_long(DS1307_ADDR_S0COUNTABS, s0_count_abs);     // save S0-Counter abs to DS1307 RAM
    DS1307_write_long(DS1307_ADDR_S0COUNTHOUR, s0_count_hour);   // save S0-Counter hour to DS1307 RAM
    // Maximalwerte in DS1307 RAM speichern
    if (s0_count_day > s0_count_max_day) {
      s0_count_max_day = s0_count_day;                                // Maximum pro Tag übernehmen
      DS1307_write_long(DS1307_ADDR_MAXS0DAY, s0_count_max_day);      // Maximum pro Tag speichern
      DS1307_write_long(DS1307_ADDR_MAXS0DAYTIME, now());             // aktuelle Uhrzeit speichern
    }
    if (s0_count_month > s0_count_max_month) {
      s0_count_max_month = s0_count_month;                            // Maximum pro Monat übernehmen
      DS1307_write_long(DS1307_ADDR_MAXS0MONTH, s0_count_max_month);  // Maximum Monat speichern
      DS1307_write_long(DS1307_ADDR_MAXS0MONTHTIME, now());           // aktuelle Uhrzeit speichern
    }
    if (s0_count_year > s0_count_max_year) {
      s0_count_max_year = s0_count_year;                              // Maximum pro Jahr übernehmen
      DS1307_write_long(DS1307_ADDR_MAXS0YEAR, s0_count_max_year);    // Maximum Jahr speichern
      DS1307_write_long(DS1307_ADDR_MAXS0YEARTIME, now());            // aktuelle Uhrzeit speichern
    }
    digitalWrite(LED_red, HIGH) ;             // LED aus
  }

  // Perform regular checks, 1 time/sec
  if (second() != sec_old) {
    sec_old = second();
    /*
        if (sec_old % 5 == 0) {
          // print RAM-Info, see https://arduino-esp8266.readthedocs.io/en/latest/libraries.html#esp-specific-apis
          Serial.print(F("ESP8266 FreeHeap:          "));
          Serial.println(ESP.getFreeHeap());         // free heap size.
          Serial.print(F("ESP8266 MaxFreeBlockSize:  "));
          Serial.println(ESP.getMaxFreeBlockSize()); // largest contiguous free RAM block in the heap, useful for checking heap fragmentation. NOTE: Maximum malloc() -able block will be smaller due to memory manager overheads.
          Serial.print(F("ESP8266 HeapFragmentation: "));
          Serial.println(ESP.getHeapFragmentation()); // fragmentation metric (0% is clean, more than ~50% is not harmless)
          // END print RAM-Info
        }
    */
    if (Button_Press_Count > 0) {
      Button_Press_Time++;                            // increase timer
      digitalWrite(LED_red, !digitalRead(LED_red));   // LED toggle
      if (Button_Press_Time > 5) {
        String logText = F("Button pressed ");
        logText += Button_Press_Count;
        logText += F(" times, ");
        switch (Button_Press_Count) {
          case 3:   // Taster 3x betätigt - WPS starten
            estart = 0;                                 // Zähler zurück setzen
            EEPROM_write_byte(EEPROM_ADDR_START, estart);      // write Start Count to EEPROM
            logText += F("Wifi WPS start.");
            appendLogFile(logText);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
            Serial.println(logText);
#endif
            // starte WiFi Protected Setup (WPS)
            if (!start_WPS_connect()) {               // Wifi WPS Connection failed
              logText = F("Wifi WPS connection failed #");
              logText += estart;
              appendLogFile(logText);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
              Serial.println(logText);
#endif
              delay(1000);
              /* After programming your ESP, the ESP.restart(); does not always work.
                 You have to reset it manually, after programming. Because after it reliable Works */
              ESP.restart();
            }
            break;
          case 4:   // Taster 4x betätigt - Accesspoint starten
            logText += F("Wifi AP start.");
            appendLogFile(logText);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
            Serial.println(logText);
#endif
            WiFi.disconnect();                // disconnect wifi
            delay(100);
            setupAP();                        // Accesspoint starten
            break;
          case 5:   // Taster 5x betätigt - GZ16 neu starten
            logText += F("GZ16 restart.");
            appendLogFile(logText);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
            Serial.println(logText);
#endif
            WiFi.disconnect();                // disconnect wifi
            delay(100);
            ESP.restart();
            break;
        }
        Button_Press_Count = 0;
        Button_Press_Time = 0;
        digitalWrite(LED_red, HIGH) ;             // LED aus
      }
    }
    checkDst();                 // check and switch Daylight Saving Time
    uptime += 1;
    if (webtype == 0) {                             // Station
      // check if WLAN is connected
      if (WiFi.status() != WL_CONNECTED) {          // Wifi disconnected
        digitalWrite(LED_green, !digitalRead(LED_green));   // LED toggle
        if (ConnectWifi == true) {                  // nur einmal ausführen
          ConnectWifi = false;                      // nur einmal ausführen
          String logtext = F("Wifi connection lost");
          appendLogFile(logtext);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
          Serial.println(logtext);
#endif
          connectWifiBestRssi(); // connect to AP with best RSSI
          // WiFi.begin(essid.c_str(), epass.c_str());
        }
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
        Serial.print(F("WiFi-Status: ")); Serial.print(WiFi.status()); Serial.print(F(" ("));  Serial.print(strWifiStatus[WiFi.status()]); Serial.println(')'); // 3 = connected
#endif
      } else {                                    // Wifi connected
        if (wpsSuccess == true) {
          String qssid = WiFi.SSID();
          String qpass = WiFi.psk(); // ist hier leer! Nach Neustart allerdings da!
          if ((qssid.length() > 0) && (qpass.length() > 0)) {
            wpsSuccess = false;                   // nur einmal ausführen
            essid = qssid;                                // SSID übernehmen
            epass = qpass;                                // Wifi Passwort übernehmen
            String logText = F("Wifi WPS connected to: ");
            logText += essid;
            appendLogFile(logText);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
            Serial.println(logText);
#endif
            EEPROM_write_string(EEPROM_ADDR_SSID, essid);   // write String to EEPROM
            EEPROM_write_string(EEPROM_ADDR_PASS, epass);   // write String to EEPROM
            appendLogFile(F("Wifi settings saved"));
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
            Serial.print(F("Uptime seconds: "));
            Serial.println(uptime);
            Serial.print(F("WPS connected to: "));
            Serial.println(essid);
            Serial.print(F("WPS with password: "));
            Serial.println(epass);
#endif
          } else {
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
            Serial.print(F("Uptime seconds: "));
            Serial.println(uptime);
            Serial.println(F("WPS SSID or password empty!"));
#endif
          }
        }
        if (ConnectWifi == false) {               // nur einmal ausführen
          ConnectWifi = true;                     // nur einmal ausführen
          digitalWrite(LED_green, LOW) ;          // LED ein
          reconnCount += 1;                       // Counter Reconnects erhöhen
          String logtext = F("Wifi connected to ");
          logtext += WiFi.SSID();
          logtext += F(", channel ");
          logtext += WiFi.channel();
          appendLogFile(logtext);
#ifdef DEBUG_OUTPUT_SERIAL_WIFI
          Serial.println(logtext);
          Serial.print(F("Local IP: "));
          Serial.println(WiFi.localIP());
#endif
        }
        if (ntpServerNr != 3) {                   // NTP eingeschaltet
          ntpSyncSecCount += 1;                   // Zähler erhöhen
          if (second() < 50) {                    // nicht bei Datenspeicherung
            if (ntpSyncSecCount >= ntpSyncInterval) {
              ntpSyncSecCount = 0;
              setTimeNtp();
            }
          }
        }
        // check if MQTT is connected
        if (MqttConnect == true) {              // MQTT eingeschaltet
          if (!client.connected()) {            // MQTT nicht connected
            digitalWrite(LED_red, !digitalRead(LED_red));   // LED toggle
            // Attempt to connect
            client.setServer(eMqttBroker.c_str(), eMqttPort); // Sets the server details.
            if (eMqttUsername.length() == 0) {                // no MQTT Username
              client.connect(OwnStationHostname.c_str());     // Connects the client without authentification
            } else {
              client.connect(OwnStationHostname.c_str(), eMqttUsername.c_str(), eMqttPassword.c_str()); // Connects with authentification
            }
            if (client.connected()) {                           // MQTT Broker connected
              String logtext = F("MQTT Broker ");
              logtext += (eMqttBroker);
              logtext += F(" connected");
              appendLogFile(logtext);
#ifdef DEBUG_OUTPUT_SERIAL_MQTT
              Serial.println(logtext);
#endif
              client.publish(OwnStationHostname.c_str(), "Connected");
              digitalWrite(LED_red, HIGH) ;                     // LED aus
            } else {                                            // MQTT Broker disconnected
#ifdef DEBUG_OUTPUT_SERIAL_MQTT
              Serial.print(F("MQTT connect to "));
              Serial.print(eMqttBroker);
              Serial.print(F(" failed, state="));
              Serial.println(client.state());
              /* Possible values for client.state()
                #define MQTT_CONNECTION_TIMEOUT     -4
                #define MQTT_CONNECTION_LOST        -3
                #define MQTT_CONNECT_FAILED         -2
                #define MQTT_DISCONNECTED           -1
                #define MQTT_CONNECTED               0
                #define MQTT_CONNECT_BAD_PROTOCOL    1
                #define MQTT_CONNECT_BAD_CLIENT_ID   2
                #define MQTT_CONNECT_UNAVAILABLE     3
                #define MQTT_CONNECT_BAD_CREDENTIALS 4
                #define MQTT_CONNECT_UNAUTHORIZED    5
              */
              switch (client.state()) {
                case -4:
                  Serial.println(F("MQTT_CONNECTION_TIMEOUT"));
                  break;
                case -3:
                  Serial.println(F("MQTT_CONNECTION_LOST"));
                  break;
                case -2:
                  Serial.println(F("MQTT_CONNECTION_FAILED"));
                  break;
                case -1:
                  Serial.println(F("MQTT_DISCONNECTED"));
                  break;
                case 0:
                  Serial.println(F("MQTT_CONNECTED"));
                  break;
                case 1:
                  Serial.println(F("MQTT_CONNECT_BAD_PROTOCOL"));
                  break;
                case 2:
                  Serial.println(F("MQTT_CONNECT_BAD_CLIENT_ID"));
                  break;
                case 3:
                  Serial.println(F("MQTT_CONNECT_UNAVAILABLE"));
                  break;
                case 4:
                  Serial.println(F("MQTT_CONNECT_BAD_CREDENTIALS"));
                  break;
                case 5:
                  Serial.println(F("MQTT_CONNECT_UNAUTHORIZED"));
                  break;
                default:
                  Serial.println(F("MQTT UNKNOWN ERROR"));
                  break;
              }
#endif
            }
          }
        }
      }
    }

    if (second() == 30) {
      rotateLog();    // Größe Systemlog prüfen und evtl. rotieren
    }

    // kann weg bei endgültiger Version
    //    if (second() == 45) {
    //      if (minute() % 3 == 0) {       // alle x Minuten
    //        // Zähler auf Zufall
    //        long zufall = random(3);    // max. 48 Impulse/h
    //        s0_count_hour += zufall;      // S0-Counter hour
    //        s0_count_day += zufall;     // S0-Counter day
    //        s0_count_month += zufall; // S0-Counter month
    //        s0_count_year += zufall;   // S0-Counter year
    //        s0_count_abs += zufall;     // S0-Counter absolut
    //        s0_count_mqtt += zufall;     // S0-Counter MQTT
    //        DS1307_write_long(DS1307_ADDR_S0COUNTABS, s0_count_abs);     // save S0-Counter to DS1307 RAM
    //        DS1307_write_long(DS1307_ADDR_S0COUNTHOUR, s0_count_hour);   // save S0-Counter hour to DS1307 RAM
    //        // Maximalwerte in DS1307 RAM speichern
    //        if (s0_count_day > s0_count_max_day) {
    //          s0_count_max_day = s0_count_day;                                // Maximum pro Tag übernehmen
    //          DS1307_write_long(DS1307_ADDR_MAXS0DAY, s0_count_max_day);      // Maximum pro Tag speichern
    //          DS1307_write_long(DS1307_ADDR_MAXS0DAYTIME, now());             // aktuelle Uhrzeit speichern
    //        }
    //        if (s0_count_month > s0_count_max_month) {
    //          s0_count_max_month = s0_count_month;                            // Maximum pro Monat übernehmen
    //          DS1307_write_long(DS1307_ADDR_MAXS0MONTH, s0_count_max_month);  // Maximum Monat speichern
    //          DS1307_write_long(DS1307_ADDR_MAXS0MONTHTIME, now());           // aktuelle Uhrzeit speichern
    //        }
    //        if (s0_count_year > s0_count_max_year) {
    //          s0_count_max_year = s0_count_year;                              // Maximum pro Jahr übernehmen
    //          DS1307_write_long(DS1307_ADDR_MAXS0YEAR, s0_count_max_year);    // Maximum Jahr speichern
    //          DS1307_write_long(DS1307_ADDR_MAXS0YEARTIME, now());            // aktuelle Uhrzeit speichern
    //        }
    //      }
    //    }

    if (second() >= 40) {
      if ((minute() + 1) % eMqttPublish_Intervall == 0) {   // MQTT publish interval
        if (MqttConnect == true) {                          // MQTT eingeschaltet
          if (sendMQTT == true) {
            sendMQTT = false;                               // nur einmal ausführen
            if (client.state() == 0) {                      // MQTT_CONNECTED - the client is connected
              char payload[20];
              String topic;
              float fl = 0;
              if (eMqttPublish_s0_count_abs == true) {
                fl = s0_count_abs / 100.0;                  // Zählerstand absolut
                dtostrf(fl, 4, 2, payload);     //4 is mininum width, 2 is precision; float value is copied onto buff
                topic = OwnStationHostname + "/CountAbs";
#ifdef DEBUG_OUTPUT_SERIAL_MQTT
                Serial.print(F("MQTT publish absolute Counter: "));
                Serial.println(payload);
#endif
                client.publish(topic.c_str(), payload);
              }
              if (eMqttPublish_s0_count_mom == true) {
                fl = s0_count_mqtt / 100.0;                  // Zählerstand momentan MQTT
                dtostrf(fl, 4, 2, payload);     //4 is mininum width, 2 is precision; float value is copied onto buff
                topic = OwnStationHostname + "/CountMom";
#ifdef DEBUG_OUTPUT_SERIAL_MQTT
                Serial.print(F("MQTT publish momentan Counter: "));
                Serial.println(payload);
#endif
                client.publish(topic.c_str(), payload);
                s0_count_mqtt = 0;                           // reset S0-Counter Moment
              }
              if (eMqttPublish_rssi == true) {
                topic = OwnStationHostname + "/RSSI";
                sprintf(payload, "%d", WiFi.RSSI());          // RSSI
#ifdef DEBUG_OUTPUT_SERIAL_MQTT
                Serial.print(F("MQTT publish RSSI: "));
                Serial.println(payload);
#endif
                client.publish(topic.c_str(), payload);
              }
              if (reconnCount != reconnCountOld) {
                if (eMqttPublish_recon == true) {
                  topic = OwnStationHostname + "/WiFiReconnects";
                  sprintf(payload, "%d", reconnCount);          // Counter WiFi Reconnects
#ifdef DEBUG_OUTPUT_SERIAL_MQTT
                  Serial.print(F("MQTT publish WiFi Reconnects: "));
                  Serial.println(reconnCount);
#endif
                  client.publish(topic.c_str(), payload);
                  reconnCountOld = reconnCount; // nur einmal ausführen
                }
              }
            }
          }
        }
      }
    } else {
      sendMQTT = true;    // enable send MQTT
    }

    // jährliche, monatliche und stündliche Logs schreiben
    if (minute() == 59) {
      // jährliche und monatliche Logs schreiben
      if (hour() == 23) {
        // jährliche monatliche Logs schreiben
        if (second() >= 45 && saveLogYear == true) {
          saveLogYear = false;      // nur einmal ausführen
          // Werte jeden letzten Tag des Monats in jährliche Dateien schreiben (12 Zeilen pro Datei)
          if (day() == LastDayOfMonth(month(), year())) {     //letzter Tag im Monat
#ifdef DEBUG_OUTPUT_SERIAL_LITTLEFS
            Serial.print(F("S0-Pulse count last month: "));
            Serial.print(TimeToString(now()));                // return Time String from Timestamp
            Serial.print(F(" : "));
            Serial.println(s0_count_month);
#endif
            String FileName = (F("/log/y_"));
            FileName += (year());
            FileName += (F(".log"));
            LittleFSWriteS0Count(FileName, s0_count_month);     // monatliche Werte in Jahresdatei schreiben
            s0_count_month = 0;                               // reset S0-Counter month
#ifdef DEBUG_OUTPUT_SERIAL_LITTLEFS
            Serial.print(F("Days of Month: "));
            Serial.println(LastDayOfMonth(month(), year()));
#endif
            //letzter Tag im Jahr
            if (month() == 12) {
              s0_count_year = 0;                              // reset S0-Counter year
            }
          }
        }

        // monatliche Logs schreiben
        if (second() >= 50 && saveLogMonth == true) {
          saveLogMonth = false;      // nur einmal ausführen
          // Werte jeden Tag in monatliche Dateien schreiben (max 31 Zeilen pro Datei)
#ifdef DEBUG_OUTPUT_SERIAL_LITTLEFS
          Serial.print(F("S0-Pulse count last day: "));
          Serial.print(TimeToString(now()));         // return Time String from Timestamp
          Serial.print(F(" : "));
          Serial.println(s0_count_day);
#endif
          String FileName = ("/log/m_");
          FileName += (month());
          FileName += (F(".log"));
          // prüfe Logdatei und lösche sie, wenn Monat nicht mit aktuellem übereinstimmt
          if (LittleFS.exists(FileName)) {                    // Returns true if a file with given path exists, false otherwise.
#ifdef DEBUG_OUTPUT_SERIAL_LITTLEFS
            Serial.print(F("LittleFS Reading Data from: "));
            Serial.println(FileName);
#endif
            File LogFile = LittleFS.open(FileName, "r");      // Open text file for reading.
            String Datum = LogFile.readStringUntil(' ');    // Lets read string from the file
            LogFile.close();
            Datum.remove(0, 3); // Remove 3 characters starting at index=0
#ifdef DEBUG_OUTPUT_SERIAL_LITTLEFS
            Serial.print(Datum);
            Serial.println(F("<-- Monat und Jahr aus Datei"));
#endif
            String DatumNow(DateToString(now()));
            DatumNow.remove(0, 3); // Remove 3 characters starting at index=0
#ifdef DEBUG_OUTPUT_SERIAL_LITTLEFS
            Serial.print(DatumNow);
            Serial.println(F("<-- Monat und Jahr heute"));
#endif
            if (Datum != DatumNow) {
#ifdef DEBUG_OUTPUT_SERIAL_LITTLEFS
              String logtext = F("LittleFS Delete old log: ");
              logtext += (FileName);
              Serial.println(F("Monat und Jahr nicht identisch"));
              Serial.println(logtext);
              appendLogFile(logtext);
#endif
              LittleFS.remove(FileName);  // Deletes the file given its absolute path. Returns true if file was deleted successfully.
            }
          }
          LittleFSWriteS0Count(FileName, s0_count_day);   // tägliche Werte in Monatsdatei schreiben
          s0_count_day = 0;                                   // reset S0-Counter day
        }
      }

      // stündliche Logs schreiben
      if (second() >= 55 && saveLogDayBool == true) {
        saveLogDayBool = false;      // nur einmal ausführen
        // Werte jede Stunde in tägliche Dateien schreiben (24 Zeilen pro Datei)
#ifdef DEBUG_OUTPUT_SERIAL_LITTLEFS
        Serial.print(F("S0-Pulse count last hour: "));
        Serial.print(TimeToString(now()));         // return Time String from Timestamp
        Serial.print(F(" : "));
        Serial.println(s0_count_hour);
#endif
        String FileName = (F("/log/d_"));
        FileName += (day());
        FileName += (F(".log"));
        // prüfe Logdatei und lösche sie, wenn Datum nicht mit aktuellem übereinstimmt
        if (LittleFS.exists(FileName)) {                    // Returns true if a file with given path exists, false otherwise.
#ifdef DEBUG_OUTPUT_SERIAL_LITTLEFS
          Serial.print(F("LittleFS Reading Data from: "));
          Serial.println(FileName);
#endif
          File LogFile = LittleFS.open(FileName, "r");      // Open text file for reading.
          String Datum = LogFile.readStringUntil(' ');    // Lets read string from the file
          LogFile.close();
#ifdef DEBUG_OUTPUT_SERIAL_LITTLEFS
          Serial.print(Datum);
          Serial.println(F("<-- Datum aus Datei"));
          Serial.print(DateToString(now()));
          Serial.println(F("<-- Datum heute"));
#endif
          if (Datum != DateToString(now())) {
#ifdef DEBUG_OUTPUT_SERIAL_LITTLEFS
            String logtext = F("LittleFS Delete old log: ");
            logtext += (FileName);
            Serial.println(F("Datum nicht identisch"));
            Serial.println(logtext);
            appendLogFile(logtext);
#endif
            LittleFS.remove(FileName);  // Deletes the file given its absolute path. Returns true if file was deleted successfully.
          }
        }
        LittleFSWriteS0Count(FileName, s0_count_hour);    // write counter to logfile
        DS1307_write_long(DS1307_ADDR_S0COUNTHOUR, 0);   // save S0-Counter hour to DS1307 RAM
        s0_count_hour = 0;                              // reset S0-Counter hour
        // Anzahl Logzeilen einmal pro Tag prüfen
        if (hour() == 23) {
          // Anzahl Zeilen ermitteln
          int rows = countLogLines(FileName); // Anzahl Zeilen ermitteln
          if (rows < numberLogLinesDay) { // wird bei Zeitumstellung Sommer/Winter geändert, normal 24
            String logtext = F("GZ16 number of log entries per day < ");
            logtext += numberLogLinesDay;
            logtext += F(" (");
            logtext += rows;
            logtext += ')';
            appendLogFile(logtext);
          }
          numberLogLinesDay = 24;
        }
      }
    } else {
      saveLogYear = true;
      saveLogMonth = true;
      saveLogDayBool = true;
    }
  }
}
