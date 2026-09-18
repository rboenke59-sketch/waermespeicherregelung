#include "temperatursens.h"
#include "displayhandling.h"

#include "eestore.h"

namespace TEMPSENS
{
  OneWire oneWire(ONE_WIRE_BUS);
  DallasTemperature sensors(&oneWire);

  DeviceAddress sensorAddress[MAX_SENSORS];
  float sensorTemps[MAX_SENSORS]; // [°C]
  const float sensorDefekt = 155.f;//von 99,9 auf 155 erhöht!!

  int sensorCount = 0;

// Adresse sensor1:  0x28, 0xFF, 0x64, 0x0E, 0x6D, 0x46, 0xFA, 0xDD
// Adresse sensor2:  0x28, 0xFF, 0x64, 0x0E, 0x6D, 0xDC, 0x49, 0x29 //Biogaspumpsteuerung
// Adresse sensor3:  0x28, 0xCC, 0xC4, 0x75, 0xD0, 0x01, 0x3C, 0x28
// Adresse sensor4:  0x28, 0x1F, 0x8F, 0x76, 0xE0, 0x01, 0x3C, 0x7A
// Adresse sensor5:  0x28, 0xFF, 0x64, 0x1F, 0x79, 0xD6, 0x3B, 0xE9
// Adresse sensor6:  0x28, 0x21, 0x9F, 0x76, 0xE0, 0x01, 0x3C, 0xF8
// Adresse sensor7:  0x28, 0xFF, 0x64, 0x0E, 0x6D, 0x4D, 0xA2, 0xE7
// Adresse sensor8:  0x28, 0xFF, 0x64, 0x1F, 0x79, 0x94, 0xCA, 0xC9
// Adresse sensor9:  0x28, 0xFF, 0x64, 0x1F, 0x79, 0xFC, 0xE7, 0x3B
// Adresse sensor10: 0x28, 0xBC, 0x97, 0x75, 0xD0, 0x01, 0x3C, 0x6B
// Adresse sensor11: 0x28, 0xD9, 0x5C, 0x75, 0xD0, 0x01, 0x3C, 0x11
// Adresse sensorREF:0x28, 0xFF, 0x64, 0x0E, 0x6D, 0x20, 0x73, 0x3D
// Adresse sensor20: 0x28, 0x37, 0x11, 0x70, 0x3C, 0x19, 0x01, 0x0A
// Adresse sensor21: 0x28, 0x2C, 0xB2, 0x6E, 0x3C, 0x19, 0x01, 0xF9
// Adresse sensor22: 0x28, 0x82, 0xB9, 0x76, 0x3C, 0x19, 0x01, 0xE2
// Adresse sensor23: 0x28, 0xA5, 0xDC, 0x68, 0x3C, 0x19, 0x01, 0x3F
// Adresse sensor24: 0x28, 0xF6, 0x46, 0x70, 0x3C, 0x19, 0x01, 0x66
// Adresse sensor25: 0x28, 0x1D, 0xFE, 0x6A, 0x3C, 0x19, 0x01, 0xE5
// Adresse sensor11:
  /* Tabelle Funktion zu Index
   * Idx | Funktion
   * 0   | BHKW_VL    
   * 1   | BHKW_RL    
   * 2   | Haupt_VL   
   * 3   | Haupt_RL   
   * 4   | Hackschn_VL
   * 5   | Hackschn_RL
   * 6   | Tank_Oben  
   * 7   | Tank_Unten 
   * 8   | BGA_VL     
   * 9   | BGA_RL     
   */
  TempStruct t[MAX_SENSORS] = {
      // BHKW_VL       = 0,
      {-1,                       // index des dynamisch gefundenen sensors
       {0, 1, 2, 3, 4, 5, 6, 7}, // DS18B20 adresse
       0.f, 67.f},
      // BHKW_RL       = 1,
      {-1,                       // index des dynamisch gefundenen sensors
       {0, 1, 2, 3, 4, 5, 6, 7}, // DS18B20 adresse
       0.f, 62.f},
      // Haupt_VL      = 2,
      {-1,                       // index des dynamisch gefundenen sensors
       {0, 1, 2, 3, 4, 5, 6, 7}, // DS18B20 adresse
       0.f, 62.f},
      // Haupt_RL      = 3,
      {-1,                       // index des dynamisch gefundenen sensors
       {0, 1, 2, 3, 4, 5, 6, 7}, // DS18B20 adresse
       0.f, 58.f},
      // Hackschn_VL   = 4,
      {-1,                       // index des dynamisch gefundenen sensors
       {0, 1, 2, 3, 4, 5, 6, 7}, // DS18B20 adresse
       0.f, 68.f},
      // Hackschn_RL   = 5,
      {-1,                       // index des dynamisch gefundenen sensors
       {0, 1, 2, 3, 4, 5, 6, 7}, // DS18B20 adresse
       0.f, 58.f},
      // Tank_Oben     = 6,
      {-1,                       // index des dynamisch gefundenen sensors
       {0, 1, 2, 3, 4, 5, 6, 7}, // DS18B20 adresse
       0.f, 66.f},
      // Tank_Unten    = 7,
      {-1,                       // index des dynamisch gefundenen sensors
       {0, 1, 2, 3, 4, 5, 6, 7}, // DS18B20 adresse
       0.f, 64.f},
      // BGA_VL        = 8,
      {-1,                       // index des dynamisch gefundenen sensors
       {0, 1, 2, 3, 4, 5, 6, 7}, // DS18B20 adresse
       0.f, 68.f},
      // BGA_RL        = 9
      {-1,                       // index des dynamisch gefundenen sensors
       {0, 1, 2, 3, 4, 5, 6, 7}, // DS18B20 adresse
       0.f, 64.f}};

  float getTemp(uint8_t idx)
  {
    if (idx < 0 || idx >= MAX_SENSORS) {
      return 0.f;
    } else if (t[idx].simActive) {
      return t[idx].simTemp;
    } else  {
      return t[idx].temp;
    }
  }

  // Funktion zum Vergleichen der DeviceAddresses. Gibt true zurueck wenn addressen identisch
  bool deviceAddressesAreIdentical(DeviceAddress addr1, DeviceAddress addr2)
  {
    for (int i = 0; i < 8; i++)
    {
      if (addr1[i] != addr2[i])
      {
        return false;
      }
    }
    return true;
  }
  void copyAddress(uint8_t targetIdx, uint8_t sourceIdx)
  {
    if (sourceIdx < 0 || (sourceIdx >= MAX_SENSORS && sourceIdx != 0xFF)) { // 0xFF ist addresse zuruecksetzen
      Serial.print(F("ungueltiger Quellindex: "));
      Serial.println(sourceIdx);
      return;
    } else if (targetIdx < 0 || targetIdx >= MAX_SENSORS) {
      Serial.print(F("ungueltiger Zielindex: "));
      Serial.println(targetIdx);
      return;
    }
    if (sourceIdx == 0xFF) { // addresse zuruecksetzen
      for (int i = 0; i < 8; i++) {
        t[targetIdx].speicherAddr[i] = 0xFF;
      }
      Serial.print(F("funktion "));
      Serial.print(targetIdx);
      Serial.println(F(" wurde addresse zurueckgesetzt"));
    } else { // addresse uebernehmen
      for (int i = 0; i < 8; i++) {
        t[targetIdx].speicherAddr[i] = sensorAddress[sourceIdx][i];
      }
      Serial.print(F("funktion "));
      Serial.print(targetIdx);
      Serial.print(F(" wurde addresse aus Sensorindex "));
      Serial.print(sourceIdx);
      Serial.print(F(" zugewiesen mit addresse: "));
      Serial.println(TEMPSENS::addr2txt(TEMPSENS::sensorAddress[sourceIdx]));
    }
  }

  void copyAddress(DeviceAddress &target, DeviceAddress source)
  {
    for (int i = 0; i < 8; i++)
    {
      target[i] = source[i];
    }
  }

  String addr2txt(DeviceAddress addr)
  {
    String addrTxt = "";
    for (uint8_t i = 0; i < 8; i++)
    {
      addrTxt += String(addr[i], HEX) + " ";
    }
    return addrTxt;
  }

  void loadAddressesFromEeprom()
  {
    byte tempAddr[8];
    bool isDefault = true;
    for (int i = 0; i < MAX_SENSORS; i++)
    {
      isDefault = true;
      for (int j = 0; j < 8; j++)
      {
        tempAddr[j] = EEStore::roms[i][j];
        if (tempAddr[j] != 0xFF)
        { // default wert bei uninitialisiertem EEPROM = 0xFF
          isDefault = false;
        }
      }

      Serial.print(F("Sensor "));
      Serial.print(i);
      if (!isDefault)
      { // Adresse nicht 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF ?
        Serial.println(F(" hat gueltige Adresse im EEPROM. Wird uebernommen"));
        for (int j = 0; j < 8; j++)
        {
          t[i].speicherAddr[j] = tempAddr[j];
        }
      } else {
        Serial.println(F(" verwendet Initialadresse"));
      }
    }
  }

  void printDeviceAddresses()
  {
    for (int i = 0; i < MAX_SENSORS; i++)
    {
         Serial.print(F("Adresse Sensor ["));
         Serial.print(i);
         Serial.print(F("]: "));
         Serial.println( addr2txt(t[i].speicherAddr) );
    }
  }

  void setup()
  {
    loadAddressesFromEeprom(); // adressen aus eeprom laden
    printDeviceAddresses();

    sensors.begin();

    Serial.println(F("Suche verbundene DS18B20-Sensoren..."));
    sensorCount = sensors.getDeviceCount();
    if (sensorCount > MAX_SENSORS)
      sensorCount = MAX_SENSORS;

    for (int i = 0; i < sensorCount; i++)
    {
      if (sensors.getAddress(sensorAddress[i], i))
      {
        sensors.setResolution(sensorAddress[i], 12);
        Serial.print(F("Sensor #"));
        Serial.print(i);
        Serial.println(F(" gefunden"));
      }
      else
      {
        Serial.print(F("Konnte Adresse von Sensor #"));
        Serial.print(i);
        Serial.println(F(" nicht lesen"));
      }
    }

    // gespeicherte sensoradressen wurden gerade gefunden ?
    for (int i = 0; i < MAX_SENSORS; i++)
    { // ueber alle gespeicherten / benoetigten sensoren
      for (int j = 0; j < MAX_SENSORS; j++)
      { // ueber alle dynamisch gefundene sensoren
        if (deviceAddressesAreIdentical(t[i].speicherAddr, sensorAddress[j]))
        {               // gespeicherte addresse passt zu verbundenem sensor ?
          t[i].idx = j; // index merken
        }
      }
    }

    // pruefen ob alle benoetigten sensoren gefunden wurden
    String errMsg = "Sensor nicht gefunden:";
    uint8_t errCnt = 0;
    for (int i = 0; i < MAX_SENSORS; i++)
    {
      if (t[i].idx == -1)
      {
        errMsg += String(i) + ",";
        errCnt++;
      }
    }
    if (errCnt > 0)
    { // wenn fehler vorhanden dann auf display anzeigen
      errMsg.toCharArray(DISP::infotext, sizeof(DISP::infotext));
      Serial.println(DISP::infotext);
    }
  }

  void loop()
  {
    uint32_t now = millis();
  static uint32_t msLastReadTemp = 0;

  if (now - msLastReadTemp > 5000 ) {
    msLastReadTemp = now;
    sensors.requestTemperatures();
    for (int i = 0; i < sensorCount; i++) {
      sensorTemps[i] = sensors.getTempC(sensorAddress[i]);
    }

    for (int i = 0; i < MAX_SENSORS; i++) {
      if (t[i].idx  > -1) { // gueltiger index hinterlegt? d.h. hinterlegter sensor wurde angeschlossen und gefunden?
       t[i].temp = sensorTemps[t[i].idx];
      } else {
        t[i].temp = 99.9f;
      }
    }
  }
  }
}