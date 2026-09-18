#ifndef AKTOREN_H
#define AKTOREN_H

#include "Arduino.h"
#include "motorklappe.h"

namespace AKTOREN {
    const uint16_t pinPumpe = 41;
    extern Motorklappe klappeBGA;
    extern Motorklappe klappeGaertnerei;

    void pumpeAnAus(uint8_t val); // einschalten mit pumpeAnAus(HIGH) oder ausschalten mit pumpeAnAus(LOW)
    uint8_t getStatePumpe();
    
    void setup();
    void loop();
}

#endif 