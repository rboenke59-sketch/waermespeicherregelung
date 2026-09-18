#include "motorklappe.h"

Motorklappe::Motorklappe(const Config& cfg)
  : _cfg(cfg),
  _State(State::Idle),
  _endAuf(cfg.pinEndAuf),
  _endZu(cfg.pinEndZu),
  _laufzeit(0),
  _sollPos(255),
  _istPos(255) {}

void Motorklappe::fahreAufPosition(uint8_t sollpos)
{
    _sollPos = sollpos;
}

void Motorklappe::stop()
{
    _sollPos = 255;
}

void Motorklappe::setSim(bool sim)
{
    _cfg.sim = sim;
}

void Motorklappe::setup()
{
    _endAuf.setDebounceTime(_cfg.debounceMs);
    _endZu.setDebounceTime(_cfg.debounceMs);

    pinMode(_cfg.pinAufOut, OUTPUT);
    pinMode(_cfg.pinZuOut,  OUTPUT);
}

void Motorklappe::setAusgangAuf(uint8_t anAus)
{
    // pegel invertieren?
    // anAus = HIGH -> AUS, LOW -> EIN
    if (_cfg.outAktivLow) {
        digitalWrite(_cfg.pinAufOut, anAus ? LOW : HIGH);
    } else {
        // pegel nicht invertieren
        // anAus = LOW -> AUS, HIGH -> EIN
        digitalWrite(_cfg.pinAufOut, anAus);
    }
}

void Motorklappe::setAusgangZu(uint8_t anAus)
{
    // pegel invertieren?
    // anAus = HIGH -> AUS, LOW -> EIN
    if (_cfg.outAktivLow) {
        digitalWrite(_cfg.pinZuOut, anAus ? LOW : HIGH);
    } else {
        // pegel nicht invertieren
        // anAus = LOW -> AUS, HIGH -> EIN
        digitalWrite(_cfg.pinZuOut, anAus);
    }
}

bool Motorklappe::istEndlageAuf()
{  
    if (_cfg.endAktivLow) {
        return _endAuf.getState() == LOW;
    } else {
        return _endAuf.getState() == HIGH;
    }
}

bool Motorklappe::istEndlageZu()
{
    if (_cfg.endAktivLow) {
        return _endZu.getState() == LOW;
    } else {
        return _endZu.getState() == HIGH;
    }
}

void Motorklappe::loop()
{
    uint32_t msNow = millis();

    _endAuf.loop();
    _endZu.loop();

    if (_State != _OldState) { // zeitpunkt bei zustandswechsel merken
        _msStateTime = msNow;
        Serial.print(F("Klappe Zustandswechsel. Neu="));
        Serial.print((uint16_t)_State);
        Serial.print(F(". Alt="));
        Serial.println((uint16_t)_OldState);
    }

    switch (_State) {
        case State::Idle: // initialzustand
            setAusgangAuf(LOW);
            setAusgangZu(LOW);
            _laufzeit = 0;
            if (istEndlageAuf()) {
                _State = State::Auf;
            } else if (istEndlageZu()) {
                _State = State::Zu;
            } else { // neues Kommando ?
                if (_istPos == 255) { // position unbekannt ?
                    if (_sollPos == 101) { // oeffnen zum referenzieren
                        _State = State::Oeffnet;
                    }
                } else { // position bekannt
                    if (_sollPos > _istPos && _sollPos < 255) {
                        _State = State::Oeffnet;
                    } else if (_sollPos < _istPos && _sollPos < 255) {
                        _State = State::Schliesst;
                    }
                }
            }
        break;

        case State::Oeffnet:
            if (msNow - _msStateTime > _cfg.msDelayZuAuf) {
                setAusgangAuf(HIGH);
                setAusgangZu(LOW);
                if (_laufzeit == 0) { // startzeitpunkt laufzeit einmalig merken
                    // Serial.println(F("Klappe AUF"));
                    _laufzeit = msNow;
                } 
            }
            // (simulierte) endlage erreicht ?
            if (istEndlageAuf()) { 
                setAusgangAuf(LOW);
                setAusgangZu(LOW);
                _State = State::Auf;
            } else if (_istPos >= _sollPos // position erreicht ?
            || (_sollPos == 255) ){ // stopbefehl ?
                setAusgangAuf(LOW);
                setAusgangZu(LOW);
                _State = State::Idle;
            } else if (_sollPos < _istPos && _sollPos < 255) { // neue sollposition ?
                setAusgangAuf(LOW);
                setAusgangZu(LOW);
                _State = State::Schliesst;
            }
        break;
        case State::Schliesst:
            if (msNow - _msStateTime > _cfg.msDelayZuAuf) {
                setAusgangAuf(LOW);
                setAusgangZu(HIGH);
                if (_laufzeit == 0) { // startzeitpunkt laufzeit einmalig merken
                    // Serial.println(F("Klappe ZU"));
                    _laufzeit = msNow;
                } 
            }
            // endlage erreicht ?
            if (istEndlageZu()) { 
                setAusgangAuf(LOW);
                setAusgangZu(LOW);
                _State = State::Zu;
            } else if (_istPos <= _sollPos // position erreicht ?
            || (_sollPos == 255) ){ // stopbefehl ?
                setAusgangAuf(LOW);
                setAusgangZu(LOW);
                _State = State::Idle;
            } else if (_sollPos > _istPos && _sollPos < 255) { // neue sollposition ?
                setAusgangAuf(LOW);
                setAusgangZu(LOW);
                _State = State::Oeffnet;
            }
        break;
        case State::Auf:
            setAusgangAuf(LOW);
            setAusgangZu(LOW);
            _laufzeit = 0;
            if (_sollPos < _istPos && _sollPos < 255){ // neues kommando ?
                _State = State::Schliesst;
            }
        break;
        case State::Zu:
            setAusgangAuf(LOW);
            setAusgangZu(LOW);
            _laufzeit = 0;
            if (_sollPos > _istPos && _sollPos < 255) { // neues kommando ?
                _State = State::Oeffnet;
            }
        break;
        case State::Fehler:
            // TODO. evtl auch entfernen
        break;
    }
    _OldState = _State;

    // einmalig referenzieren wenn simulation aktiv
    if (_cfg.sim && _istPos == 255) {
        _istPos = 100;
    } 

    // istposition ueberschreiben wenn endlagen erreicht
    if (istEndlageAuf()) {
        _istPos = 100;
    } else if (istEndlageZu()) {
        _istPos = 0;
    } else if (_istPos != 255) { // endlage berechnen abhaengig von laufzeit, wenn position einmal bekannt gewesen ist
        if (_State == State::Oeffnet) {
            // istposition auf 100 begrenzt
            _istPos = min(100, ((uint32_t)(msNow - _laufzeit))*100UL / _cfg.msMaxZuAuf);
        } else if (_State == State::Schliesst) {
            // istposition auf 0 begrenzt
            _istPos = max(0, 100UL - ((uint32_t)(msNow - _laufzeit))*100UL / _cfg.msMaxAufZu);
        }
    }
}
