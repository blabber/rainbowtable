#include "Tlc5940.h"

const int DELTA_MAX = 1000;
const int DELTA_COLOR = 10;
const int DELAY_COLOR = 10;
const int DELAY_ROW = 1.75;

const int RGB_LEDS = 5;
const int ROW_COUNT = 5;
const int ROW_PINS[] = {
  A4, A0, A1, A2, A3};

const int MAX_COLOR = 12288;

int deltaColumn = DELTA_MAX;
int directionColumn = -1;

int deltaRow = 0;
int directionRow = 1;

int baseColor = 0;
int activeRow = 0;

unsigned long lastColorChange;
unsigned long lastRowChange;

void setup()
{
  Tlc.init();

  for (int r = 0; r < ROW_COUNT; r++) {
    pinMode(ROW_PINS[r], OUTPUT); 
  }

  lastColorChange = lastRowChange = millis();
  setRGBRow(activeRow, baseColor);
  setRowActive(activeRow);
}

void loop()
{
  unsigned long currentMillis = millis();
  if (lastRowChange + DELAY_ROW <= currentMillis) {
    lastRowChange = currentMillis;

    activeRow = (activeRow + 1) % ROW_COUNT;
    setRGBRow(activeRow, baseColor);
    setRowActive(activeRow);
  } 

  currentMillis = millis();
  if (lastColorChange + DELAY_COLOR <= currentMillis) {
    lastColorChange = currentMillis;

    if ((directionRow > 0 && deltaRow < DELTA_MAX) 
      || (directionRow < 0 && deltaRow > 0))
      deltaRow += directionRow * DELTA_COLOR;
    else
      directionRow *= -1;

    if ((directionColumn > 0 && deltaColumn < DELTA_MAX)
      || (directionColumn < 0 && deltaColumn > 0))
      deltaColumn += directionColumn * DELTA_COLOR;
    else
      directionColumn *= -1;

    baseColor = (baseColor + DELTA_COLOR) % MAX_COLOR;
  }
}

void setRGBRow(int row, int color)
{
  for (int l = 0; l < RGB_LEDS; l++) {
    int color = (baseColor + l*deltaColumn + row*deltaRow) % MAX_COLOR;
    setRGB(l, color);
  }
}

void setRowActive(int row)
{
  if (row == 0)
    digitalWrite(ROW_PINS[ROW_COUNT - 1], LOW);
  else
    digitalWrite(ROW_PINS[row - 1], LOW);

  Tlc.update();

  digitalWrite(ROW_PINS[row], HIGH);
}

void setRGB(int led, int color)
{
  int red;
  int green;
  int blue;

  if (color <= 4095)
  {
    red = 4095 - color;
    green = color;
    blue = 0;
  }
  else if (color <= 8191)
  {
    red = 0;
    green = 4095 - (color - 4096); 
    blue = (color - 4096);
  }
  else 
  {
    red =  (color - 8192);
    green = 0;
    blue = 4095 - (color - 8192);
  }

  int channel = (led * 3);

  Tlc.set(channel, red);
  Tlc.set(channel + 1, green);
  Tlc.set(channel + 2, blue);
}
