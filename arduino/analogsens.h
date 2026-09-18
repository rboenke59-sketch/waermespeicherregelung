#ifndef ANALOGSENS_H
#define ANALOGSENS_H

#include <Arduino.h>

namespace ANASENS {

    const uint16_t pinAussentemp = A0; 

    float getAussenTemp(); 
    void setup();
    void loop();
}
#endif