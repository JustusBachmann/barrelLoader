void changePage(MenuPage* newActive) {
  if (historyIndex < MAX_HISTORY - 1) {
      navigationHistory[++historyIndex] = activePage; 
  }
  activePage = newActive;
  if (activePage->title == "Settings") {
    state = SETTING;
  }
  selectedIndex = 0;
  topIndex = 0;
}

void goBack() {
  if (historyIndex >= 0) {
    if (activePage->title == "Settings") {
      state = IDLE;
    }
    activePage = navigationHistory[historyIndex--];
    selectedIndex = 0;
    topIndex = 0; 
  }
}

void renderMenu() {
  u8g2.setFont(u8g2_font_ncenB08_tr);

  u8g2.setCursor(0, 10);
  u8g2.print(activePage->title);

  u8g2.setCursor(0, 12);
  u8g2.print("___________________________");

  for (int i = 0; i <= linesPerPage; i++) {
    int itemIndex = topIndex + i;
    if (itemIndex >= activePageLength()) break;

    int yPos = 24 + (i * 12); 

    if (itemIndex == selectedIndex) {
      u8g2.setDrawColor(1);
      u8g2.drawBox(0, yPos - 10, 128, 12);
      u8g2.setDrawColor(0);
    } else {
      u8g2.setDrawColor(1); 
    }

    u8g2.setCursor(2, yPos);
    if (itemIndex < activePage->subMenusCount) {
      u8g2.print(activePage->subMenus[itemIndex]->title);
    } else if (itemIndex - activePage->subMenusCount < activePage->positionsCount) {
      u8g2.print(activePage->positions[itemIndex - activePage->subMenusCount]->name);
    } else {
      u8g2.print(activePage->items[itemIndex - activePage->subMenusCount - activePage->positionsCount]);
    }
  }

  u8g2.setDrawColor(1);
}
