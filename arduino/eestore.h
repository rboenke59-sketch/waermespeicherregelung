#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <OneWire.h>
#include "temperatursens.h"   // liefert MAX_SENSORS

namespace EEStore {

// ---- Datentypen ----
struct AppParams {
  uint16_t tempDiffPuAus_x10    = 20;   // [1/10°C] temperaturdifferenz gaertnerei pumpe aus
  uint16_t bhkw_vl_zu_x10       = 450;  // [1/10°C] bei unterschreitung BHKW abschiebern
  uint16_t nachlaufMs           = 1000; // dummy
  uint8_t  zielKlappeBGAzu      = 5;    // [%] zielwert position klappe bei unterschreitung
  uint8_t  zielKlappeGaertnereiZu = 5;    // [%] zielwert position klappe bei unterschreitung
  uint8_t  reserve[2]    = {0};
};

// ---- Globale Daten im Namespace ----
extern AppParams params;
extern uint8_t   roms[MAX_SENSORS][8]; // DS18B20 adressen
extern uint8_t   romCount;

// ---- API ----
bool setup();          // lädt aus EEPROM, legt Defaults an wenn ungültig
bool save();           // speichert params + roms atomar
bool saveParams();     // nur params
bool saveSensors();    // nur roms/romCount

} // namespace EEStore
