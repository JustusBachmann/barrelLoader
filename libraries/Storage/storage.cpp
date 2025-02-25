#include <storage.h>

void Storage::clearEeprom() {
  for (int i = crcLength; i < EEPROM.length(); i++) {
    if (EEPROM.read(i) != 0) {
      EEPROM.write(i, 0);
    }
  }

  unsigned long crc = eepromCrc();
  for (int i = 0; i < crcLength; i++) {
    EEPROM.write(i, (crc >> (i * 8)) & 0xFF);
  }
}

bool Storage::checkCrc() {
  unsigned long storedCrc = 0;
  for (int i = 0; i < crcLength; i++) {
    storedCrc |= ((unsigned long)EEPROM.read(i)) << (i * 8);
  }
  return storedCrc == eepromCrc();
}

unsigned long Storage::eepromCrc() {
  const unsigned long crcTable[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4,
    0x4db26158, 0x5005713c, 0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };

  unsigned long crc = ~0L;
  // Calculate CRC for all data except reserved bytes
  for (int index = crcLength; index < EEPROM.length(); ++index) {
    uint8_t data = EEPROM.read(index);
    crc = crcTable[(crc ^ data) & 0x0F] ^ (crc >> 4);
    crc = crcTable[(crc ^ (data >> 4)) & 0x0F] ^ (crc >> 4);
  }
  return ~crc;
}

const __FlashStringHelper* Storage::printFromProgmem(const char *str) {
    return reinterpret_cast<const __FlashStringHelper*>(str);
}
