#include "aktoren.h"

namespace AKTOREN {
    Motorklappe klappeBGA(Motorklappe::Config{
        45,          // pinAufOut  - Ausgang "AUF"
        44,          // pinZuOut   - Ausgang "ZU"
        49,          // pinEndAuf  - Endschalter "offen"
        48,          // pinEndZu   - Endschalter "zu"
        true,       // endAktivLow - true = Schalter nach GND (INPUT_PULLUP)
        false,      // outAktivLow - Relaisausgang invertiert, d.h. LOW = Relais AN ?
        30,         // debounceMs  - Entprellzeit in Millisekunden
        39000,      // msMaxAufZu  - max. Zeit von "auf" nach "zu"
        38000,      // msMaxZuAuf  - max. Zeit von "zu" nach "auf"
        false       // sim         - Simulation aus
    });
    Motorklappe klappeGaertnerei(Motorklappe::Config{
        43,          // pinAufOut  - Ausgang "AUF"
        42,          // pinZuOut   - Ausgang "ZU"
        47,          // pinEndAuf  - Endschalter "offen"
        46,          // pinEndZu   - Endschalter "zu"
        true,       // endAktivLow - true = Schalter nach GND (INPUT_PULLUP)
        false,      // outAktivLow - Relaisausgang invertiert, d.h. LOW = Relais AN ?
        30,         // debounceMs  - Entprellzeit in Millisekunden
        30000,      // msMaxAufZu  - max. Zeit von "auf" nach "zu"
        30000,      // msMaxZuAuf  - max. Zeit von "zu" nach "auf"
        false       // sim - Simulation aus
    });

    void pumpeAnAus(uint8_t val) 
    {
        digitalWrite(pinPumpe, val);
    }
    uint8_t getStatePumpe()
    {
        return (uint8_t)digitalRead(pinPumpe);
    }

    void setup()
    {
        klappeBGA.setup();
        klappeGaertnerei.setup();

        pinMode(pinPumpe, OUTPUT);
    }

    void loop()
    {
        klappeBGA.loop();
        klappeGaertnerei.loop();
    }
}