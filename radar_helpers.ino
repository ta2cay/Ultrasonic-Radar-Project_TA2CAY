
void drawStaticRadar() {
  display.setFont(u8g2_font_4x6_tr);
  for (int i = 1; i <= RINGS; i++) {
    int r = (MAX_RADIUS * i) / RINGS;
    for (int angle = 0; angle <= 180; angle += 2) {
      float rad = radians(angle);
      int x = CENTER_X + r * cos(rad);
      int y = CENTER_Y - r * sin(rad);
      if (x >= 0 && x < 128 && y >= 0 && y < 64) display.drawPixel(x, y);
    }
    char lbl[4]; sprintf(lbl, "%d", i * 10);
    int ly = CENTER_Y - r + 3;
    if (ly >= 5 && ly < 59) display.drawStr(CENTER_X - 4, ly, lbl);
  }
  display.drawPixel(CENTER_X, CENTER_Y);
  for (int a = 0; a <= 180; a += 45) {
    float rad = radians(a);
    display.drawLine(CENTER_X, CENTER_Y,
      CENTER_X + MAX_RADIUS * cos(rad),
      CENTER_Y - MAX_RADIUS * sin(rad));
  }
}

void updateRadarDisplay(int angle, long distance) {
  display.clearBuffer();
  display.setDrawColor(1);
  display.drawBox(0, 0, 128, 64);
  display.setDrawColor(0);

  drawStaticRadar();
  updateFadeTrail(angle);
  drawFadeEffect();
  drawObstacleHistory();

  float rad = radians(angle);
  display.drawLine(CENTER_X, CENTER_Y,
    CENTER_X + MAX_RADIUS * cos(rad),
    CENTER_Y - MAX_RADIUS * sin(rad));

  if (distance > 0 && distance <= 50) {
    int or2 = (distance * MAX_RADIUS) / 50;
    int ox = CENTER_X + or2 * cos(rad);
    int oy = CENTER_Y - or2 * sin(rad);
    display.drawDisc(ox, oy, 3);
    display.drawCircle(ox, oy, 5);
    addObstacle(angle, distance);
  }

  scanCount++;
  if (millis() - lastBlink > 500) { blinkState = !blinkState; lastBlink = millis(); }

  char buf[20]; int w;
  display.setFont(u8g2_font_open_iconic_embedded_1x_t); display.drawGlyph(0, 8, 75);
  display.setFont(u8g2_font_7x13B_tr);
  sprintf(buf, "%d%c", angle, 176); display.drawStr(10, 8, buf);

  if (temperature > 0) {
    display.setFont(u8g2_font_open_iconic_thing_1x_t); display.drawGlyph(0, 18, 64);
    display.setFont(u8g2_font_5x7_tr);
    sprintf(buf, "%dC", (int)temperature); display.drawStr(10, 18, buf);
  }

  display.setFont(u8g2_font_open_iconic_thing_1x_t); display.drawGlyph(120, 8, 71);
  display.setFont(u8g2_font_7x13B_tr);
  sprintf(buf, "%ld", distance < 999 ? distance : 0);
  w = display.getStrWidth(buf); display.drawStr(115 - w, 8, buf);

  display.setFont(u8g2_font_open_iconic_thing_1x_t); display.drawGlyph(120, 18, 68);
  display.setFont(u8g2_font_5x7_tr);
  sprintf(buf, "%d", totalDetections);
  w = display.getStrWidth(buf); display.drawStr(118 - w, 18, buf);

  display.setFont(u8g2_font_open_iconic_arrow_1x_t); display.drawGlyph(120, 28, 79);
  display.setFont(u8g2_font_5x7_tr);
  sprintf(buf, "%lu", scanCount / 36);
  w = display.getStrWidth(buf); display.drawStr(118 - w, 28, buf);

  display.setDrawColor(0); display.drawRBox(35, 0, 58, 14, 2);
  display.setDrawColor(1);
  display.setFont(u8g2_font_ncenB10_tr);
  w = display.getStrWidth("RADAR");
  display.drawStr((128 - w) / 2, 11, "RADAR");
  display.setDrawColor(0);

  if (closestDistance < 30 && blinkState) {
    display.setFont(u8g2_font_ncenB08_tr);
    char* sym = getDangerSymbol(closestDistance);
    w = display.getStrWidth(sym);
    display.drawStr((128 - w) / 2, 45, sym);
  }
  display.sendBuffer();
}

void addObstacle(int angle, long distance) {
  unsigned long t = millis();
  for (int i = 0; i < MAX_OBSTACLES; i++)
    if (obstacleHistory[i].active && (t - obstacleHistory[i].timestamp > 10000))
      obstacleHistory[i].active = false;
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    if (!obstacleHistory[i].active) {
      obstacleHistory[i] = {angle, distance, t, true};
      break;
    }
  }
  totalDetections++;
  totalDistance += distance;
  if (distance < closestDistance) closestDistance = distance;
}

void updateFadeTrail(int a) { fadeAngles[fadeIndex] = a; fadeIndex = (fadeIndex+1)%FADE_LENGTH; }

void drawFadeEffect() {
  for (int i = 0; i < FADE_LENGTH; i++) {
    int idx = (fadeIndex - i - 1 + FADE_LENGTH) % FADE_LENGTH;
    int ang = fadeAngles[idx];
    if (ang >= 0 && ang <= 180 && i % 2 == 0) {
      float rad = radians(ang);
      display.drawLine(CENTER_X, CENTER_Y,
        CENTER_X + (MAX_RADIUS - i*3)*cos(rad),
        CENTER_Y - (MAX_RADIUS - i*3)*sin(rad));
    }
  }
}

void drawObstacleHistory() {
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    if (obstacleHistory[i].active) {
      float rad = radians(obstacleHistory[i].angle);
      int r2 = (obstacleHistory[i].distance * MAX_RADIUS) / 50;
      int ox = CENTER_X + r2*cos(rad), oy = CENTER_Y - r2*sin(rad);
      display.drawPixel(ox, oy); display.drawPixel(ox+1, oy);
      display.drawPixel(ox, oy+1); display.drawPixel(ox-1, oy);
    }
  }
}

char* getDangerSymbol(long d) {
  if (d < 15) return "!!!";
  if (d < 30) return "!!";
  return "!";
}
