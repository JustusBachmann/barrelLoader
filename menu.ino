void changePage(MenuPage* newActive) {
  // if (historyIndex < MAX_HISTORY - 1) {
  //     navigationHistory[++historyIndex] = activePage; 
  // }
  loadActivePage(newActive);
  selectedIndex = 0;
  topIndex = 0;
}

void goBack() {
  if (historyIndex >= 0) {
    loadActivePage(&mainMenu);
    // activePage = navigationHistory[historyIndex--];
    selectedIndex = 0;
    topIndex = 0; 
  }
}

void setPosition(int8_t dir) {
  newPosition.value += STEP_SIZE * dir;
  // step(&newPosition);
}
