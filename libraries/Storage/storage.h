#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>

#define crcLength 4
#define lengthOfInt16_t 2

struct Item;
struct MenuPage;
struct Program;

class Storage {
  public:
    void clearEeprom();

    template <typename T>
    void loadFromProgmem(T* dest, const T* src) {
      memcpy_P(dest, src, sizeof(T));
    }

    template <typename T>
    const T* getFromProgmem(const T* const* array, uint8_t index) {
      return reinterpret_cast<const T*>(pgm_read_ptr(&array[index]));
    }

    template <typename T>
    void saveToEeprom(const Item* item, const T Item::* offsetField, uint16_t& value) {
      int baseAddr = item->*offsetField * lengthOfInt16_t + crcLength;

      write16BitToEEPROM(baseAddr, value);

      writeCrcToEEPROM(eepromCrc());
    }

    template <typename T>
    void loadCurrentValueFromEeprom(const Item* itemPtr, const T Item::* offsetField, uint16_t& currentValue) {
        int baseAddr = loadOffsetFromProgmem(itemPtr, offsetField) * lengthOfInt16_t + crcLength;

        if (baseAddr + lengthOfInt16_t > EEPROM.length()) {
            currentValue = 0;
            return;
        }
        currentValue = read16BitFromEEPROM(baseAddr);
    }

    const __FlashStringHelper* printFromProgmem(const char *str);

  private:
    static unsigned long eepromCrc();

    template <typename T>
    uint8_t loadOffsetFromProgmem(const Item* itemPtr, const T Item::* offsetField) {
      Item item;
      loadFromProgmem(&item, itemPtr);
      return item.*offsetField;
    }

    void writeByteToEEPROM(int address, uint8_t value) {
        if (EEPROM.read(address) != value) {
            EEPROM.write(address, value);
        }
    }

    void writeCrcToEEPROM(unsigned long crc) {
        for (int i = 0; i < crcLength; i++) {
            writeByteToEEPROM(i, (crc >> (i * 8)) & 0xFF);
        }
    }
    
    uint16_t read16BitFromEEPROM(int address) {
      return EEPROM.read(address) | ((uint16_t)EEPROM.read(address + 1) << 8);
    }

    void write16BitToEEPROM(int address, uint16_t value) {
        writeByteToEEPROM(address, value & 0xFF);
        writeByteToEEPROM(address + 1, (value >> 8) & 0xFF);
    }
    bool checkCrc();
};

#endif // STORAGE_H
