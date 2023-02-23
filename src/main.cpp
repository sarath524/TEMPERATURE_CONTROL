// Include application, user and local libraries
//tft display updated
//.
#include "SPI.h"
#include "TFT_22_ILI9225.h"

#include "FreeSans24pt7b.h"
#include <math.h>
#include <Arduino_BuiltIn.h>

#define TFT_RST 26  // IO 26
#define TFT_RS  25  // IO 25
#define TFT_CLK 14  // HSPI-SCK
#define TFT_SDI 13  // HSPI-MOSI
#define TFT_CS  15  // HSPI-SS0
#define TFT_LED 0   // 0 if wired to +5V directly
SPIClass hspi(HSPI);

char temp_value0[50];
char temp_value1[50];
char temp_value2[50];
char Avg_Value[50];

// int Rntc0, Temp0,Temp1,Temp2,avg_temp, Rntc1,Rntc2;

int sensor_analog_pin0 = 34, sensor_analog_pin1 = 2, sensor_analog_pin2= 0;
// int sensor_value0 = 0,  sensor_value1 = 0, sensor_value2 = 0; 
// int sensor_analogue0 = 0, sensor_analogue1 = 0, sensor_analogue2 = 0;

float V1, ln0,V2,V3,V4,V5,V6,ln1,ln2;

 float Rntc0, Temp0,Temp1,Temp2,Rntc1,Rntc2,avg_temp;
#define TFT_BRIGHTNESS 200 // Initial brightness of TFT backlight (optional)

TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, TFT_BRIGHTNESS);

void setup() 
{
  hspi.begin();
  tft.begin(hspi);
  Serial.begin(9600);

  pinMode(sensor_analog_pin0, INPUT);
  pinMode(sensor_analog_pin1, INPUT);
  pinMode(sensor_analog_pin2, INPUT);
}

void temp_sensor_voltage() {
  int sensor_value0 = 0;
  int sensor_value1 = 0;
  int sensor_value2 = 0;

  int sensor_analogue0 = 0;
  int sensor_analogue1 = 0;
  int sensor_analogue2 = 0;
  float V1, ln0,V2,V3,V4,V5,V6,ln1,ln2;
 

  for (int i = 0; i < 50; i++) {
  sensor_value0 = sensor_value0 + analogRead(sensor_analog_pin0);
  sensor_analogue0=sensor_value0/50;
}
for (int i = 0; i < 50; i++) {
  sensor_value1 = sensor_value1 + analogRead(sensor_analog_pin1);
  sensor_analogue1=sensor_value1/50;
}
for (int i = 0; i < 50; i++) {
  sensor_value2 = sensor_value2 + analogRead(sensor_analog_pin2);
  sensor_analogue2=sensor_value2/50;
}

  //sensor_analogue = analogRead(sensor_analog_pin);
  // sensor_analogue = sensor_value / 5;

  V1 =sensor_analogue0 * (3.3 / 4095.00);
  V2= V1/1.34;
  Rntc0 = ((3.3 / V2) - 1) * 15000;
  ln0 = log(Rntc0/ 10000);
  Temp0 = (1 / ((ln0 / 3977) + (1 / 298.15))) - 273.15;
  Serial.println(sensor_analogue0);
  delay(1000);

  V3=sensor_analogue1 * (3.3 / 4095.00);
  V4= V3/1.34;
  Rntc1 = ((3.3 / V4) - 1) * 15000;
  ln1 = log(Rntc1/ 10000);
  Temp1 = (1 / ((ln1 / 3977) + (1 / 298.15))) - 273.15;
  Serial.println(sensor_analogue1);
  delay(1000);
  

  V5 =sensor_analogue2 * (3.3/ 4095.00);
  V5= V5/1.34;
  Rntc2 = ((3.3 / V5) - 1) * 15000;
  ln2 = log(Rntc2/ 10000);
  Temp2 = (1 / ((ln2 / 3977) + (1 / 298.15))) - 273.15;
  Serial.println(sensor_analogue2);
  delay(1000);
  
  
if (Temp0>0)
Temp0=Temp0+13;
else if (Temp0<0)
Temp0 = Temp0+12.15;
else if (Temp0=0) 
Temp0=0; 
Serial.println(Temp0);

  
if (Temp1>0)
Temp1=Temp1+13;
else if (Temp1<0)
Temp1 = Temp1+12.15;
else if (Temp1=0) 
Temp1=0; 
Serial.println(Temp1);

  
if (Temp2>0)
Temp2=Temp2+13;
else if (Temp2<0)
Temp2 = Temp2+12.15;
else if (Temp2=0) 
Temp2=0; 
Serial.println(Temp2);

avg_temp=Temp1+Temp2;
avg_temp= avg_temp/2;
 

}


void display()
 {
    dtostrf(Temp0,5,2,temp_value0);

    dtostrf(Temp1,5,2,temp_value1 );

    dtostrf(Temp2,5,2,temp_value2);

    dtostrf(avg_temp,5,2,Avg_Value);

    tft.setFont(Terminal12x16);
    tft.setOrientation(3);
    tft.setGFXFont(&FreeSans24pt7b);
    tft.clear();
    tft.drawGFXText(30, 115, Avg_Value, COLOR_WHITE);
    tft.drawCircle(162, 85, 3, COLOR_WHITE);
    tft.drawGFXText(165, 115 , "C",COLOR_WHITE);
    tft.drawText(130 , 34 ,temp_value0, COLOR_WHITE);
    tft.drawCircle(197, 33, 2, COLOR_WHITE);
    tft.drawText(200, 34 , "C");
    tft.drawText(150, 50 , "AMB");
    tft.setFont(Terminal6x8);
    tft.drawText(120, 130 , "TOP ");
    tft.drawText(150, 130 , temp_value1);
    tft.drawCircle(188, 131, 2, COLOR_WHITE);
    tft.drawText(190, 130 , "C");
    tft.drawText(120, 150 , "BOT ");
    tft.drawText(150,150,temp_value2);
    tft.drawCircle(188, 151, 2, COLOR_WHITE);
    tft.drawText(190, 150 ,"C");

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

 
  temp_sensor_voltage();
  display();
  delay(10);
}