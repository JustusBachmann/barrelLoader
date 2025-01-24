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

  u8g2.drawCircle(64, 32, 20); // Centered circle for the logo
  
  u8g2.drawLine(64, 27, 64, 37); // Vertical line
  u8g2.drawLine(59, 32, 69, 32); // Horizontal line

  u8g2.setCursor(40, 55); // Adjust position as needed
  u8g2.print("Arduino");

  u8g2.setCursor(40, 10);
  u8g2.print("Loading...");
}