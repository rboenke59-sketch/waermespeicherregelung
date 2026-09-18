#ifndef LOGIK_H
#define LOGIK_H

#include "Arduino.h"

namespace LOGIK {

    const float HYST_TEMP = 0.5f; // [°C] hystere f. temperatur

    bool toogleSimulation();
    bool getSimState();

    void loop();
}

#endif 