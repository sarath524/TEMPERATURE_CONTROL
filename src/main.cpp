// Include application, user and local libraries
//tft display updated
//.
#include "SPI.h"
#include "TFT_22_ILI9225.h"

#include "FreeSans24pt7b.h"
#include <math.h>
#include <Arduino_BuiltIn.h>

#define AMBIENT_SENSOR_PIN 34
#define TOP_SENSOR_PIN 2
#define BOTTOM_SENSOR_PIN 0

#define PWM_PIN 5
#define SHUNT_RESIST_FB_PIN 35                                                                                                                               

#define TFT_RST 26  // IO 26
#define TFT_RS  25  // IO 25
#define TFT_CLK 14  // HSPI-SCK
#define TFT_SDI 13  // HSPI-MOSI
#define TFT_CS  15  // HSPI-SS0
#define TFT_LED 0   // 0 if wired to +5V directly
#define TFT_BRIGHTNESS 200 // Initial brightness of TFT backlight (optional)

uint32_t Feedback_Value = 0;

SPIClass hspi(HSPI);
TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, TFT_BRIGHTNESS);

static char ambient_temperature_str[50];
static char top_temperature_str[50];
static char bottom_temperature_str[50];
static char chamber_temperature_str[50];

float ambient_temperature,top_temperature,bottom_temperature,chamber_temperature;

float convert_to_temperature(int analogValue)
{
  float coeff_A1 = 0.00335401643468053;
  float coeff_B1 = 0.00025698501802;
  float coeff_C1 = 0.0000026201306709;
  float coeff_D1 = 0.000000063830907998;
  int R25 = 10000;
  float V1 = analogValue * (3.30 / 4095.00);
  float V2 = ((10 * V1) + (1.63 * 3.4)) / 13.4;
  float Rntc0 = ((3.3 / V2) - 1) * 15000;
  float X = Rntc0 / R25;
  float temp = log(X);
  float result = coeff_A1 + coeff_B1 * temp + coeff_C1 * pow(temp, 2) + coeff_D1 * pow(temp, 3);
  return ((1 / result)-273.15);
}

int get_sensor_average(byte sensorPin)
{
  int sensor_value = 0;
  for (int i = 0; i < 50; i++) 
  {
    sensor_value += analogRead(sensorPin);
  }
  return (int)(sensor_value/50);
}

void get_feedback_value()
{
  for(int i;  i< 10; i++){
  Feedback_Value = analogRead(SHUNT_RESIST_FB_PIN) ;
  }
  Feedback_Value = Feedback_Value / 10;
  

0
}
void display()
{
  dtostrf(ambient_temperature,5,2,ambient_temperature_str);

  dtostrf(top_temperature,5,2,top_temperature_str );

  dtostrf(bottom_temperature,5,2,bottom_temperature_str);

  dtostrf(chamber_temperature,5,2,chamber_temperature_str);

  tft.setFont(Terminal12x16);
  tft.setOrientation(3);
  tft.setGFXFont(&FreeSans24pt7b);
   tft.clear();
  tft.drawGFXText(30, 115, chamber_temperature_str, COLOR_WHITE);
  tft.drawCircle(162, 85, 3, COLOR_WHITE);
  tft.drawGFXText(165, 115 , "C",COLOR_WHITE);
  tft.drawText(130 , 34 ,ambient_temperature_str, COLOR_WHITE);
  tft.drawCircle(197, 33, 2, COLOR_WHITE);
  tft.drawText(200, 34 , "C");
  tft.drawText(150, 50 , "AMB");
  tft.setFont(Terminal6x8);
  tft.drawText(120, 130 , "TOP ");
  tft.drawText(150, 130 , top_temperature_str);
  tft.drawCircle(188, 131, 2, COLOR_WHITE);
  tft.drawText(190, 130 , "C");
  tft.drawText(120, 150 , "BOT ");
  tft.drawText(150,150,bottom_temperature_str);
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

void setup() 
{
  hspi.begin();
  tft.begin(hspi);
  Serial.begin(9600);

  pinMode(AMBIENT_SENSOR_PIN, INPUT);
  pinMode(TOP_SENSOR_PIN, INPUT);
  pinMode(BOTTOM_SENSOR_PIN, INPUT);
}

void loop() { 0
  int analog_value = get_sensor_average(AMBIENT_SENSOR_PIN);
  ambient_temperature = convert_to_temperature(analog_value);
  
  analog_value = get_sensor_average(TOP_SENSOR_PIN);
  top_temperature = convert_to_temperature(analog_value);
  
  analog_value = get_sensor_average(BOTTOM_SENSOR_PIN);
  bottom_temperature = convert_to_temperature(analog_value);
  
  chamber_temperature = (top_temperature + bottom_temperature) /2;
  display();
  delay(1000);
}