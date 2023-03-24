// Include application, user and local libraries
#include "SPI.h"
#include "TFT_22_ILI9225.h"
#include "FreeSans24pt7b.h"
#include<Wire.h>

#include "RTClib.h"

char date[10] = "hh:mm:ss";
RTC_DS1307 RTC;

#define TFT_RST 26
#define TFT_RS  25
#define TFT_CS  15  // SS
#define TFT_SDI 13  // MOSI
#define TFT_CLK 14  // SCK
#define TFT_LED -1 

static char ambient_temperature_str[50];
   // 0 if wired to +5V directly
SPIClass hspi(HSPI);

#define TFT_BRIGHTNESS 200 // Initial brightness of TFT backlight (optional)

TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_BRIGHTNESS);

#define SPI_DEFAULT_FREQ         10000000
static char month_str[30];
static char day_str[50];
static char year_str[50];
static char hour_str[50];
static char minute_str[50];
static char second_str[50];
int month,day,year,hour,minute,second;
void setup() {
  
  hspi.begin();
  tft.begin(hspi);
  Wire.begin();
  RTC.begin(); 
//  tft.begin();
  Serial.begin(9600);
  digitalWrite(TFT_RS,HIGH);
  //tft.drawRectangle(0, 0, tft.maxX() - 1, tft.maxY() - 1, COLOR_WHITE);
  tft.setFont(Terminal12x16);
  tft.setOrientation(3);
 // tft.drawText(0, 3, "16.FEB.23");
 // tft.drawText(0, 30, "4:41 PM");


  
//  tft.setGFXFont(&FreeSans24pt7b);
//  tft.drawGFXText(20, 140, "3 C", COLOR_WHITE);
//  tft.drawCircle(58, 110, 2, COLOR_WHITE);
//  tft.drawText(110, 75 , "32 C");
//   tft.drawCircle(141, 75, 2, COLOR_WHITE);
//  tft.drawText(110, 90 , "AMB");
//
//  int x = 191;
//  int y = 5;
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

}

void loop() {
  DateTime now = RTC.now();
       month=now.month();
       day=now.day();
       year=now.day();
       hour=now.day();
       minute=now.day();
       second=now.day();
 

//    Serial.print(now.month());
//     
  dtostrf(month,5,0,month_str);
  dtostrf(day,5,0,day_str);
  dtostrf(year,5,0,year_str);
  dtostrf(hour,5,0,hour_str);
  dtostrf(minute,5,0,minute_str);
  dtostrf(second,5,0,second_str);
  
   tft.drawText(0, 0 ,month_str );
  tft.drawText(0, 2 ,day_str );
  tft.drawText(0, 4 ,year_str );
  tft.drawText(2, 0 ,hour_str );
  tft.drawText(2, 2 ,minute_str );
  tft.drawText(2, 4 ,second_str );
  
   delay(2000);
    tft.clear();
}
