#ifndef MOTORKLAPPE_H
#define MOTORKLAPPE_H

#include <Arduino.h>
#include <ezButton.h>

/**
 * @brief Steuerung einer Motor-Klappe mit zwei Endlagen über Relais.
 * 
 * Unterstützt:
 *  - Zwei Richtungs-Ausgänge (AUF / ZU)
 *  - Zwei Endschalter mit Entprellung (ezButton)
 *  - Schutz durch Verzoegerung bei direktem Umschalten
 */
class Motorklappe {
    
public:
    /**
     * @brief Mögliche Zustände der Motorklappe.
     */
    enum class State { 
        Idle,       // Ausgaenge aus, Halte Position
        Oeffnet,    // Ausgang AUF an, Endlage noch nicht erreicht
        Schliesst,  // Ausgang ZU an, Endlage noch nicht erreicht
        Auf,        // Ausgang AUF aus, Endlage erreicht
        Zu,         // Ausgang ZU aus, Endlage erreicht
        Fehler      // Ausgaenge aus
    };

     /**
     * @brief Konfiguration der Pins und Parameter.
     */
    struct Config {
        uint8_t pinAufOut;              ///< Ausgang "AUF" (Relais / H-Bridge-Richtung)
        uint8_t pinZuOut;               ///< Ausgang "ZU"
        uint8_t pinEndAuf;              ///< Endschalter offen
        uint8_t pinEndZu;               ///< Endschalter zu
        bool endAktivLow = true;        ///< true: Schalter gegen GND (INPUT_PULLUP)
        bool outAktivLow = false;        ///< true: Relaisausgang invertiert, d.h. LOW = Relais AN ?
        uint32_t debounceMs = 30;       ///< [ms] Entprellzeit Endlagen
        uint32_t msMaxAufZu  = 30000;   ///< [ms] Dauer von Klappe ganz auf bis Klappe ganz zu
        uint32_t msMaxZuAuf  = 30000;   ///< [ms] von Klappe ganz zu bis Klappe ganz auf 
        uint32_t msDelayZuAuf = 1000;   ///< [ms] einfache Wartezeit zum Schalten der Ausgaenge um direktes Umschalten AUF/ZU zu verhindern
        bool sim = false;               ///< simulation / an / aus
    };

    explicit Motorklappe(const Config& cfg); // Konstruktor

    /**
     * @param sollpos - 0 ... 100 % wird ueber Laufzeit berechnet. Wobei 0% = ZU und 100% = AUF.
     * 
     * mit 101 kann fest auf endlage AUF gefahren werden zum referenzieren.
     * 255 ist identisch mit stop()
     */
    void fahreAufPosition(uint8_t sollpos); 
    inline uint8_t getPosition(){return _istPos;}; // 0 ... 100 %; 255 = unbekannt. wird ueber Laufzeit berechnet. Wobei 0% = ZU und 100% = AUF
    void stop();

    void setup();
    void loop();

     /**
     * @brief Liefert aktuellen Zustand.
     */
    State getState() const { return _State; }

    void setSim(bool sim); // simulation an / aus

private:
    void setAusgangAuf(uint8_t anAus);
    void setAusgangZu(uint8_t anAus);
    bool istEndlageAuf(); // der entprellte endlagenzustand
    bool istEndlageZu(); // der entprellte endlagenzustand
    
    Config _cfg;
    State _State{State::Idle};
    State _OldState{State::Idle};
    ezButton _endAuf;
    ezButton _endZu;
    uint32_t _laufzeit;
    uint32_t _msStateTime; // [ms] zeitpunkt bei zustandswechsel
    uint8_t _sollPos; // [%] 0..100; 255 = stop
    uint8_t _istPos; // [%] 0..100; 255 = unbekannt
};

#endif