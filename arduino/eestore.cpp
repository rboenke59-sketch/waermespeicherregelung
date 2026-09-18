#include "eestore.h"

namespace EEStore {

// --- Layout ---
static constexpr uint32_t MAGIC = 0x45535452; // "ESTR"
static constexpr uint8_t  VER   = 1;
static constexpr int EE_BASE   = 0;

struct Header { uint32_t magic; uint8_t ver; uint8_t res; uint16_t total; } __attribute__((packed));
struct SecHdr  { uint16_t len;  uint16_t crc; } __attribute__((packed));

// --- State ---
AppParams params{};
uint8_t   roms[MAX_SENSORS][8]{};
uint8_t   romCount = 0;

// --- Utils ---
static uint16_t crc16_ibm(const uint8_t* d, size_t n) {
  uint16_t c = 0xFFFF;
  for (size_t i=0;i<n;++i) { c ^= d[i]; for (uint8_t b=0;b<8;++b) c = (c&1)?(c>>1)^0xA001:(c>>1); }
  return c;
}
static bool romValid(const uint8_t rom[8]) { return OneWire::crc8(rom,7)==rom[7]; }

static int writeHeader(uint16_t total) {
  Header h{MAGIC, VER, 0, total};
  int o = EE_BASE; const uint8_t* p = reinterpret_cast<const uint8_t*>(&h);
  for (size_t i=0;i<sizeof(h);++i) EEPROM.update(o++, p[i]);
  return o;
}
static bool readHeader(Header& h, int& ofs) {
  int o=EE_BASE; uint8_t* p=reinterpret_cast<uint8_t*>(&h);
  for (size_t i=0;i<sizeof(h);++i) p[i]=EEPROM.read(o++);
  ofs=o; return h.magic==MAGIC && h.ver==VER;
}
static int writeSection(const void* data, uint16_t len, int ofs) {
  SecHdr s{len, crc16_ibm(static_cast<const uint8_t*>(data), len)};
  const uint8_t* sp = reinterpret_cast<const uint8_t*>(&s);
  for (size_t i=0;i<sizeof(s);++i) EEPROM.update(ofs++, sp[i]);
  const uint8_t* dp = static_cast<const uint8_t*>(data);
  for (uint16_t i=0;i<len;++i) EEPROM.update(ofs++, dp[i]);
  return ofs;
}
static bool readSection(void* data, uint16_t maxLen, uint16_t& outLen, int& ofs) {
  SecHdr s{}; uint8_t* sp=reinterpret_cast<uint8_t*>(&s);
  for (size_t i=0;i<sizeof(s);++i) sp[i]=EEPROM.read(ofs++);
  if (s.len>maxLen) return false;
  uint8_t* dp=static_cast<uint8_t*>(data);
  for (uint16_t i=0;i<s.len;++i) dp[i]=EEPROM.read(ofs++);
  if (crc16_ibm(dp, s.len)!=s.crc) return false;
  outLen=s.len; return true;
}
bool slotIsEmpty(const uint8_t rom[8]) {
  for (uint8_t i=0;i<8;i++) if (rom[i] != 0xFF) return false;
  return true;
}
bool romIsValid(const uint8_t rom[8]) {
  if (slotIsEmpty(rom)) return false;
  uint8_t fam = rom[0];
  bool famOK = (fam == 0x28) || (fam == 0x10) || (fam == 0x22);
  if (!famOK) return false;
  return OneWire::crc8(rom, 7) == rom[7];
}
void clearAllSensors() {
  for (uint8_t i=0;i<MAX_SENSORS;i++)
    for (uint8_t j=0;j<8;j++) roms[i][j] = 0xFF;
}

// --- Public API ---
bool setup() {
  Header h{}; int ofs=0;
  if (!readHeader(h, ofs)) {
    // Defaults erstellen und speichern
    params = AppParams{};
    romCount = 0;
    return save(); // false nur bei Schreibfehler
  }

  // Params lesen
  uint16_t len=0;
  if (!readSection(&params, sizeof(params), len, ofs) || len!=sizeof(params)) {
    params = AppParams{}; romCount = 0; return save();
  }

   // Sensor-Tabelle: feste Länge = MAX_SENSORS*8
  uint8_t buf[MAX_SENSORS*8];
  if (!readSection(buf, sizeof(buf), len, ofs) || len != sizeof(buf)) {
    clearAllSensors();
    return save();
  }
  // in RAM kopieren und validieren (ungültige Slots auf 0xFF setzen)
  for (uint8_t i=0;i<MAX_SENSORS;i++) {
    memcpy(roms[i], &buf[i*8], 8);
    if (!romIsValid(roms[i])) {
      for (uint8_t j=0;j<8;j++) roms[i][j] = 0xFF;
    }
  }
  return true;
}

bool save() {
  // Plausibilität
  if (romCount>MAX_SENSORS) romCount = MAX_SENSORS;
  for (uint8_t i=0;i<romCount;++i) if (!romValid(roms[i])) return false;

  const uint16_t paramsLen = sizeof(params);
  const uint16_t sensLen   = 1 + romCount*8;
  const uint16_t total     = sizeof(SecHdr)+paramsLen + sizeof(SecHdr)+sensLen;

  int ofs = writeHeader(total);
  ofs = writeSection(&params, paramsLen, ofs);

  uint8_t buf[1 + MAX_SENSORS*8];
  buf[0]=romCount;
  for (uint8_t i=0;i<romCount;++i) memcpy(&buf[1+i*8], roms[i], 8);
  writeSection(buf, 1+romCount*8, ofs);
  return true;
}

bool saveParams()  { return save(); }
bool saveSensors() { return save(); }

} // namespace EEStore
