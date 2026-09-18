#ifndef TEMPERATURSENS_H
#define TEMPERATURSENS_H

#include <Arduino.h>
#include <DallasTemperature.h>

#define MAX_SENSORS 10
#define ONE_WIRE_BUS 2

// enum fuer "sprechenden" zugriff auf TempStruct array t
enum IDX_T
{
    BHKW_VL = 0,
    BHKW_RL = 1,
    Haupt_VL = 2,
    Haupt_RL = 3,
    Hackschn_VL = 4,
    Hackschn_RL = 5,
    Tank_Oben = 6,
    Tank_Unten = 7,
    BGA_VL = 8,
    BGA_RL = 9
};

namespace TEMPSENS {

    extern DeviceAddress sensorAddress[MAX_SENSORS];
    extern float sensorTemps[MAX_SENSORS]; // [°C]
    struct TempStruct
    {
        int8_t idx;                 // zugeordneter index aus im hochlauf ermittelten sensoren
        DeviceAddress speicherAddr; // in eeprom hinterlegte sensoraddresse
        float temp;                 // [°C] aktuelle temperatur
        float simTemp;              // [°C] aktuelle temperatur simulationswert
        bool simActive;             // reale oder sim temperatur zurueckgeben ?
    };


    extern TempStruct t[MAX_SENSORS];

    float getTemp(uint8_t idx); // temperatur (reale oder simulation) abfragen

    bool deviceAddressesAreIdentical(DeviceAddress addr1, DeviceAddress addr2);
    void copyAddress(DeviceAddress &target, DeviceAddress source);
    void copyAddress(uint8_t targetIdx, uint8_t sourceIdx);
    String addr2txt(DeviceAddress addr);
    void setup();
    void loop();
}
#endif