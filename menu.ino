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

void setPosition(int dir) {
  newPosition.value += STEP_SIZE * dir;
  newPosition.axis = currentPosition->axis;
  step(&newPosition);
}