void clearEEPROM() {
  draw(renderLoadingScreen);
  long f = 0; 
  for (int i = 0; i < lengthOfLong; i++) {
    f |= ((long)EEPROM.read(i)) << (i * 8);
  }
  Serial.println(f);
  
  if (f != eeprom_crc()) {
    for (int i = 1; i < EEPROM.length(); i++) {
      EEPROM.write(i, 0); 
    }
    for (int i = 0; i < lengthOfLong; i++) {
      EEPROM.write(i, (eeprom_crc() >> (i * 8)) & 0xFF); 
    }
  }
  
  goBack();
  draw(renderMenu);
}

unsigned long eeprom_crc(void) {
  const unsigned long crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };

  unsigned long crc = ~0L;
  
  for (int index = 0; index < EEPROM.length(); ++index) {
    uint8_t data = EEPROM.read(index); 
    crc = crc_table[(crc ^ data) & 0x0F] ^ (crc >> 4);
    crc = crc_table[(crc ^ (data >> 4)) & 0x0F] ^ (crc >> 4);
  }
  
  return ~crc; 
}

void saveToEEPROM() {
  if (currentPosition != nullptr) {
    currentPosition->value = newPosition.value;
    
    for (int i = 0; i < lengthOfLong; i++) {
      EEPROM.write(i + currentPosition->eepromOffset * lengthOfLong, 
                   (newPosition.value >> (i * 8)) & 0xFF); 
    }

    unsigned long crc = eeprom_crc();
    for (int i = 0; i < lengthOfLong; i++) {
      EEPROM.write(i, (crc >> (i * 8)) & 0xFF); 
    }

    currentPosition = nullptr;
  }
}

void readPositionFromEEPROM(Position* pos) {
  pos->value = 0; 
  for (int i = 0; i < lengthOfLong; i++) {
    pos->value |= ((long)EEPROM.read(i + pos->eepromOffset * lengthOfLong)) << (i * 8);
  }
  
  currentPosition = pos;
  newPosition.value = currentPosition->value;
  newPosition.axis = currentPosition->axis;
}

void loadPosition(Position* pos) {
   pos->value = 0; // Clear value before reading
   for (int i = 0; i < lengthOfLong; i++) {
     pos->value |= ((long)EEPROM.read(i + pos->eepromOffset * lengthOfLong)) << (i * 8);
   }
}

void loadAllPositions(Position* positions[], int count) {
  for (int i = 0; i < count; i++) {
    loadPosition(positions[i]);
  }
}