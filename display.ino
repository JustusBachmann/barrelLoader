void clampAndScroll(int8_t direction) {
  if (selectedIndex + direction >= 0 && selectedIndex + direction < activePageLength()) {
    selectedIndex += direction;
  }

  if (selectedIndex < topIndex) {
    topIndex = selectedIndex;
  } else if (selectedIndex > topIndex + (linesPerPage)) {
    topIndex = selectedIndex - (linesPerPage);
  }
}

void draw(void (*renderFunction)()) {
  u8g2.firstPage();
  do {
    renderFunction();
  } while (u8g2.nextPage());
}

void renderMenu() {
  u8g2.setFont(u8g2_font_ncenB08_tr);

  u8g2.setCursor(0, 10);
  printStringFromProgmem(activePage->title);

  u8g2.setCursor(0, 12);
  u8g2.drawLine(0, 12, 128, 12);

  for (uint8_t i = 0; i <= linesPerPage; i++) {
    uint8_t itemIndex = topIndex + i;
    if (itemIndex >= activePageLength()) break;

    uint16_t yPos = 24 + (i * 12); 

    if (itemIndex == selectedIndex) {
      u8g2.setDrawColor(1);
      u8g2.drawBox(0, yPos - 10, 128, 12);
      u8g2.setDrawColor(0);
    } else {
      u8g2.setDrawColor(1); 
    }

    u8g2.setCursor(2, yPos);
    if (itemIndex < activePage->subMenusCount) {
      printStringFromProgmem(activePage->subMenus[itemIndex]->title);
    } else if (itemIndex - activePage->subMenusCount < activePage->positionsCount) {
      u8g2.print(activePage->positions[itemIndex - activePage->subMenusCount]->name);
    } else if (itemIndex - activePage->subMenusCount - activePage->positionsCount < activePage->programsCount) {
      u8g2.print(activePage->programs[itemIndex - activePage->subMenusCount - activePage->positionsCount]->name);
    } else if (activePage->back != nullptr) {
      printStringFromProgmem(activePage->back);
    }
  }

  u8g2.setDrawColor(1);
}

void renderLoadingScreen() {
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawCircle(64, 32, 20);
  u8g2.drawLine(64, 27, 64, 37);
  u8g2.drawLine(59, 32, 69, 32);
  u8g2.setCursor(40, 55);
  u8g2.print("Arduino");
  u8g2.setCursor(40, 10);
  u8g2.print("Loading...");
}

void renderProgram() {
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawCircle(64, 32, 20);
  u8g2.drawLine(64, 27, 64, 37);
  u8g2.drawLine(59, 32, 69, 32);
  u8g2.setCursor(40, 55);
  u8g2.print(activeProgram->name);
  u8g2.setCursor(40, 10);
  u8g2.print("Running...");
}

void renderPosition() {
  u8g2.setFont(u8g2_font_ncenB08_tr);

  u8g2.setCursor(0, 10);
  u8g2.print(currentPosition->name);

  u8g2.drawLine(0, 12, 128, 12);

  u8g2.setCursor(5, 24);
  u8g2.print("old: ");
  u8g2.print(currentPosition->value);
  u8g2.setCursor(5, 36);
  u8g2.print("new: ");
  u8g2.print(newPosition.value);
}

void printStringFromProgmem(const char* str) {
    char buffer[30];
    strcpy_P(buffer, str);
    u8g2.print(buffer);
}
