#include "analogsens.h"

namespace ANASENS
{
 
  float tAussen; // [°C]

  float getAussenTemp()
  {
    return tAussen;
  }

  void setup()
  {
    
  }

  void readAussenTemp()
  {
    int16_t raw = analogRead(pinAussentemp);
    tAussen = map(raw, 0, 1023, -20, 200); // 0 ... 5V -> -20°C ... 200°C
  }

  void loop()
  {
    uint32_t now = millis();
    static uint32_t msLastReadTemp = 0;

    if (now - msLastReadTemp > 5000 ) {
      msLastReadTemp = now;
      readAussenTemp();
    }
  }
}