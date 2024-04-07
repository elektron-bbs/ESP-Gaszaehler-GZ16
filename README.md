# GZ16

Der Gasz�hler GZ16 dient zur bequemen Fernerfassung und Auswertung des Gasverbrauches. Die meisten mechanischen Gasz�hler besitzen bereits ab Werk einen Impulsmagneten in der letzten Stelle des Rollenz�hlwerkes und k�nnen einfach mit einem Impulsnehmer ausgestattet werden. Ob der Gasz�hler daf�r geeignet ist, erkennt man an einem Aufdruck wie z.B. "1 Imp ? 0,01 m�" auf dem Typenschild. Der GZ16 wertet die Impulse des Impulsnehmers aus und stellt sie auf einer Weboberfl�che im Netzwerk dar. Die Verbindung zum Netzwerk wird �ber WLAN hergestellt.

## Hardware

Es wurden bisher zwei Hardware-Version in Betrieb gesetzt:

Version 1: ESP8266 ESP-12-E, DS1307 und USB-UART-Modul CP2102

Version 2: ESP8266 NodeMCU V3 und "Tiny RTC I2C Modul"

Stromlaufpl�ne und Dokumentationen dazu befinden sich im Ordner /data/pdf

## Installation

Das Kompilieren und Flashen der Firmware erfolgt in der Arduino-IDE nachdem die Sourcen f�r den ESP82666 eingebunden wurden.
Als Board wird "NodeMCU 1.0 (ESP-12E Module) und Flash-Size "4MB (FS:3MB OTA:~512KB)" ausgew�hlt. Im Ordner /data befinden sich Dokumentationen, Pl�ne und statische HTML-Seiten der Weboberfl�che des GZ16. Diese m�ssen mittels "ESP8266 Sketch Data Upload" auf den ESP8266 hochgeladen werden.

## Setup

Bei der ersten Inbetriebnahme versucht der GZ16 zuerst eine Verbindung mittels WPS (WiFi Protected Setup) �ber WLAN mit einem Accesspoint aufzubauen. Die WPS-Push-Button-Methode sollte zu diesem Zeitpunkt im Router deshalb aktiviert werden. Der Verbindungsversuch wird dreimal durchgef�hrt. Dabei erfolgen jeweils Neustarts des GZ16, zu erkennen am gleichzeitigen Aufleuchten aller drei LED des Ger�tes. Dieser Vorgang kann mehrere Minuten dauern. War der Verbindungsaufbau erfolgreich, erkennbar am dauerhaften Leuchten der gr�nen LED, werden die SSID des Accesspoints und das Passwort dauerhaft gespeichert und es besteht jetzt eine Verbindung zum WLAN des Accesspoints. Die WPS-Funktion des Routers sollte aus Sicherheitsgr�nden jetzt wieder deaktiviert werden.

Ist keine Verbindung mittels WPS m�glich, startet anschlie�end automatisch der eingebaute Accesspoint. Dies wird durch Leuchten der gelben LED signalisiert. Jetzt kann eine Verbindung mittels geeignetem WLAN-f�higen Ger�tes, wie z.B. Notebook oder Smartphone direkt mit dem GZ16 aufgebaut werden. In der WLAN-�bersicht sollte der GZ16 mit einer SSID �hnlich "GZ16-ESP-IDIDID" erscheinen. Die Zeichenkette "IDIDID" steht dabei f�r eine eindeutige Ger�tekennung des GZ16. Die Verbindung ist ungesichert und sollte deshalb so kurz wie m�glich genutzt werden. Nach der erfolgreichen Verbindung ruft man im Internet-Browser die Adresse 192.168.4.1 auf. Es sollte die �bersichtsseite des GZ16 angezeigt werden. Unter den Men�punkten "Einstellungen" und danach "Netzwerk" k�nnen jetzt die Daten f�r das heimische Netzwerk eingegeben werden. Durch Bet�tigen des Schalters "Verbinden" wird versucht, sich mit dem gew�hlten Netzwerk zu verbinden. Den Erfolg der Verbindung quittiert der GZ16 mit dauerhaften Leuchten der gr�nen LED. Der GZ16 sollte anschlie�end neu gestartet werden, um den Accesspoint-Modus zu beenden.
