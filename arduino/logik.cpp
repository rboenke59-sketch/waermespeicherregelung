#include "logik.h"

#include "eestore.h"
#include "temperatursens.h"
#include "aktoren.h"

namespace LOGIK {
    bool sim = false;
    bool toogleSimulation()
    {
        sim = !sim;
        // Serial.println("toggle sim: " + String(sim));
        return sim;
    }
    bool getSimState()
    {
        return sim;
    }

    void loop()
    {
        //===========================================================================
        // Zirkulationspumpe AN/AUS
        // Schieber Gaertnerei AUF/ZU
        //===========================================================================
        if (TEMPSENS::getTemp(IDX_T::Haupt_VL) - TEMPSENS::getTemp(IDX_T::Haupt_RL) < EEStore::params.tempDiffPuAus_x10*0.1f) {
            AKTOREN::pumpeAnAus(LOW);
            AKTOREN::klappeGaertnerei.fahreAufPosition(EEStore::params.zielKlappeGaertnereiZu);
        } else if (TEMPSENS::getTemp(IDX_T::Haupt_VL) - TEMPSENS::getTemp(IDX_T::Haupt_RL) > EEStore::params.tempDiffPuAus_x10*0.1f + HYST_TEMP) {
            AKTOREN::pumpeAnAus(HIGH);
            AKTOREN::klappeGaertnerei.fahreAufPosition(101); // auf bis zur endlage
        }

        //===========================================================================
        // Schieber BGA AUF/ZU
        //===========================================================================
        if (TEMPSENS::getTemp(IDX_T::BHKW_VL) < EEStore::params.bhkw_vl_zu_x10*0.1f) {
            AKTOREN::klappeBGA.fahreAufPosition(EEStore::params.zielKlappeBGAzu);
        } else if (TEMPSENS::getTemp(IDX_T::BHKW_VL) > EEStore::params.bhkw_vl_zu_x10*0.1f + HYST_TEMP) {
            AKTOREN::klappeBGA.fahreAufPosition(101); // auf bis zur endlage
        }
    }
}