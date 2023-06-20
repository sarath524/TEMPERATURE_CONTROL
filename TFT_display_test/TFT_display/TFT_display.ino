// Include application, user and local libraries
#include "SPI.h"
#include "TFT_22_ILI9225.h"
//#include "FreeSans24pt7b.h"


#define TFT_RST 26
#define TFT_RS  2
#define TFT_CS  19  // SS
#define TFT_SDI 13 / MOSI
#define TFT_CLK 14  / // SCK
#define TFT_LED -1 
#define TFT_BRIGHTNESS 200

static char ambient_temperature_str[50];
   // 0 if wired to +5V directly
SPIClass hspi(HSPI);

#define TFT_BRIGHTNESS 200 // Initial brightness of TFT backlight (optional)

TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_BRIGHTNESS);

#define SPI_DEFAULT_FREQ         10000000

SPIClass spi = SPIClass(HSPI);

void setup() {
  
  Serial.begin(115200);
  hspi.begin();
  tft.begin(hspi);
  
  digitalWrite(TFT_RS,HIGH);
  tft.setFont(Terminal12x16);
  tft.setOrientation(3);

  //tft.setGFXFont(&FreeSans24pt7b);
//  tft.drawGFXText(20, 140, "3 C", COLOR_WHITE);
//  tft.drawCircle(58, 110, 2, COLOR_WHITE);
//  tft.drawText(110, 75 , "32 C");
//   tft.drawCircle(141, 75, 2, COLOR_WHITE);
//  tft.drawText(110, 90 , "AMB");
//
  int x = 191;
  int y = 5;
//  tft.drawRectangle(x, y, x + 22, y + 10, COLOR_WHITE);
//  tft.fillRectangle(x + 2, y + 2, x + 5, y + 8, COLOR_WHITE);
//  tft.fillRectangle(x + 7, y + 2, x + 10, y + 8, COLOR_WHITE);
//  tft.fillRectangle(x + 12, y + 2, x + 15, y + 8, COLOR_WHITE);
//  tft.fillRectangle(x + 17, y + 2, x + 20, y + 8, COLOR_WHITE);
//  tft.fillRectangle(x + 22, y + 3, x + 24, y + 6, COLOR_WHITE);
//  x = 175;
//  y = 3;
//  tft.drawLine(x + 3, y , x + 6, y + 3, COLOR_WHITE);
//  tft.drawLine(x + 6, y + 3 , x + 3, y + 6, COLOR_WHITE);
//  tft.drawLine(x + 3, y + 6 , x + 6, y + 9, COLOR_WHITE);
//  tft.drawLine(x + 6, y + 9, x + 3, y + 12, COLOR_WHITE);
//  tft.drawLine(x + 3, y , x + 3, y + 12, COLOR_WHITE);
//  tft.drawLine(x, y + 3 , x + 3, y + 6, COLOR_WHITE);
//  tft.drawLine(x + 3, y + 6 , x, y + 9, COLOR_WHITE);

  x = 15;
  y = 120;
  tft.drawLine(x + 7, y + 6 , x+28, y + 6, COLOR_WHITE);
  tft.drawLine(x + 28, y + 6 , x+28 ,y + 40, COLOR_WHITE);
  tft.drawLine(x + 28, y + 40 , x+1, y + 40, COLOR_WHITE);
  tft.drawLine(x + 1, y + 40 , x+1, y + 20, COLOR_WHITE);
  tft.drawLine(x + 1, y + 20 , x+7, y + 15, COLOR_WHITE);
  tft.drawLine(x + 7, y + 15 , x+7, y + 6, COLOR_WHITE);
  tft.fillRectangle(x+10, y+10, x+12, y+15, COLOR_WHITE);
  tft.fillRectangle(x+14, y+10, x+16, y+15, COLOR_WHITE);
  tft.fillRectangle(x+18, y+10, x+20, y+15, COLOR_WHITE);
  tft.fillRectangle(x+22, y+10, x+24, y+15, COLOR_WHITE);
//  tft.fillRectangle(x+18, y+10, x+20, y+15, COLOR_WHITE);
}

void loop() {

}
