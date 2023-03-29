// Include application, user and local libraries
#include "SPI.h"
#include "TFT_22_ILI9225.h"
#include "FreeSans24pt7b.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"


char date[10] = "hh:mm:ss";


#define TFT_RST 26
#define TFT_RS  25
#define TFT_CS  15  // SS
#define TFT_SDI 13 / MOSI
#define TFT_CLK 14  / // SCK
#define TFT_LED -1 

#define SCK  14
#define MISO  12
#define MOSI  13
#define CS  5

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

SPIClass spi = SPIClass(HSPI);


void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}


void setup() {
  
  Serial.begin(115200);
  hspi.begin();
  tft.begin(hspi);
  spi.begin(SCK, MISO, MOSI, CS);
    if(!SD.begin(CS,spi,80000000)){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }


  digitalWrite(TFT_RS,HIGH);
  tft.setFont(Terminal12x16);
  tft.setOrientation(3);

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

  writeFile(SD, "/hello.txt", "Hello ");

}

void loop() {
//  tft.setFont(Terminal6x8);
//  DateTime now = RTC.now();
//       month=now.month();
//       day=now.day();
//       year=now.day();
//       hour=now.day();
//       minute=now.day();
//       second=now.day();
// 
//
//    Serial.print(now.month());
//    Serial.print(now.Serial.print(now.month());());
//    Serial.print(now.month());
//    Serial.print(now.month());
//    Serial.print(now.month());
//    Serial.print(now.month());
//    
////     
//  dtostrf(month,5,0,month_str);
//  dtostrf(day,5,0,day_str);
//  dtostrf(year,5,0,year_str);
//  dtostrf(hour,5,0,hour_str);
//  dtostrf(minute,5,0,minute_str);
//  dtostrf(second,5,0,second_str);
//  
//   tft.drawText(0, 0 ,month_str );
//  tft.drawText(0, 2 ,day_str );
//  tft.drawText(0, 4 ,year_str );
//  tft.drawText(2, 0 ,hour_str );
//  tft.drawText(2, 2 ,minute_str );
//  tft.drawText(2, 4 ,second_str );
//  
//   delay(2000);
//    tft.clear();
}
