// Dieser Sketch dient dazu den Wärmehaushalt:
// BGA,Pufferspeichher,Hackschnitzelheizung besser zu Menagen, dadurch Energie sparen!!
// Schaltaktoren sind
// - zwei Motorklappen (einmal BGA absperren, einmal Hauptringleitung absperren)
// - eine Zirkulationspumpe in Hauptringleitung
// Verbaute Elemente sind u.a.:
// - Mega pro mini (Werkzeug: Arduino Mega ADK!!)
// - Nextion 7" 
// - DS18B20-Sensoren
// VER.2 !!
// Funktionen:
// - 10x DS18B20 Temperatursensoren auslesen und mit hinterlegten Adressen (EEPROM) den Temperaturstellen zuordnen
// - 1x Motorklappe auf/zu ansteuern (Richtung BGA)
// - 1x Motorklappe prozentual ueber Laufzeit ansteuern
// - 1x Zirkulationspumpe ein-/ausschalten
// Pinbelegung:
// 2     - Temperatursensoren DS18B20
// 16,17 - Serial2 fuer Nextion
// 49    - Motorklappe BGA Endlage AUF
// 48    - Motorklappe BGA Endlage ZU
// 47    - Motorklappe Gaertnerei Endlage AUF
// 46    - Motorklappe Gaertnerei Endlage ZU
// 45    - Motorklappe BGA AUF
// 44    - Motorklappe BGA ZU
// 43    - Motorklappe Gaertnerei AUF
// 42    - Motorklappe Gaertnerei ZU
// 41    - Zirkulationspumpe AN

#include <Arduino.h>
#include <OneWire.h>

#include "displayhandling.h"
#include "analogsens.h"
#include "temperatursens.h"
#include "aktoren.h"
#include "eestore.h"
#include "logik.h"

void setup() 
{
  Serial.begin(9600);      // PC

  Serial.println(F("V2026.09.18")); // Datum / Version als Wiedererkennung ausgeben

  DISP::setup();           // Nextion Display (Pins 16=RX2, 17=TX2)

  EEStore::setup();   // lädt Params + Sensor-ROMs oder legt Defaults an

  ANASENS::setup();  // PT100 sensor
  TEMPSENS::setup(); // DS18B20 sensoren

  AKTOREN::setup(); // Motorklappen und pumpe
}

void loop() 
{
  static uint32_t msLastPrint = 0;
  uint32_t msNow = millis();

  DISP::loop(); // daten vom display senden und empfangen
  ANASENS::loop(); // temperaturen lesen und auswerten
  TEMPSENS::loop(); // temperaturen lesen und auswerten

  LOGIK::loop();

  AKTOREN::loop(); // Motorklappen und pumpe

  if (msNow - msLastPrint > 4000) {
    msLastPrint = msNow;
    // Serial.print(F("status pumpe = "));
    // Serial.println(AKTOREN::getStatePumpe());
  }
}
 