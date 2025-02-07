#pragma region eeprom
void clearEeprom() {
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

bool checkCrc() {
  unsigned long storedCrc = 0;
  for (int i = 0; i < crcLength; i++) {
    storedCrc |= ((unsigned long)EEPROM.read(i)) << (i * 8);
  }

  return storedCrc == eepromCrc();
}

unsigned long eepromCrc(void) {
  const unsigned long crcTable[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
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

void saveToEeprom(DynPosition* dynPos) {
  int baseAddr = loadOffsetFromProgmem(dynPos->pos) * lengthOfInt16_t + crcLength;

  for (int i = 0; i < lengthOfInt16_t; i++) {
    uint8_t byteValue = (dynPos->value >> (i * 8)) & 0xFF;
    if (EEPROM.read(baseAddr + i) != byteValue) {
      EEPROM.write(baseAddr + i, byteValue);
    }
  }

  unsigned long crc = eepromCrc();
  for (int i = 0; i < crcLength; i++) {
    uint8_t byteValue = (crc >> (i * 8)) & 0xFF;
    if (EEPROM.read(i) != byteValue) {
      EEPROM.write(i, byteValue);
    }
  }
  newPosition = {nullptr, -1, -1};
  currentPosition = {nullptr, -1, -1};
}

void loadCurrentPosFromEeprom(Position* pos) {
  getFromEeprom(pos, &currentPosition);
  newPosition.axis = currentPosition.axis;
  newPosition.value = currentPosition.value;
}

void getFromEeprom(Position* pos, DynPosition* dynPos) {
  if (!pos || !dynPos) {
    return;
  }

  dynPos->pos = pos;
  int baseAddr = loadOffsetFromProgmem(dynPos->pos) * lengthOfInt16_t + crcLength;

  if (baseAddr + lengthOfInt16_t > EEPROM.length()) {
    dynPos->value = 0;
    return;
  }

  dynPos->value = 0;
  for (int i = 0; i < lengthOfInt16_t; i++) {
    dynPos->value |= ((int16_t)EEPROM.read(baseAddr + i)) << (i * 8);
  }
  dynPos->axis = loadAxisFromProgmem(pos);
}
#pragma endregion

#pragma region progmem
void loadActivePage(MenuPage* menuPtr) {
  loadFromProgmem(&activePage, menuPtr);
  activePageLength = (activePage.back == nullptr ? 0 : 1) + activePage.subMenusCount + activePage.positionsCount + activePage.programsCount;
}

void loadFromProgmem(MenuPage* page, const MenuPage* menuPtr) {
  memcpy_P(page, menuPtr, sizeof(MenuPage));
}

void loadFromProgmem(Program* prog, const Program* progPtr) {
  memcpy_P(prog, progPtr, sizeof(Program));
}

uint8_t loadAxisFromProgmem(const Position* posPtr) {
  Position pos;
  memcpy_P(&pos, posPtr, sizeof(Position));
  return pos.axis;
}

uint8_t loadOffsetFromProgmem(const Position* posPtr) {
  Position pos;
  memcpy_P(&pos, posPtr, sizeof(Position));
  return pos.eepromOffset;
}

const MenuPage* getFromProgmem(const MenuPage* const* subMenusArray, uint8_t index) {
    return (const MenuPage*)pgm_read_ptr(&subMenusArray[index]);
}

const Program* getFromProgmem(const Program* const* programsArray, uint8_t index) {
    return (const Program*)pgm_read_ptr(&programsArray[index]);
}

const Position* getFromProgmem(const Position* const* positionsArray, uint8_t index) {
    return (const Position*)pgm_read_ptr(&positionsArray[index]);
}

void printFromProgmem(MenuPage* subMenuPtr) {
  MenuPage subMenu;
  memcpy_P(&subMenu, subMenuPtr, sizeof(MenuPage));
  printFromProgmem(subMenu.title);
}

void printFromProgmem(Program* programPtr) {
  Program program;
  memcpy_P(&program, programPtr, sizeof(Program));
  printFromProgmem(program.name);
}

void printFromProgmem(Position* positionPtr) {
  Position position;
  memcpy_P(&position, positionPtr, sizeof(Position));
  printFromProgmem(position.name);
}
#pragma endregion
