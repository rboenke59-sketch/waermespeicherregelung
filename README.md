# Waermespeicherregelung

Enthaelt
- arduino projekt
- nextion display projekt

## Funktionen

Dieser Sketch dient dazu den Wärmehaushalt:
BGA,Pufferspeichher,Hackschnitzelheizung besser zu Menagen, dadurch Energie sparen!!
Schaltaktoren sind
- zwei Motorklappen (einmal BGA absperren, einmal Hauptringleitung absperren)
- eine Zirkulationspumpe in Hauptringleitung

Verbaute Elemente sind u.a.:
- Mega pro mini (Werkzeug: Arduino Mega ADK!!)
- Nextion 7" 
- DS18B20-Sensoren

Funktionen:
- 10x DS18B20 Temperatursensoren auslesen und mit hinterlegten Adressen (EEPROM) den Temperaturstellen zuordnen
- 1x Motorklappe auf/zu ansteuern (Richtung BGA)
- 1x Motorklappe prozentual ueber Laufzeit ansteuern
- 1x Zirkulationspumpe ein-/ausschalten
