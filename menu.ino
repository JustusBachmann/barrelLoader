void changePage(MenuPage* newActive) {
  if (historyIndex < MAX_HISTORY - 1) {
      navigationHistory[++historyIndex] = activePage; 
  }
  activePage = newActive;
  selectedIndex = 0;
  topIndex = 0;
}

void goBack() {
  if (historyIndex >= 0) {
    activePage = navigationHistory[historyIndex--];
    selectedIndex = 0;
    topIndex = 0; 
  }
}

void setPosition(int8_t dir) {
  newPosition.value += STEP_SIZE * dir;
  newPosition.axis = currentPosition->axis;
  step(&newPosition);
}
