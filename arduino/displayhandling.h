#ifndef DISPLAYHANDLING_H
#define DISPLAYHANDLING_H

#include "Arduino.h"

namespace DISP {
    const uint32_t COLOR_WHITE = 65535;
    const uint32_t COLOR_BLACK = 0;
    const uint32_t COLOR_GREEN = 1024;

    extern char infotext[35]; // flexibler anzeigetext auf page1

    void updatePage1();

    void sendPage2();
    void getPage2SimValues();
    void getPage2ParamValues();

    void sendPage3SensorAddr(uint8_t idx);
    void sendPage3SensorTemp(uint8_t idx);
    void sendPage3Combobox(uint8_t idx);
    void sendPage3();
    void updatePage3();

    void sensorZuordnungVonNextionAuswerten();

    void loop();
    void setup();
}
#endif