void clampAndScroll(int direction) {
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
