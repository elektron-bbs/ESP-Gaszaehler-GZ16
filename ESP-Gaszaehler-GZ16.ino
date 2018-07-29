#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <EEPROM.h>
#include <FS.h>
#include <TimeLib.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SPI.h>
extern "C" {
#include "user_interface.h"
}
#include <detail\RequestHandlersImpl.h>

#define HW_VERSION_1                // Version 1: ESP-12E, auskommentieren für Version 2 mit NodeMCU

#ifdef HW_VERSION_1
ADC_MODE(ADC_VCC);                  // vcc read
#else
const float ADC_DIV = 190.0;        // Divisor für Batteriespannung bei HW-Version 2
#endif

#define VERSION                     1
#define BUILD                       86
#define DEBUG_OUTPUT                false
//#define DEBUG_OUTPUT                true

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
#define EEPROM_ADDR_NTPSERVERNUMBER 96
#define EEPROM_ADDR_START           97
#define EEPROM_ADDR_MQTTINTERVALL   98
// Boolean
#define EEPROM_ADDR_DST             112
#define EEPROM_ADDR_DHCP            113
#define EEPROM_ADDR_MQTTCONNECT     114
#define EEPROM_ADDR_MQTTPUBLISHABS  115
#define EEPROM_ADDR_MQTTPUBLISHMOM  116
#define EEPROM_ADDR_MQTTPUBLISHRSSI 117
#define EEPROM_ADDR_SERIALOUTPUT    118
// EEPROM Data
// long (4 Bytes)
#define EEPROM_ADDR_S0TRIP1         168
#define EEPROM_ADDR_S0TRIP1TIME     172
#define EEPROM_ADDR_S0TRIP2         176
#define EEPROM_ADDR_S0TRIP2TIME     180

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
boolean edhcp = true;                     // IP-Adresse mittels DHCP
IPAddress eip;                            // IP-Adresse static IP
IPAddress esnm;                           // Subnetzmaske static
IPAddress esgw;                           // Standard-Gateway static
IPAddress edns;                           // Domain Name Server static
boolean ConnectWifi = false;
String OwnStationHostname = "GZ16-";
unsigned long ulReconncount;              // Counter Reconnects
unsigned long ulWifiRxBytes;              // Count Received Bytes
unsigned long ulWifiTxBytes;              // Count Transmitted Bytes
byte webtype = 255;
byte estart;                              // Start-Zähler für WPS, AP etc.
int WifiNetworks;                         // Anzahl Wifi-Netzwerke

// MQTT
PubSubClient client(espClient);
String eMqttBroker = "";                      // MQTT-Broker
String eMqttUsername = "";                    // MQTT-Username
String eMqttPassword = "";                    // MQTT-Password
int eMqttPort = 0;                            // MQTT-Port
byte eMqttPublish_Intervall = 3;              // MQTT publish intervall
boolean eMqttPublish_s0_count_abs = false;    // MQTT publish S0-Counter absolut
boolean eMqttPublish_s0_count_mom = false;    // MQTT publish S0-Counter Moment
boolean eMqttPublish_rssi = false;            // MQTT publish WLAN RSSI
boolean MqttConnect = false;                  // MQTT connect on/off
boolean sendMQTT = false;

// Zeit
const int8_t TimeZone = 1;                    // Europe/Berlin
String ntpServerName[3] = {"pool.ntp.org", "de.pool.ntp.org", ""};
byte ntpServerNr = 0;
boolean DST = true;
unsigned long ntpSyncInterval = 0;            // time sync will be attempted after this many seconds
unsigned long ntpSyncSecCount = 0;            // Counter Seconds since last NTP sync
long ntpSyncTimeDeviation = 0;                // Abweichung Zeit intern zu NTP
unsigned long lastSetTime;
int sec_old;
unsigned long uptime;

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
boolean SerialOutput;   // serielle Ausgabe ein/aus
long DiagrammTimestamp;

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
  SerialPrintLine();            // Trennlinie seriell ausgeben
  if (! DS1307_isrunning()) {
    //appendLogFile("RTC is NOT running!");
    Serial.println(F("RTC is NOT running! Please set date and time."));
    digitalWrite(LED_red, LOW) ;     // LED ein
    DS1307_clear_ram();
    //Serial.println("Set date and time to compile time");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(__DATE__, __TIME__));
    //rtc.adjust(DateTime("Feb 28 2017","23:59:45"));
  } else {
    DS1307_getTime();                          // get date and time from DS1307
    lastSetTime = now();
    //appendLogFile("RTC read date and time");
    Serial.println(F("RTC read date and time:"));
    Serial.print(F("Date: "));
    Serial.println(DateToString(now()));        // return Date String from Timestamp
    Serial.print(F("Day of week: "));
    Serial.println(weekday());                  // Day of the week, Sunday is day 1
    Serial.print(F("Time: "));
    Serial.println(TimeToString(now()));        // return Time String from Timestamp
  }

  // Initialize file system.
  SerialPrintLine();            // Trennlinie seriell ausgeben
  if (!SPIFFS.begin()) {
    //filesystem = false;
    Serial.println(F("SPIFFS Failed to mount file system"));
  } else {
    //filesystem = true;
    // always use this to "mount" the filesystem
    //bool result = SPIFFS.begin();
    Serial.println(F("SPIFFS Filesystem mounted"));
    File logFile = SPIFFS.open("/log/system.log", "r");     //Open text file for reading.
    if (!logFile) {
      Serial.println(F("SPIFFS file /log/system.log doesn't exist yet. Creating it"));
      File logFile = SPIFFS.open("/log/system.log", "w");   // Truncate file to zero length or create text file for writing.
      if (!logFile) {
        Serial.println(F("SPIFFS file creation failed"));
      } else {
        logFile.println("---------- CREATE LOG FILE ----------");
      }
    }
    logFile.close();
    appendLogFile("GZ16 ESP8266 Restart");
  }

  // Get ESP8266 infomation
  print_info();

  // disconnect wifi
  WiFi.disconnect();
  delay(100);

  // start EEPROM
  EEPROM.begin(EEPROM_MAX_ADDR);                // max 4096 Bytes
  eeprom_alldata_read();                         // read all data from EEPROM

  // Read S0-Data from SPIFFS
  String FileName = (F("/log/d_"));                    // lese aktuellen Tag
  FileName += (day());
  FileName += (F(".log"));     // Dateiende ".log" anhängen
  s0_count_day = SpiffsReadS0Count(FileName, 1);
  FileName = (F("/log/m_"));                           // lese aktuellen Monat
  FileName += (month());
  FileName += (F(".log"));     // Dateiende ".log" anhängen
  s0_count_month = SpiffsReadS0Count(FileName, 2);
  FileName = (F("/log/y_"));                           // lese aktuelles Jahr
  FileName += (year());
  FileName += (F(".log"));     // Dateiende ".log" anhängen
  s0_count_year = SpiffsReadS0Count(FileName, 3);
  SerialPrintLine();            // Trennlinie seriell ausgeben

  // Read S0-Data from DS1307
  ds1307_alldata_read();                         // read all data from DS1307 RAM
  SerialPrintLine();            // Trennlinie seriell ausgeben
  s0_count_day += s0_count_hour;
  s0_count_month += s0_count_day;
  s0_count_year += s0_count_month;
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

  // Hostnamen bilden
  SerialPrintLine();            // Trennlinie seriell ausgeben
  Serial.print(F("Hostname: "));
  Serial.println(WiFi.hostname());
  OwnStationHostname += WiFi.hostname();
  OwnStationHostname.replace("_", "-");       // Unterstrich ersetzen, nicht zulässig im Hostnamen
  Serial.print(F("new Hostname: "));
  Serial.println(OwnStationHostname);
  WiFi.hostname(OwnStationHostname);
  char* chrOwnHostname = &OwnStationHostname[0];
  wifi_station_set_hostname(chrOwnHostname);

  if (estart > 3) {                                         // max. 3 Versuche WPS
    // 3 x Fehler WPS - starte Accesspoint
    setupAP();                                              // Accesspoint starten
  } else {
    if ((essid.length() != 0) && (epass.length() != 0)) {   // SSID und Passwort im EEPROM
      startStation();                                     //connect to Wifi in Station Mode
    } else {                                      // keine SSID und keine Passwort im EEPROM
      Serial.println(F("No SSID and Password in EEPROM"));
      // starte WiFi Protected Setup (WPS)
      if (!start_WPS_connect()) {                         // Wifi WPS Connection failed
        estart += 1;                                       // Start-Zähler erhöhen
        String logText = F("Wifi WPS connection failed #");
        logText += estart;
        Serial.println(logText);
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
    if (Button_Press_Count > 0) {
      Button_Press_Time++;                            // increase timer
      digitalWrite(LED_red, !digitalRead(LED_red));   // LED toggle
      if (Button_Press_Time > 5) {
        switch (Button_Press_Count) {
          case 3:   // Taster 3x betätigt - WPS starten
            // starte WiFi Protected Setup (WPS)
            if (!start_WPS_connect()) {               // Wifi WPS Connection failed
              String logText = F("Wifi WPS connection failed #");
              logText += estart;
              appendLogFile(logText);
              if (SerialOutput == 1) {                // serielle Ausgabe eingeschaltet
                Serial.println(logText);
              }
              delay(1000);
              /* After programming your ESP, the ESP.restart(); does not always work.
                 You have to reset it manually, after programming. Because after it reliable Works
              */
              ESP.restart();
            }
            break;
          case 4:   // Taster 4x betätigt - Accesspoint starten
            WiFi.disconnect();                // disconnect wifi
            delay(100);
            setupAP();                        // Accesspoint starten
            break;
          case 5:   // Taster 5x betätigt - GZ16 neu starten
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
          if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
            Serial.println(logtext);
          }
          WiFi.begin(essid.c_str(), epass.c_str());
        }
        if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
          Serial.print(F("Wifi-Status: "));
          Serial.println(WiFi.status());
        }
      } else {                                    // Wifi connected
        if (ConnectWifi == false) {               // nur einmal ausführen
          ConnectWifi = true;                     // nur einmal ausführen
          digitalWrite(LED_green, LOW) ;          // LED ein
          ulReconncount += 1;                     // Counter Reconnects erhöhen
          String logtext = F("Wifi connected to: ");
          logtext += WiFi.SSID();
          appendLogFile(logtext);
          if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
            Serial.println(logtext);
            Serial.print("Local IP: ");
            Serial.println(WiFi.localIP());
          }
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
            if (eMqttUsername.length() == 0) {                  // no MQTT Username
              client.connect(OwnStationHostname.c_str());       // Connects the client without authentification
            } else {
              client.connect(OwnStationHostname.c_str(), eMqttUsername.c_str(), eMqttPassword.c_str()); // Connects with authentification
            }
            if (client.connected()) {                           // MQTT Broker connected
              String logtext = F("MQTT Broker ");
              logtext += (eMqttBroker);
              logtext += F(" connected");
              appendLogFile(logtext);
              if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
                Serial.println(logtext);
              }
              client.publish(OwnStationHostname.c_str(), "Connected");
              digitalWrite(LED_red, HIGH) ;                     // LED aus
            } else {                                            // MQTT Broker disconnected
              if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
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
              }
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
      if ((minute() + 1) % eMqttPublish_Intervall == 0) {      // MQTT publish interval
        if (MqttConnect == true) {                        // MQTT eingeschaltet
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
                if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
                  Serial.print(F("MQTT publish absolute Counter: "));
                  Serial.println(payload);
                }
                client.publish(topic.c_str(), payload);
              }
              if (eMqttPublish_s0_count_mom == true) {
                fl = s0_count_mqtt / 100.0;                  // Zählerstand momentan MQTT
                dtostrf(fl, 4, 2, payload);     //4 is mininum width, 2 is precision; float value is copied onto buff
                topic = OwnStationHostname + "/CountMom";
                if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
                  Serial.print(F("MQTT publish momentan Counter: "));
                  Serial.println(payload);
                }
                client.publish(topic.c_str(), payload);
                s0_count_mqtt = 0;                           // reset S0-Counter Moment
              }
              if (eMqttPublish_rssi == true) {
                topic = OwnStationHostname + "/RSSI";
                sprintf(payload, "%d", WiFi.RSSI());          // RSSI
                if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
                  Serial.print(F("MQTT publish RSSI: "));
                  Serial.println(payload);
                }
                client.publish(topic.c_str(), payload);
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
#if DEBUG_OUTPUT == true
            unsigned long startOperation = millis();   // benötigte Rechenzeit für Operation ermitteln (vor Beginn einfügen)
#endif
            if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
              Serial.print(F("S0-Pulse count last month: "));
              Serial.print(TimeToString(now()));                // return Time String from Timestamp
              Serial.print(F(" : "));
              Serial.println(s0_count_month);
            }
            String FileName = (F("/log/y_"));
            FileName += (year());
            FileName += (F(".log"));
            SpiffsWriteS0Count(FileName, s0_count_month);     // monatliche Werte in Jahresdatei schreiben
            s0_count_month = 0;                               // reset S0-Counter month
            if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
              Serial.print(F("Days of Month: "));
              Serial.println(LastDayOfMonth(month(), year()));
            }
            //letzter Tag im Jahr
            if (month() == 12) {
              s0_count_year = 0;                              // reset S0-Counter year
            }
#if DEBUG_OUTPUT == true
            TimeOfOperation(startOperation);   // benötigte Rechenzeit für Operation (nach Ende einfügen)
#endif
          }
        }

        // monatliche Logs schreiben
        if (second() >= 50 && saveLogMonth == true) {
          saveLogMonth = false;      // nur einmal ausführen
          // Werte jeden Tag in monatliche Dateien schreiben (max 31 Zeilen pro Datei)
#if DEBUG_OUTPUT == true
          unsigned long startOperation = millis();   // benötigte Rechenzeit für Operation ermitteln (vor Beginn einfügen)
#endif
          if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
            Serial.print(F("S0-Pulse count last day: "));
            Serial.print(TimeToString(now()));         // return Time String from Timestamp
            Serial.print(F(" : "));
            Serial.println(s0_count_day);
          }
          String FileName = ("/log/m_");
          FileName += (month());
          FileName += (F(".log"));
          // prüfe Logdatei und lösche sie, wenn Monat nicht mit aktuellem übereinstimmt
          if (SPIFFS.exists(FileName)) {                    // Returns true if a file with given path exists, false otherwise.
            if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
              Serial.print(F("SPIFFS Reading Data from: "));
              Serial.println(FileName);
            }
            File LogFile = SPIFFS.open(FileName, "r");      // Open text file for reading.
            String Datum = LogFile.readStringUntil(' ');    // Lets read string from the file
            LogFile.close();
            Datum.remove(0, 3); // Remove 3 characters starting at index=0
            if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
              Serial.print(Datum);
              Serial.println(F("<-- Monat und Jahr aus Datei"));
            }
            String DatumNow(DateToString(now()));
            DatumNow.remove(0, 3); // Remove 3 characters starting at index=0
            if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
              Serial.print(DatumNow);
              Serial.println(F("<-- Monat und Jahr heute"));
            }
            if (Datum != DatumNow) {
              String logtext = F("SPIFFS Delete old log: ");
              logtext += (FileName);
              if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
                Serial.println(F("Monat und Jahr nicht identisch"));
                Serial.println(logtext);
              }
#if DEBUG_OUTPUT == true
              appendLogFile(logtext);
#endif
              SPIFFS.remove(FileName);  // Deletes the file given its absolute path. Returns true if file was deleted successfully.
            }
          }
          SpiffsWriteS0Count(FileName, s0_count_day);   // tägliche Werte in Monatsdatei schreiben
          s0_count_day = 0;                                   // reset S0-Counter day
#if DEBUG_OUTPUT == true
          TimeOfOperation(startOperation);   // benötigte Rechenzeit für Operation (nach Ende einfügen)
#endif
        }
      }

      // stündliche Logs schreiben
      if (second() >= 55 && saveLogDayBool == true) {
        saveLogDayBool = false;      // nur einmal ausführen
        // Werte jede Stunde in tägliche Dateien schreiben (24 Zeilen pro Datei)
#if DEBUG_OUTPUT == true
        unsigned long startOperation = millis();   // benötigte Rechenzeit für Operation ermitteln (vor Beginn einfügen)
#endif
        if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
          Serial.print(F("S0-Pulse count last hour: "));
          Serial.print(TimeToString(now()));         // return Time String from Timestamp
          Serial.print(F(" : "));
          Serial.println(s0_count_hour);
        }
        String FileName = (F("/log/d_"));
        FileName += (day());
        FileName += (F(".log"));
        // prüfe Logdatei und lösche sie, wenn Datum nicht mit aktuellem übereinstimmt
        if (SPIFFS.exists(FileName)) {                    // Returns true if a file with given path exists, false otherwise.
          if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
            Serial.print(F("SPIFFS Reading Data from: "));
            Serial.println(FileName);
          }
          File LogFile = SPIFFS.open(FileName, "r");      // Open text file for reading.
          String Datum = LogFile.readStringUntil(' ');    // Lets read string from the file
          LogFile.close();
#if DEBUG_OUTPUT == true
          Serial.print(Datum);
          Serial.println(F("<-- Datum aus Datei"));
          Serial.print(DateToString(now()));
          Serial.println(F("<-- Datum heute"));
#endif
          if (Datum != DateToString(now())) {
            String logtext = F("SPIFFS Delete old log: ");
            logtext += (FileName);
            if (SerialOutput == 1) {    // serielle Ausgabe eingeschaltet
              Serial.println(F("Datum nicht identisch"));
              Serial.println(logtext);
            }
#if DEBUG_OUTPUT == true
            appendLogFile(logtext);
#endif
            SPIFFS.remove(FileName);  // Deletes the file given its absolute path. Returns true if file was deleted successfully.
          }
        }
        SpiffsWriteS0Count(FileName, s0_count_hour);    // write counter to logfile
        DS1307_write_long(DS1307_ADDR_S0COUNTHOUR, 0);   // save S0-Counter hour to DS1307 RAM
        s0_count_hour = 0;                              // reset S0-Counter hour
#if DEBUG_OUTPUT == true
        TimeOfOperation(startOperation);   // benötigte Rechenzeit für Operation (nach Ende einfügen)
#endif
      }
    } else {
      saveLogYear = true;
      saveLogMonth = true;
      saveLogDayBool = true;
    }
  }
}

