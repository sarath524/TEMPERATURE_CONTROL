// Include application, user and local libraries
#include "SPI.h"
#include "TFT_22_ILI9225.h"
#include "FreeSans24pt7b.h"

#define TFT_RST 26  // IO 26
#define TFT_RS  25  // IO 25
#define TFT_CLK 14  // HSPI-SCK
#define TFT_SDI 13  // HSPI-MOSI
#define TFT_CS  15  // HSPI-SS0
#define TFT_LED 0   // 0 if wired to +5V directly
SPIClass hspi(HSPI);

#define TFT_BRIGHTNESS 200 // Initial brightness of TFT backlight (optional)

TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, TFT_BRIGHTNESS);


void setup() {
  hspi.begin();
  tft.begin(hspi);
  Serial.begin(9600);
  //tft.drawRectangle(0, 0, tft.maxX() - 1, tft.maxY() - 1, COLOR_WHITE);
  tft.setFont(Terminal12x16);
  tft.setOrientation(3);
  tft.drawText(0, 10, "16.FEB.23");
  tft.drawText(0, 30, "4:41 PM");
  tft.setGFXFont(&FreeSans24pt7b);
  tft.drawGFXText(20, 140, "3 C", COLOR_WHITE);
  tft.drawCircle(58, 110, 2, COLOR_WHITE);
  tft.drawText(110, 75 , "32 C");
   tft.drawCircle(141, 75, 2, COLOR_WHITE);
  tft.drawText(110, 90 , "AMB");

  int x = 191;
  int y = 5;
  tft.drawRectangle(x, y, x + 22, y + 10, COLOR_WHITE);
  tft.fillRectangle(x + 2, y + 2, x + 5, y + 8, COLOR_WHITE);
  tft.fillRectangle(x + 7, y + 2, x + 10, y + 8, COLOR_WHITE);
  tft.fillRectangle(x + 12, y + 2, x + 15, y + 8, COLOR_WHITE);
  tft.fillRectangle(x + 17, y + 2, x + 20, y + 8, COLOR_WHITE);
  tft.fillRectangle(x + 22, y + 3, x + 24, y + 6, COLOR_WHITE);
  x = 175;
  y = 3;
  tft.drawLine(x + 3, y , x + 6, y + 3, COLOR_WHITE);
  tft.drawLine(x + 6, y + 3 , x + 3, y + 6, COLOR_WHITE);
  tft.drawLine(x + 3, y + 6 , x + 6, y + 9, COLOR_WHITE);
  tft.drawLine(x + 6, y + 9, x + 3, y + 12, COLOR_WHITE);
  tft.drawLine(x + 3, y , x + 3, y + 12, COLOR_WHITE);
  tft.drawLine(x, y + 3 , x + 3, y + 6, COLOR_WHITE);
  tft.drawLine(x + 3, y + 6 , x, y + 9, COLOR_WHITE);

}

void loop() {


}
