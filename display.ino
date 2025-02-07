void clampAndScroll(int8_t direction) {
  if (selectedIndex + direction >= 0 && selectedIndex + direction < activePageLength) {
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
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(0, 10);
  do {
    renderFunction();
  } while (u8g2.nextPage());
}

void renderMenu() {
  printFromProgmem(activePage.title);

  u8g2.setCursor(0, 12);
  u8g2.drawLine(0, 12, 128, 12);

  for (uint8_t i = 0; i <= linesPerPage; i++) {
    uint8_t itemIndex = topIndex + i;
    if (itemIndex >= activePageLength) break;

    uint16_t yPos = 24 + (i * 12); 

    if (itemIndex == selectedIndex) {
      u8g2.setDrawColor(1);
      u8g2.drawBox(0, yPos - 10, 128, 12);
      u8g2.setDrawColor(0);
    } else {
      u8g2.setDrawColor(1); 
    }

    u8g2.setCursor(2, yPos);
    if (itemIndex < activePage.subMenusCount) {
      const MenuPage* subMenuPtr = getFromProgmem(activePage.subMenus, itemIndex);
      printFromProgmem(subMenuPtr);
    } else if (itemIndex - activePage.subMenusCount < activePage.positionsCount) {
      const Position* positionPtr = getFromProgmem(activePage.positions, itemIndex - activePage.subMenusCount);
      printFromProgmem(positionPtr);
    } else if (itemIndex - activePage.subMenusCount - activePage.positionsCount < activePage.programsCount) {
      const Program* programPtr = getFromProgmem(activePage.programs, itemIndex - activePage.subMenusCount- activePage.positionsCount);
      printFromProgmem(programPtr);
    } else if (activePage.back != nullptr) {
      printFromProgmem(activePage.back);
    }
  }

  u8g2.setDrawColor(1);
}

void renderProgram() {
  u8g2.print("Running...");
}

void renderPosition() {
  printFromProgmem(currentPosition.pos);

  u8g2.drawLine(0, 12, 128, 12);

  u8g2.setCursor(5, 24);
  u8g2.print("old: ");
  u8g2.print(currentPosition.value);
  u8g2.setCursor(5, 36);
  u8g2.print("new: ");
  u8g2.print(newPosition.value);
}

void printFromProgmem(const char* str) {
    char buffer[20];
    strcpy_P(buffer, str);
    u8g2.print(buffer);
}

void changePage(MenuPage* newActive) {
  if (historyIndex < MAX_HISTORY - 1) {
    navigationHistory[++historyIndex] = newActive;
  } else {
    // Shift history if needed; this ensures the newest page is always stored
    for (int i = 1; i < MAX_HISTORY; i++) {
      navigationHistory[i - 1] = navigationHistory[i];
    }
    navigationHistory[MAX_HISTORY - 1] = newActive;
  }

  loadActivePage(newActive);
  selectedIndex = 0;
  topIndex = 0;
}

void goBack() {
    if (historyIndex > 0) {
    loadActivePage(navigationHistory[--historyIndex]);
  } else if (historyIndex == 0) {
    loadActivePage(navigationHistory[historyIndex]); 
  }
  selectedIndex = 0;
  topIndex = 0;
}
