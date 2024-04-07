/* https://github.com/esp8266/Arduino/tree/master
  Einstellungen:
  --------------
  Board: NodeMCU 1.0 (ESP-12E Module)
  Flash-Size: 4MB (FS:3MB OTA:~512KB)

  Bibliotheken installieren:
  --------------------------
  Time          by Michael Margolis

  PubSubClient  by Nick O'Leary
  UB: mosquitto MQTT v3.1 broker on debian wheezy - Änderung notwendig!
  - The client uses MQTT 3.1.1 by default. It can be changed to use MQTT 3.1 by
  - changing value of `MQTT_VERSION` in `PubSubClient.h`.
  // MQTT_VERSION : Pick the version
  #define MQTT_VERSION MQTT_VERSION_3_1
  #ifndef MQTT_VERSION
  #define MQTT_VERSION MQTT_VERSION_3_1_1
  #endif

  für ESP8266 SPIFFS Sketch Data Upload erforderlich
  https://github.com/esp8266/arduino-esp8266fs-plugin
  https://github.com/esp8266/arduino-esp8266fs-plugin/releases/
  kopieren nach:
  E:\Eigene Dateien\Eigene Dokumente\Arduino\tools\ESP8266FS\tool\esp8266fs.jar

  für ESP8266 Little Data-Upload-Tool installieren:
  https://github.com/earlephilhower/arduino-esp8266littlefs-plugin
  https://github.com/earlephilhower/arduino-esp8266littlefs-plugin/releases

  ---------------------------------------------------------------------------------------------------------------------

  How to upgrade a file system via OTA
  ------------------------------------
  Here is original: https://www.fancon.cz/esp8266-humi-server-klient/arduino-humi-server-client.html#otafile

  Translation:
  How to upgrade a file system via OTA
  1/28/2021
  I've been looking for advice for a long time on how to upload a file to a file system via OTA, as I mentioned above.
  But I always broke down LittleFS (SPIFFS).
  But now I read that I can't upload files directly, but first I have to convert the whole file system to * .bin.
  There is a special tool for this, which does not work more than it works.
  But I found in that scary jungle of the internet (They should cancel it ...) a gem, advice on gold.

  To upgrade via a serial line, you always need tools (downloaded from github.com), which I have already installed in the Arduino IDE.
  If I have data for SPIFFS stored in the project in the data directory where it must be located,
  then just run the ESP8266 LittleFS Data Upload tool over the open project.
  He is trying to record via a serial line, but since the ESP8266 is not connected, it crashes.
  Before his death, however, he writes where he created the file system in the * .bin format.

  For a project named HUMI3_LOOP, it looks something like this:
  [LittleFS] upload: C: \ Users \ Pavel \ AppData \ Local \ Temp \ arduino_build_732033 / HUMI3_LOOP.mklittlefs.bin
  [LittleFS] upload  : C:\Users\SiHeiTec\AppData\Local\Temp\arduino_build_686077/ESP_Gaszaehler_V1_93_Core302_LittleFS.mklittlefs.bin

  And then you just need to extract the HUMI3_LOOP.mklittlefs.bin file from this location and upload it via OTA with the FileSystem entry.
  In order to be able to easily transfer the file to the client, the settings in your ESP8266 http server must not be missing.

  For favicon.ico it is this line:
  server.serveStatic ("/ favicon.ico", LittleFS, "favicon.ico");

  That line of code tells the server to reach the file system when a client requests a favicon.ico file and send the requested file directly to the client.
  Amazing with its simplicity. If the file is not found, then the client's 404 File not found is returned.
  Beautifully transparent and it even works. Only in this work does the accuracy of the name and the size of the letters matter.

  But when I first successfully uploaded favicon.ico via OTA, a tiny icon didn't appear in Chrome.
  I deleted the cache and who knows what else. Eventually I ran Edge and lo and behold, the icon was.
  Chrome remembered that he hadn't received a favicon from a source before, and then didn't ask for it again. But after a while he could tell.

  You can also easily get a file from the ESP8266 http server file system,
  including favicon.ico, by simply querying IP-ESP8266 / favicon.ico and having it on the screen.
  It's amazing when I uploaded a nice JPG image via OTA and the browser could view it directly from that mischievous cockroach.
  Please note the tiny blue favicon at the top left. The image itself is full screen, but I have reduced the size of the browser window to see what and how.
  If you know all this and you knew how to do it, then you should have told me ...

  ---------------------------------------------------------------------------------------------------------------------
  2023-11-05 - ESP_Gaszaehler_V1_97_Core312_WPS
  . Variables and constants in RAM (global, static), used 37368 / 80192 bytes (46%)
  ║   SEGMENT  BYTES    DESCRIPTION
  ╠══ DATA     1552     initialized variables
  ╠══ RODATA   4168     constants
  ╚══ BSS      31648    zeroed variables
  . Instruction RAM (IRAM_ATTR, ICACHE_RAM_ATTR), used 61863 / 65536 bytes (94%)
  ║   SEGMENT  BYTES    DESCRIPTION
  ╠══ ICACHE   32768    reserved space for flash instruction cache
  ╚══ IRAM     29095    code in IRAM
  . Code in flash (default, ICACHE_FLASH_ATTR), used 410176 / 1048576 bytes (39%)
  ║   SEGMENT  BYTES    DESCRIPTION
  ╚══ IROM     410176   code in flash

  2023-10-22 - ESP_Gaszaehler_V1_97_Core312_WPS.ino
  - MDNS entfernt
  - WPS aktualisiert
  - Webseiten überarbeitet
  - Struktur LittleFS geändert (update data erforderlich!)
  . Variables and constants in RAM (global, static), used 37492 / 80192 bytes (46%)
  ║   SEGMENT  BYTES    DESCRIPTION
  ╠══ DATA     1552     initialized variables
  ╠══ RODATA   4300     constants
  ╚══ BSS      31640    zeroed variables
  . Instruction RAM (IRAM_ATTR, ICACHE_RAM_ATTR), used 61863 / 65536 bytes (94%)
  ║   SEGMENT  BYTES    DESCRIPTION
  ╠══ ICACHE   32768    reserved space for flash instruction cache
  ╚══ IRAM     29095    code in IRAM
  . Code in flash (default, ICACHE_FLASH_ATTR), used 411864 / 1048576 bytes (39%)
  ║   SEGMENT  BYTES    DESCRIPTION
  ╚══ IROM     411864   code in flash

  2023-09-07 - ESP_Gaszaehler_V1_97_Core312_WLAN
  . Variables and constants in RAM (global, static), used 37624 / 80192 bytes (46%)
  ║   SEGMENT  BYTES    DESCRIPTION
  ╠══ DATA     1552     initialized variables
  ╠══ RODATA   4280     constants
  ╚══ BSS      31792    zeroed variables
  . Instruction RAM (IRAM_ATTR, ICACHE_RAM_ATTR), used 62231 / 65536 bytes (94%)
  ║   SEGMENT  BYTES    DESCRIPTION
  ╠══ ICACHE   32768    reserved space for flash instruction cache
  ╚══ IRAM     29463    code in IRAM
  . Code in flash (default, ICACHE_FLASH_ATTR), used 431884 / 1048576 bytes (41%)
  ║   SEGMENT  BYTES    DESCRIPTION
  ╚══ IROM     431884   code in flash

  2023-09-04 - ESP_Gaszaehler_V1_97_Core312_opt2
  Websites: sResponse.reserve(8192);
  geändert: Einstellungen WLAN, verbindet mit gewähltem AP
  geändert: connectWifiBestRssi(); // connect to AP with best RSSI

  Arduino 1.8.19, esp8266-3.1.2 - ESP_Gaszaehler_V1_97_Core312_WLAN
  geändert: sResponse += F(
  geändert: zusätzliche Zeile einfügen bei Fehler IP Adresse
  5 * geändert:
  // sResponse += String(ipadr[0]) + '.' + String(ipadr[1]) + '.' + String(ipadr[2]) + '.' + String(ipadr[3]);
  sprintf(str, "%d.%d.%d.%d", ipadr[0], ipadr[1], ipadr[2], ipadr[3]);
  sResponse += str;
  Seite Einstellungen WLAN 4 * <td> ersetzt durch <td colspan=\"4\">
  133 * colspan=\"1\" entfernt
  . Variables and constants in RAM (global, static), used 37624 / 80192 bytes (46%)
  ║   SEGMENT  BYTES    DESCRIPTION
  ╠══ DATA     1552     initialized variables
  ╠══ RODATA   4280     constants
  ╚══ BSS      31792    zeroed variables
  . Instruction RAM (IRAM_ATTR, ICACHE_RAM_ATTR), used 62231 / 65536 bytes (94%)
  ║   SEGMENT  BYTES    DESCRIPTION
  ╠══ ICACHE   32768    reserved space for flash instruction cache
  ╚══ IRAM     29463    code in IRAM
  . Code in flash (default, ICACHE_FLASH_ATTR), used 432412 / 1048576 bytes (41%)
  ║   SEGMENT  BYTES    DESCRIPTION
  ╚══ IROM     432412   code in flash

  Arduino 1.8.19, esp8266-3.1.2 - ESP_Gaszaehler_V1_97_Core312_opt
  -----------------------------------------------------------------------
  2022-12-19
  - client.setServer(eMqttBroker.c_str(), eMqttPort); // Sets the server details.
    gelöscht in void startStation() und bool start_WPS_connect(), eingefügt in if (MqttConnect == true)
  Der Sketch verwendet 479509 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 37568 Bytes (45%) des dynamischen Speichers, 44352 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.

  2022-06-04
   - in Websites.ino void SiteSetup diverse Logausausgaben aus submit ans Ende verschoben
  Der Sketch verwendet 479541 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 37584 Bytes (45%) des dynamischen Speichers, 44336 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.

  2022-06-02
  - LittleFS.setTimeCallback (LittleFS filesystems file timestamp hinzugefügt), entfernt:
  - extern "C" {
  - #include "user_interface.h"
  - }
  Der Sketch verwendet 479733 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 37584 Bytes (45%) des dynamischen Speichers, 44336 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.

  2022-01-27
  - add command restart
  - F-Macro in server.on...
  Der Sketch verwendet 479685 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 37584 Bytes (45%) des dynamischen Speichers, 44336 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.

  2022-01-24
  Der Sketch verwendet 479229 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 37744 Bytes (46%) des dynamischen Speichers, 44176 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.

  2022-01-22
  Der Sketch verwendet 479209 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 37804 Bytes (46%) des dynamischen Speichers, 44116 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 479273 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 37804 Bytes (46%) des dynamischen Speichers, 44116 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 479513 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 37788 Bytes (46%) des dynamischen Speichers, 44132 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.

  2022-01-21
  HW_VERSION_1
  Der Sketch verwendet 479481 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 37788 Bytes (46%) des dynamischen Speichers, 44132 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  HW_VERSION_2
  Der Sketch verwendet 479477 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 37788 Bytes (46%) des dynamischen Speichers, 44132 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.

  2022-01-16
  Der Sketch verwendet 479381 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 37864 Bytes (46%) des dynamischen Speichers, 44056 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 479365 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 38284 Bytes (46%) des dynamischen Speichers, 43636 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 479333 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 38124 Bytes (46%) des dynamischen Speichers, 43796 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.

  - Datei "Websites" void SiteFiles() "Dateien anzeigen" ändern
  Der Sketch verwendet 479389 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 38408 Bytes (46%) des dynamischen Speichers, 43512 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.

  - Hostnamen bilden funktioniert nicht mehr, wird jetzt aus ESP.getChipId() gebildet
  Der Sketch verwendet 478109 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 38412 Bytes (46%) des dynamischen Speichers, 43508 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.

  - Alle Einträge "SPIFFS" ersetzt durch "LittleFS"
  Der Sketch verwendet 478133 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 38392 Bytes (46%) des dynamischen Speichers, 43528 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.

  2022-01-14
  Aussetzer beim Schreiben in tägliche Logs
  https://forum.arduino.cc/t/esp32-mit-spiffs-eine-speicher-katastrophe/621573/14
  Umstellung SPIFFS -> LittleFS
  Der Sketch verwendet 477469 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 38108 Bytes (46%) des dynamischen Speichers, 43812 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.

  Arduino 1.8.19, esp8266-3.0.2 - ESP_Gaszaehler_V1_93_Core302_LittleFS
  -----------------------------------------------------------------------

  2020-03-28
  Aussetzer beim Schreiben in tägliche und monatliche Logs seit 19.03.2020, Data neu geschrieben, Sketch compiliert
  Der Sketch verwendet 478256 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 36976 Bytes (45%) des dynamischen Speichers, 44944 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.

  Arduino 1.8.12, esp8266-2.6.3 - ESP_Gaszaehler_V1_92_Core263_ICACHE_RAM
  -----------------------------------------------------------------------

  2020-02-14
  Interrupt-Routinen verschoben von Misc.ino in ESP_Gaszaehler_V1_91_Core263_ICACHE_RAM.ino
  und Attribut ICACHE_RAM_ATTR bei beiden ISR hinzugefügt.

  Der Sketch verwendet 478224 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 36976 Bytes (45%) des dynamischen Speichers, 44944 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.

  Arduino 1.8.10, esp8266-2.6.3 - ESP_Gaszaehler_V1_91_Core263_ICACHE_RAM
  -----------------------------------------------------------------------

  2019-11-23
  Versuch Firmwareupdate mit esp8266-2.6.1 - dauernde Neustarts, zurück auf esp8266-2.5.0

  Der Sketch verwendet 468988 Bytes (44%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 37740 Bytes (46%) des dynamischen Speichers, 44180 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.

  // String contentTyp = StaticRequestHandler::getContentType(filename);
  String contentTyp = getContentType(filename);
  // #include <detail\RequestHandlersImpl.h>
  lwIPVariant - v2 lower memory (no features)
  Exeptions - Legacy (new can return nullptr)
  SSL Support - Basic

  Arduino 1.8.10, esp8266-2.6.1 - ESP_Gaszaehler_V1_90_Core261
  -------------------------------------------------------------

  Der Sketch verwendet 469252 Bytes (44%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 37720 Bytes (46%) des dynamischen Speichers, 44200 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.

  Der Sketch verwendet 475180 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 37776 Bytes (46%) des dynamischen Speichers, 44144 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Arduino 1.8.8, esp8266-2.5.0 - ESP_Gaszaehler_V1_89_ResetReason - Reset reason in log
  Der Sketch verwendet 474788 Bytes (45%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 37792 Bytes (46%) des dynamischen Speichers, 44128 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.

  Arduino 1.8.8, esp8266-2.5.0 - ESP_Gaszaehler_V1_88_MQTT - publish WiFiReconnects, WiFi channel in log
  ------------------------------------------------------------------------------------------------------

  compile error, Fehler in boards.txt, WPS disabled
  undefined reference to `WPS_is_unavailable_in_this_configuration__Please_check_FAQ_or_board_generator_tool'
  Arduino 1.8.7, esp8266-2.4.2 - ESP_Gaszaehler_V1_86_Diagramme
  -------------------------------------------------------------

  Der Sketch verwendet 437593 Bytes (41%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 39120 Bytes (47%) des dynamischen Speichers, 42800 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  lwIP Variant: "v2 Lower memory"
  Arduino 1.8.5, esp8266-2.4.1 - ESP_Gaszaehler_V1_86_Diagramme - Diagramm Jahre hinzugefügt
  ------------------------------------------------------------------------------------------

  Der Sketch verwendet 441828 Bytes (42%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41376 Bytes (50%) des dynamischen Speichers, 40544 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  lwIP Variant: "v1.4 Open Source"
  Arduino 1.8.5, esp8266-2.4.0 - ESP_Gaszaehler_V1_85_Log - Speicherung Logs jetzt ab 5 bis 15 Sekunden vor Mitternacht
  ---------------------------------------------------------------------------------------------------------------------

  Der Sketch verwendet 414768 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41856 Bytes (51%) des dynamischen Speichers, 40064 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Arduino 1.8.5 - ESP_Gaszaehler_V1_84_DST - Nach Umschaltung Sommerzeit Uhrzeit in RTC speichern
  -----------------------------------------------------------------------------------------------

  Der Sketch verwendet 414768 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41872 Bytes (51%) des dynamischen Speichers, 40048 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Arduino 1.8.4 - ESP_Gaszaehler_V1_82_Debounce

  Der Sketch verwendet 414760 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41856 Bytes (51%) des dynamischen Speichers, 40064 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 414744 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41856 Bytes (51%) des dynamischen Speichers, 40064 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 414740 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41852 Bytes (51%) des dynamischen Speichers, 40068 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Arduino 1.8.3 - ESP_Gaszaehler_V1_82_Debounce (neue Debounce-Routinen, GZ16 zählte immer noch zu viele Impulse)

  Der Sketch verwendet 414688 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41848 Bytes (51%) des dynamischen Speichers, 40072 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Arduino 1.8.3 - ESP_Gaszaehler_V1_81_MQTT
  Der Sketch verwendet 414444 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 42020 Bytes (51%) des dynamischen Speichers, 39900 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Arduino 1.8.3 - ESP_Gaszaehler_V1_80_UBatt
  Der Sketch verwendet 414344 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41864 Bytes (51%) des dynamischen Speichers, 40056 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Arduino 1.8.3 - ESP_Gaszaehler_V1_79_S0-Count (Variablen die in Interrupts verwendet werden als "volatile" deklariert.
                  GZ16 zählte immer noch zu viele Impulse)
  Der Sketch verwendet 414024 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41864 Bytes (51%) des dynamischen Speichers, 40056 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Arduino 1.8.2 - ESP_Gaszaehler_V1_78_S0-Count (Debounce Zeit für S0-Interrupt von 200 mS auf 3000 mS erhöht,
                  GZ16 zählte zu viele Impulse)
  Der Sketch verwendet 414000 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41848 Bytes (51%) des dynamischen Speichers, 40072 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Change: Taster und S0-Eingang Abfrage in Interrupt-Abfrage, Bounce2 entfällt
  Arduino 1.8.2 - ESP_Gaszaehler_V1_76_Interupt
  Der Sketch verwendet 414140 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41844 Bytes (51%) des dynamischen Speichers, 40076 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Arduino 1.8.2 - ESP_Gaszaehler_V1_75_Statistik
  Der Sketch verwendet 414140 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41844 Bytes (51%) des dynamischen Speichers, 40076 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 414060 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41844 Bytes (51%) des dynamischen Speichers, 40076 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Arduino 1.8.2 - ESP_Gaszaehler_V1_74_Diagramm
  Der Sketch verwendet 414004 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41844 Bytes (51%) des dynamischen Speichers, 40076 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 414012 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41844 Bytes (51%) des dynamischen Speichers, 40076 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 414044 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41892 Bytes (51%) des dynamischen Speichers, 40028 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 413940 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41876 Bytes (51%) des dynamischen Speichers, 40044 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 413860 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41828 Bytes (51%) des dynamischen Speichers, 40092 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 413884 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41844 Bytes (51%) des dynamischen Speichers, 40076 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Arduino 1.8.2 - ESP_Gaszaehler_V1_72_Diagramm
  Der Sketch verwendet 413660 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41844 Bytes (51%) des dynamischen Speichers, 40076 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 413756 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41844 Bytes (51%) des dynamischen Speichers, 40076 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Arduino 1.8.2 - ESP_Gaszaehler_V1_71_Statistik (WebsiteStatistikJahreszeiten - if/then)
  Der Sketch verwendet 413812 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41908 Bytes (51%) des dynamischen Speichers, 40012 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Arduino 1.8.2 - ESP_Gaszaehler_V1_71_Statistik (WebsiteStatistikJahreszeiten - switch/case)

  Der Sketch verwendet 413660 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41892 Bytes (51%) des dynamischen Speichers, 40028 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Arduino 1.8.2 - ESP_Gaszaehler_V1_70_clear
  Der Sketch verwendet 415324 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41892 Bytes (51%) des dynamischen Speichers, 40028 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 414156 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41828 Bytes (51%) des dynamischen Speichers, 40092 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 414148 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41860 Bytes (51%) des dynamischen Speichers, 40060 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 414068 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41844 Bytes (51%) des dynamischen Speichers, 40076 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Arduino 1.8.2 - ESP_Gaszaehler_V1_69_MinMax
  Der Sketch verwendet 412044 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41828 Bytes (51%) des dynamischen Speichers, 40092 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 411164 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41828 Bytes (51%) des dynamischen Speichers, 40092 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 413252 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41812 Bytes (51%) des dynamischen Speichers, 40108 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  ESP_Gaszaehler_V1_64_HTTP-Cache, CSS in static
  Der Sketch verwendet 412548 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41748 Bytes (50%) des dynamischen Speichers, 40172 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  ESP_Gaszaehler_V1_63_MQTT
  Der Sketch verwendet 412988 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41748 Bytes (50%) des dynamischen Speichers, 40172 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  ESP_Gaszaehler_V1_61_S0-Count
  Der Sketch verwendet 412700 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41748 Bytes (50%) des dynamischen Speichers, 40172 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  ESP_Gaszaehler_V1_59_Systemlog
  Der Sketch verwendet 412540 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41732 Bytes (50%) des dynamischen Speichers, 40188 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  The Wire library has a buffer of 32 bytes!!!
  ESP_Gaszaehler_V1_58_I2C - Fehler bei Lesen und Schreiben von mehr als 32 Byte
  Der Sketch verwendet 412492 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41732 Bytes (50%) des dynamischen Speichers, 40188 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  ESP_Gaszaehler_V1_57_Taste
  Der Sketch verwendet 412060 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41732 Bytes (50%) des dynamischen Speichers, 40188 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  ESP_Gaszaehler_V1_56_DS1307
  Der Sketch verwendet 411852 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41732 Bytes (50%) des dynamischen Speichers, 40188 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Version 0.53 Optimierung
  Der Sketch verwendet 411844 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41732 Bytes (50%) des dynamischen Speichers, 40188 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Version 0.52 Diagramme und Logs
  Der Sketch verwendet 411892 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41732 Bytes (50%) des dynamischen Speichers, 40188 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 411812 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41732 Bytes (50%) des dynamischen Speichers, 40188 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Version 0.51 Diagramme und Logs
  Der Sketch verwendet 411884 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41716 Bytes (50%) des dynamischen Speichers, 40204 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Version 0.50 Log Jahr
  Der Sketch verwendet 411700 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41732 Bytes (50%) des dynamischen Speichers, 40188 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 411828 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41732 Bytes (50%) des dynamischen Speichers, 40188 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 411988 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41748 Bytes (50%) des dynamischen Speichers, 40172 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 411876 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41748 Bytes (50%) des dynamischen Speichers, 40172 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Version 0.49 Serial Output
  Der Sketch verwendet 411836 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41748 Bytes (50%) des dynamischen Speichers, 40172 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Version 0.48 NTP Settings
  Der Sketch verwendet 411652 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41748 Bytes (50%) des dynamischen Speichers, 40172 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 411828 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41748 Bytes (50%) des dynamischen Speichers, 40172 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 411348 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41748 Bytes (50%) des dynamischen Speichers, 40172 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 411348 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41748 Bytes (50%) des dynamischen Speichers, 40172 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 411292 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41764 Bytes (50%) des dynamischen Speichers, 40156 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Der Sketch verwendet 411244 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41844 Bytes (51%) des dynamischen Speichers, 40076 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  Version 0.47 Serial Output
  PubSubClient.h
  #define MQTT_KEEPALIVE 60
  E:\Eigene Dateien\Eigene Dokumente\Arduino\libraries
  Der Sketch verwendet 410420 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1044464 Bytes.
  Globale Variablen verwenden 41796 Bytes (51%) des dynamischen Speichers, 40124 Bytes für lokale Variablen verbleiben. Das Maximum sind 81920 Bytes.
  ##### ARDUINO Version 1.8.0 #####
  Der Sketch verwendet 410.420 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 41.796 Bytes (51%) des dynamischen Speichers, 40.124 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.46 MQTT - Unterbrechung nach 24h
  Der Sketch verwendet 410.388 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 41.812 Bytes (51%) des dynamischen Speichers, 40.108 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 410.540 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 41.796 Bytes (51%) des dynamischen Speichers, 40.124 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.45 Optimierung
  [SPIFFS] upload : e:\Temp\build8a6acc90625273d27dbc0ad522ca8b0a.spiffs/ESP_Gaszaehler_44_Download.spiffs.bin
  [SPIFFS] reset  : ck
  [SPIFFS] port   : COM3
  [SPIFFS] speed  : 115200
  [SPIFFS] address: 0x100000
  [SPIFFS] data   : N:\ESP8266\Arduino\ESP_Gaszaehler\ESP_Gaszaehler_44_Download\data
  [SPIFFS] size   : 3052
  [SPIFFS] page   : 256
  [SPIFFS] block  : 8192
  Der Sketch verwendet 410.620 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 41.812 Bytes (51%) des dynamischen Speichers, 40.108 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 410.476 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 41.812 Bytes (51%) des dynamischen Speichers, 40.108 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.44 Download unbekannter Dateien aus SPIFFS
  Der Sketch verwendet 414.220 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 42.148 Bytes (51%) des dynamischen Speichers, 39.772 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 414.248 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 42.216 Bytes (51%) des dynamischen Speichers, 39.704 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.41 Counter 1 wie Counter 2
  Der Sketch verwendet 414.112 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 42.232 Bytes (51%) des dynamischen Speichers, 39.688 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 414.176 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 42.232 Bytes (51%) des dynamischen Speichers, 39.688 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 414.208 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 42.232 Bytes (51%) des dynamischen Speichers, 39.688 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 413.936 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 42.488 Bytes (51%) des dynamischen Speichers, 39.432 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 413.968 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 42.488 Bytes (51%) des dynamischen Speichers, 39.432 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 414.096 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 42.520 Bytes (51%) des dynamischen Speichers, 39.400 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.40 Optimierung
  Der Sketch verwendet 414.072 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 42.512 Bytes (51%) des dynamischen Speichers, 39.408 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 414.136 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 42.512 Bytes (51%) des dynamischen Speichers, 39.408 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.39 Statistik Jahreszeiten
  Der Sketch verwendet 412.516 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 42.396 Bytes (51%) des dynamischen Speichers, 39.524 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.36 Count - Diagramme Monat und Tag
  Der Sketch verwendet 411.988 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 42.476 Bytes (51%) des dynamischen Speichers, 39.444 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 412.244 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 42.476 Bytes (51%) des dynamischen Speichers, 39.444 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.34 Count - Zählerstand einstellen
  Der Sketch verwendet 411.324 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 42.444 Bytes (51%) des dynamischen Speichers, 39.476 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.33 Count - Zählerstand (S0-Count_abs) im RAM PCF8583
  Der Sketch verwendet 410.308 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 42.348 Bytes (51%) des dynamischen Speichers, 39.572 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 412.308 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 42.316 Bytes (51%) des dynamischen Speichers, 39.604 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.32 Count - Zählerstand einstellen
  Der Sketch verwendet 409.860 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 42.300 Bytes (51%) des dynamischen Speichers, 39.620 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 409.796 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 42.348 Bytes (51%) des dynamischen Speichers, 39.572 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 411.092 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 42.348 Bytes (51%) des dynamischen Speichers, 39.572 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 411.684 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 43.660 Bytes (53%) des dynamischen Speichers, 38.260 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 413.996 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 44.412 Bytes (54%) des dynamischen Speichers, 37.508 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 413.284 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 44.396 Bytes (54%) des dynamischen Speichers, 37.524 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 409.412 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 44.236 Bytes (53%) des dynamischen Speichers, 37.684 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 409.104 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 44.176 Bytes (53%) des dynamischen Speichers, 37.744 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.31 MQTT
  Der Sketch verwendet 411.960 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 44.880 Bytes (54%) des dynamischen Speichers, 37.040 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 411.608 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 44.880 Bytes (54%) des dynamischen Speichers, 37.040 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 411.580 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 44.884 Bytes (54%) des dynamischen Speichers, 37.036 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 412.548 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 44.900 Bytes (54%) des dynamischen Speichers, 37.020 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 412.908 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 44.916 Bytes (54%) des dynamischen Speichers, 37.004 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 413.056 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 44.976 Bytes (54%) des dynamischen Speichers, 36.944 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.30 Optimierung EEPROM
  Der Sketch verwendet 413.168 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 44.976 Bytes (54%) des dynamischen Speichers, 36.944 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 413.152 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 44.976 Bytes (54%) des dynamischen Speichers, 36.944 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 412.832 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 44.976 Bytes (54%) des dynamischen Speichers, 36.944 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 411.508 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 44.932 Bytes (54%) des dynamischen Speichers, 36.988 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.29 Statistik
  Der Sketch verwendet 411.200 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 44.936 Bytes (54%) des dynamischen Speichers, 36.984 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 411.244 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 44.980 Bytes (54%) des dynamischen Speichers, 36.940 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 410.984 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 45.656 Bytes (55%) des dynamischen Speichers, 36.264 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 410.992 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 46.472 Bytes (56%) des dynamischen Speichers, 35.448 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 410.504 Bytes (39%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 46.312 Bytes (56%) des dynamischen Speichers, 35.608 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.28
  Der Sketch verwendet 359.163 Bytes (34%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 44.960 Bytes (54%) des dynamischen Speichers, 36.960 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.27
  Der Sketch verwendet 359.019 Bytes (34%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 45.296 Bytes (55%) des dynamischen Speichers, 36.624 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 358.083 Bytes (34%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 45.368 Bytes (55%) des dynamischen Speichers, 36.552 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 357.563 Bytes (34%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 45.512 Bytes (55%) des dynamischen Speichers, 36.408 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.26
  Der Sketch verwendet 354.819 Bytes (33%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 45.560 Bytes (55%) des dynamischen Speichers, 36.360 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Einstellungen Zeit anzeigen optimiert
  Der Sketch verwendet 354.771 Bytes (33%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 47.192 Bytes (57%) des dynamischen Speichers, 34.728 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Systeminfo anzeigen optimiert
  Der Sketch verwendet 354.579 Bytes (33%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 48.552 Bytes (59%) des dynamischen Speichers, 33.368 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Dateien anzeigen optimiert
  Der Sketch verwendet 354.451 Bytes (33%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 48.888 Bytes (59%) des dynamischen Speichers, 33.032 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Logs optimiert
  Der Sketch verwendet 353.579 Bytes (33%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 49.400 Bytes (60%) des dynamischen Speichers, 32.520 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Diagramm Tag optimiert
  Der Sketch verwendet 353.219 Bytes (33%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 50.056 Bytes (61%) des dynamischen Speichers, 31.864 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Diagramm Monat optimiert
  Der Sketch verwendet 352.419 Bytes (33%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 50.296 Bytes (61%) des dynamischen Speichers, 31.624 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Diagramm Jahr optimiert
  Der Sketch verwendet 351.443 Bytes (33%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 50.392 Bytes (61%) des dynamischen Speichers, 31.528 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 351.315 Bytes (33%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 50.776 Bytes (61%) des dynamischen Speichers, 31.144 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.24
  Der Sketch verwendet 350.131 Bytes (33%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 51.064 Bytes (62%) des dynamischen Speichers, 30.856 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.23
  Der Sketch verwendet 352.627 Bytes (33%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 52.008 Bytes (63%) des dynamischen Speichers, 29.912 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 350.807 Bytes (33%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 52.188 Bytes (63%) des dynamischen Speichers, 29.732 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 350.419 Bytes (33%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 52.616 Bytes (64%) des dynamischen Speichers, 29.304 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.20
  Der Sketch verwendet 348.827 Bytes (33%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 53.432 Bytes (65%) des dynamischen Speichers, 28.488 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 348.163 Bytes (33%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 54.072 Bytes (66%) des dynamischen Speichers, 27.848 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 348.091 Bytes (33%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 55.160 Bytes (67%) des dynamischen Speichers, 26.760 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Optimierung Strings aus Flash (F("xxxxx"))
  Der Sketch verwendet 346.299 Bytes (33%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 57.256 Bytes (69%) des dynamischen Speichers, 24.664 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 346.395 Bytes (33%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 57.208 Bytes (69%) des dynamischen Speichers, 24.712 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.20
  Der Sketch verwendet 343.071 Bytes (32%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 55.540 Bytes (67%) des dynamischen Speichers, 26.380 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 343.023 Bytes (32%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 55.492 Bytes (67%) des dynamischen Speichers, 26.428 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 343.131 Bytes (32%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 55.736 Bytes (68%) des dynamischen Speichers, 26.184 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.19
  Der Sketch verwendet 342.395 Bytes (32%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 54.984 Bytes (67%) des dynamischen Speichers, 26.936 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.18
  Der Sketch verwendet 342.627 Bytes (32%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 55.120 Bytes (67%) des dynamischen Speichers, 26.800 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Version 0.17
  Der Sketch verwendet 343.923 Bytes (32%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 55.968 Bytes (68%) des dynamischen Speichers, 25.952 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 344.027 Bytes (32%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 56.032 Bytes (68%) des dynamischen Speichers, 25.888 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 344.067 Bytes (32%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 56.128 Bytes (68%) des dynamischen Speichers, 25.792 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 344.299 Bytes (32%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 56.272 Bytes (68%) des dynamischen Speichers, 25.648 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 339.907 Bytes (32%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 54.288 Bytes (66%) des dynamischen Speichers, 27.632 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Der Sketch verwendet 340.123 Bytes (32%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 54.176 Bytes (66%) des dynamischen Speichers, 27.744 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
  Diagramm Jahr eingefügt
  Der Sketch verwendet 338.259 Bytes (32%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 54.208 Bytes (66%) des dynamischen Speichers, 27.712 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.

  unsigned int adcVdd[480]; entfernt
  Der Sketch verwendet 338.083 Bytes (32%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 54.112 Bytes (66%) des dynamischen Speichers, 27.808 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.

  Der Sketch verwendet 338.179 Bytes (32%) des Programmspeicherplatzes. Das Maximum sind 1.044.464 Bytes.
  Globale Variablen verwenden 56.048 Bytes (68%) des dynamischen Speichers, 25.872 Bytes für lokale Variablen verbleiben. Das Maximum sind 81.920 Bytes.
*/
