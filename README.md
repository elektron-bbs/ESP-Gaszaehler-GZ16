# GZ16

Der Gaszähler GZ16 dient zur bequemen Fernerfassung und Auswertung des Gasverbrauches. Die meisten mechanischen Gaszähler besitzen bereits ab Werk einen Impulsmagneten in der letzten Stelle des Rollenzählwerkes und können einfach mit einem Impulsnehmer ausgestattet werden. Ob der Gaszähler dafür geeignet ist, erkennt man an einem Aufdruck wie z.B. "1 Imp ? 0,01 m³" auf dem Typenschild. Der GZ16 wertet die Impulse des Impulsnehmers aus und stellt sie auf einer Weboberfläche im Netzwerk dar. Die Verbindung zum Netzwerk wird über WLAN hergestellt.

## Hardware

Es wurden bisher zwei Hardware-Version in Betrieb gesetzt:

Version 1: ESP8266 ESP-12-E, DS1307 und USB-UART-Modul CP2102

Version 2: ESP8266 NodeMCU V3 und "Tiny RTC I2C Modul"

Stromlaufpläne und Dokumentationen dazu befinden sich im Ordner /data/pdf

## Installation

Das Kompilieren und Flashen der Firmware erfolgt in der Arduino-IDE nachdem die Sourcen für den ESP82666 eingebunden wurden.
Als Board wird "NodeMCU 1.0 (ESP-12E Module) und Flash-Size "4MB (FS:3MB OTA:~512KB)" ausgewählt. Im Ordner /data befinden sich Dokumentationen, Pläne und statische HTML-Seiten der Weboberfläche des GZ16. Diese müssen mittels "ESP8266 Sketch Data Upload" auf den ESP8266 hochgeladen werden.

## Setup

Bei der ersten Inbetriebnahme versucht der GZ16 zuerst eine Verbindung mittels WPS (WiFi Protected Setup) über WLAN mit einem Accesspoint aufzubauen. Die WPS-Push-Button-Methode sollte zu diesem Zeitpunkt im Router deshalb aktiviert werden. Der Verbindungsversuch wird dreimal durchgeführt. Dabei erfolgen jeweils Neustarts des GZ16, zu erkennen am gleichzeitigen Aufleuchten aller drei LED des Gerätes. Dieser Vorgang kann mehrere Minuten dauern. War der Verbindungsaufbau erfolgreich, erkennbar am dauerhaften Leuchten der grünen LED, werden die SSID des Accesspoints und das Passwort dauerhaft gespeichert und es besteht jetzt eine Verbindung zum WLAN des Accesspoints. Die WPS-Funktion des Routers sollte aus Sicherheitsgründen jetzt wieder deaktiviert werden.

Ist keine Verbindung mittels WPS möglich, startet anschließend automatisch der eingebaute Accesspoint. Dies wird durch Leuchten der gelben LED signalisiert. Jetzt kann eine Verbindung mittels geeignetem WLAN-fähigen Gerätes, wie z.B. Notebook oder Smartphone direkt mit dem GZ16 aufgebaut werden. In der WLAN-Übersicht sollte der GZ16 mit einer SSID ähnlich "GZ16-ESP-IDIDID" erscheinen. Die Zeichenkette "IDIDID" steht dabei für eine eindeutige Gerätekennung des GZ16. Die Verbindung ist ungesichert und sollte deshalb so kurz wie möglich genutzt werden. Nach der erfolgreichen Verbindung ruft man im Internet-Browser die Adresse 192.168.4.1 auf. Es sollte die Übersichtsseite des GZ16 angezeigt werden. Unter den Menüpunkten "Einstellungen" und danach "Netzwerk" können jetzt die Daten für das heimische Netzwerk eingegeben werden. Durch Betätigen des Schalters "Verbinden" wird versucht, sich mit dem gewählten Netzwerk zu verbinden. Den Erfolg der Verbindung quittiert der GZ16 mit dauerhaften Leuchten der grünen LED. Der GZ16 sollte anschließend neu gestartet werden, um den Accesspoint-Modus zu beenden.
