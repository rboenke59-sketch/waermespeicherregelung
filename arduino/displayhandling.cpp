#include "displayhandling.h"

#include "EasyNextionLibrary.h"
#include "temperatursens.h"
#include "logik.h"
#include "aktoren.h"
#include "eestore.h"

// page2 "Simulation EIN/AUS"
void trigger0(){ // printh 23 02 54 00
  bool sim = LOGIK::toogleSimulation();
  for (uint8_t i = 0; i < MAX_SENSORS; i++) {
    TEMPSENS::t[i].simActive = sim;
  }
  AKTOREN::klappeBGA.setSim(sim);
  AKTOREN::klappeGaertnerei.setSim(sim);

  DISP::sendPage2(); // ganze seite neu schicken weil es gibt keine dynamischen werte sondern nur parameter
}

// page2 "werte updaten" f. simulation
void trigger1(){ // printh 23 02 54 01
  DISP::getPage2SimValues();
  DISP::sendPage2(); // ganze seite neu schicken weil es gibt keine dynamischen werte sondern nur parameter
}

// page2 "parameter speichern"
void trigger2(){ // printh 23 02 54 02
  DISP::getPage2ParamValues();
  DISP::sendPage2(); // ganze seite neu schicken weil es gibt keine dynamischen werte sondern nur parameter
}

// page3 "Zuordnung speichern"
void trigger16(){ // printh 23 02 54 10
  // Serial.println(F("trigger16: sensorzuordnung"));
  DISP::sensorZuordnungVonNextionAuswerten();
  
}

namespace DISP {

EasyNex myNex(Serial2); // Create an object of EasyNex class with the name < myNex >
                      // Set as parameter the Hardware Serial you are going to use

char infotext[35] = ""; // flexibler anzeigetext auf page1

// x0 ... x9 auf page2
void writeSimTemp(uint8_t idx)
{
  int32_t temp = TEMPSENS::t[idx].simTemp * 10;
  myNex.writeNum("x" + String(idx) + ".val", temp); // Write it to x0 Xfloat component on Nextion
}

void sendPage2()
{
  for (uint8_t i = 0; i < MAX_SENSORS; i++) {
    writeSimTemp(i);
  }
  myNex.writeNum(F("b2.val"), LOGIK::getSimState());
  myNex.writeNum("x10.val", EEStore::params.tempDiffPuAus_x10);
  myNex.writeNum("x11.val", EEStore::params.bhkw_vl_zu_x10);
  myNex.writeNum("x12.val", EEStore::params.zielKlappeBGAzu);
  myNex.writeNum("x13.val", EEStore::params.zielKlappeGaertnereiZu);
}

// eine einzelne sensoradresse (uebergeben per idx) ans display senden 
void sendPage3SensorAddr(uint8_t idx)
{
  myNex.writeStr("addr" + String(idx) + ".txt", TEMPSENS::addr2txt(TEMPSENS::sensorAddress[idx]));
}

// eine einzelne temperatur (uebergeben per idx) ans display senden 
void sendPage3SensorTemp(uint8_t idx)
{
  myNex.writeStr("t" + String( idx ) + ".txt", String( TEMPSENS::sensorTemps[idx], 1));
}

/*
 * eine einzelne combobox (uebergeben per idx) ans display senden 
 * combobox index 0 ist text "-1"
 * combobox index 1 ist text "0"
 * ...
 * Deshalb temperatur index + 1 schicken
 */
void sendPage3Combobox(uint8_t idx)
{
  myNex.writeNum("cb" + String(idx) + ".val",  TEMPSENS::t[idx].idx + 1 );
}

void sendPage3()
{
  // 10 Sensoren => 0...9
  for (uint8_t i = 0; i < MAX_SENSORS; i++) {
    sendPage3SensorAddr(i); // addresse anzeigen
    sendPage3SensorTemp(i); // temperatur anzeigen
    sendPage3Combobox(i);
  }

  myNex.writeStr(F("tres.txt"), "");
}

void updatePage1()
{
  uint32_t msStart = millis();

  myNex.writeStr(F("tinf.txt"), infotext); // info text aktualisieren

  // textfelder so benannt dass sie zum sensorindex passen, daher einfach loop
  for (uint8_t i = 0; i < MAX_SENSORS; i++) { 
    myNex.writeStr("t" + String(i) + ".txt", String(TEMPSENS::getTemp(i), 1) );
  };

  //===========================================================================
  // Klappe BGA
  //===========================================================================
  myNex.writeStr(F("tBGA.txt"), String(AKTOREN::klappeBGA.getPosition()) + "%");
  // endlagen darstellung 
  if (AKTOREN::klappeBGA.getState() == Motorklappe::State::Auf) {
    myNex.writeNum(F("tBGAauf.bco"), COLOR_GREEN); 
    myNex.writeNum(F("tBGAzu.bco"), COLOR_WHITE);
  } else if (AKTOREN::klappeBGA.getState() == Motorklappe::State::Zu) {
    myNex.writeNum(F("tBGAauf.bco"), COLOR_WHITE); 
    myNex.writeNum(F("tBGAzu.bco"), COLOR_GREEN);
  } else {
    myNex.writeNum(F("tBGAauf.bco"), COLOR_WHITE); 
    myNex.writeNum(F("tBGAzu.bco"), COLOR_WHITE);
  }

  //===========================================================================
  // Klappe Gaertnerei
  //===========================================================================
  myNex.writeStr(F("tGa.txt"), String(AKTOREN::klappeGaertnerei.getPosition())+ "%");
   // endlagen darstellung 
  if (AKTOREN::klappeGaertnerei.getState() == Motorklappe::State::Auf) {
    myNex.writeNum(F("tGAauf.bco"), COLOR_GREEN); 
    myNex.writeNum(F("tGAzu.bco"), COLOR_WHITE);
  } else if (AKTOREN::klappeGaertnerei.getState() == Motorklappe::State::Zu) {
    myNex.writeNum(F("tGAauf.bco"), COLOR_WHITE); 
    myNex.writeNum(F("tGAzu.bco"), COLOR_GREEN);
  } else {
    myNex.writeNum(F("tGAauf.bco"), COLOR_WHITE); 
    myNex.writeNum(F("tGAzu.bco"), COLOR_WHITE);
  }

  if (AKTOREN::getStatePumpe() == HIGH) {
    myNex.writeNum(F("tP.bco"), COLOR_GREEN);
  } else {
    myNex.writeNum(F("tP.bco"), COLOR_WHITE);
  }

  // uint32_t msEnd = millis();
  // Serial.print(F("updatePage1 Dauer: "));
  // Serial.print(msEnd - msStart);
  // Serial.println(F(" ms"));
}

void updatePage2()
{
  myNex.writeNum(F("b2.val"), LOGIK::getSimState());
}

void updatePage3()
{
  // 10 Sensoren => 0...9
  for (uint8_t i = 0; i < MAX_SENSORS; i++) {
    sendPage3SensorTemp(i); // temperatur anzeigen
  }
}

/**
 * @param targetIdx - funktion
 * combobox index 0 ist text "-1"
 * combobox index 1 ist text "0"
 * ...
 * Deshalb temperatur index -1 abziehen
 */
bool getCbVal(uint8_t targetIdx) 
{
  uint32_t number = myNex.readNumber("cb" + String(targetIdx) + ".val"); 
  if(number != 777777){       // 777777 is the return value if the code fails to read the new value
    number--;
    TEMPSENS::copyAddress(targetIdx, number);
    return false;
  } else { // number == 777777 -> fehler
     Serial.print(F("combobox "));
     Serial.print(targetIdx);
     Serial.println(F("konnte nicht gelesen werden"));
     return true;
  }
}

void sensorZuordnungVonNextionAuswerten() { // nach buttonklick speicher auf page3
  bool err = false;
  err |= getCbVal(0);// BHKW_VL    
  err |= getCbVal(1);// BHKW_RL    
  err |= getCbVal(2);// Haupt_VL   
  err |= getCbVal(3);// Haupt_RL   
  err |= getCbVal(4);// Hackschn_VL
  err |= getCbVal(5);// Hackschn_RL
  err |= getCbVal(6);// Tank_Oben  
  err |= getCbVal(7);// Tank_Unten 
  err |= getCbVal(8);// BGA_VL     
  err |= getCbVal(9);// BGA_RL     

  if (err) {
    myNex.writeStr(F("tres.txt"), "Fehler");
  } else {
    EEStore::save();
    myNex.writeStr(F("tres.txt"), F("Speichern OK"));
  }
}

// x0 ... x9 auf page2
float readSimTemp(uint8_t idx)
{
  int32_t temp = myNex.readNumber("x" + String(idx) + ".val"); // Write it to x0 Xfloat component on Nextion
  return temp * 0.1f;
}

void getPage2ParamValues()
{
  int32_t val = myNex.readNumber(F("x10.val")); // AppParams.tempDiffPuAus_x10
  EEStore::params.tempDiffPuAus_x10 = (uint16_t)val;

  val = myNex.readNumber(F("x11.val")); // AppParams.bhkw_vl_zu_x10
  EEStore::params.bhkw_vl_zu_x10 = (uint16_t)val;

  val = myNex.readNumber(F("x12.val")); // AppParams.zielKlappeBGAzu
  EEStore::params.zielKlappeBGAzu = (uint8_t)val;

  val = myNex.readNumber(F("x13.val")); // AppParams.zielKlappeGaertnereiZu
  EEStore::params.zielKlappeGaertnereiZu = (uint8_t)val;

  EEStore::saveParams();

  Serial.println(F("Neue Parameter gespeichert."));
  Serial.print(F("tempDiffPuAus_x10 =  "));
  Serial.println(EEStore::params.tempDiffPuAus_x10);
  Serial.print(F("bhkw_vl_zu_x10 = "));
  Serial.println(EEStore::params.bhkw_vl_zu_x10);
  Serial.print(F("zielKlappeBGAzu = "));
  Serial.println(EEStore::params.zielKlappeBGAzu);
  Serial.print(F("zielKlappeGaertnereiZu = "));
  Serial.println(EEStore::params.zielKlappeGaertnereiZu);
}

void getPage2SimValues()
{
  for (uint8_t i = 0; i < MAX_SENSORS; i++) {
    TEMPSENS::t[i].simTemp = readSimTemp(i);
    Serial.println("simtemp " + String(i) + " = " + String(TEMPSENS::t[i].simTemp));
  }

}

// 🔹 Hilfsfunktion: Daten an Nextion senden (mit echtem Gradzeichen)
void sendToNextion(String field, float value) {
  Serial2.print(field);
  Serial2.print(".txt=\"");
  if (value == DEVICE_DISCONNECTED_C) {
    Serial2.print("Fehler");
  } else {
    Serial2.print(value, 1);
    Serial2.write(0xB0);  // ASCII-Gradzeichen
    Serial2.print("C");
  }
  Serial2.print("\"");
  Serial2.write(0xFF);
  Serial2.write(0xFF);
  Serial2.write(0xFF);
}

void setup()
{
  myNex.begin(38400);

  myNex.writeStr(F("page page1"));
}

void loop()
{
  uint32_t now = millis();
  static uint32_t msLastDisplayUpdate = 0;
  static byte oldPage;

  // checkNextionInput();
  myNex.NextionListen();

  // Seite muss aktualisiert werden ?
  if (now - msLastDisplayUpdate > 2000 ) {
    msLastDisplayUpdate = now;
    switch (myNex.currentPageId) {
      case 0:
        break;
      case 1:
        updatePage1();
        break;
      case 2:
        updatePage2();
        break;
      case 3:
        updatePage3();
        break;
      default:
        break;
    }
  }

  // Display Seitenwechsel ?
  if (myNex.currentPageId != oldPage) {
    
    Serial.print("Seitenwechsel von ");
    Serial.print(oldPage);
    Serial.print(" nach ");
    Serial.println(myNex.currentPageId);
    oldPage = myNex.currentPageId;

    switch (myNex.currentPageId) { // seitenwechsel auf sensorzuordnung
      case 0:
      case 1:
      break;
      case 2:
        sendPage2();
      break;
      case 3:

      sendPage3();
      break;
      default:
      break;
    }
  }
}

}