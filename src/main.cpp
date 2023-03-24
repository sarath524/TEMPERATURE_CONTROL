// Include application, user and local libraries
//tft display updated
//.
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "TFT_22_ILI9225.h"

#include<Wire.h>
#include "RTClib.h"
RTC_DS1307 RTC;

#include "FreeSans24pt7b.h"
#include <math.h>
#include <Arduino_BuiltIn.h>

#define AMBIENT_SENSOR_PIN 34
#define TOP_SENSOR_PIN 39
#define BOTTOM_SENSOR_PIN 2

#define RED_LED 19
#define GREEN_LED 23
#define BLUE_LED 18
const int LIMIT_SWITCH = 35; 
int buzzerPin = 4;

#define PWM_PIN 27 
const int freq = 500;
const int ledChannel = 0;
const int resolution = 8;
#define SHUNT_RESIST_FB_PIN 12                                                                                                                               
#define LOW_CURRENT_MODE 1.75
#define MED_CURRENT_MODE 3
#define HIGH_CURRENT_MODE 4
#define MAX_PWM 82
#define MIN_PWM 92

#define TFT_RST 26  // IO 26
#define TFT_RS  25  // IO 25
#define TFT_CLK 14  // HSPI-SCK
#define TFT_SDI 13  // HSPI-MOSI
#define TFT_CS  15  // HSPI-SS0
#define TFT_LED 0   // 0 if wired to +5V directly
#define TFT_BRIGHTNESS 200 // Initial brightness of TFT backlight (optional)

unsigned long previousMillis = 0;
unsigned long previousMillis1 = 0; // will store last time LED1 was updated

int buttonState;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 6000;
uint32_t Feedback_Value = 0;
int i_expected;
int i_actual;
int dutycycle = 81;

SPIClass hspi(HSPI);
TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, TFT_BRIGHTNESS);

static char ambient_temperature_str[50];
static char top_temperature_str[50];
static char bottom_temperature_str[50];
static char chamber_temperature_str[50];
static char month_str[10];
static char day_str[10];
static char year_str[10];
static char hour_str[10];
static char minute_str[10];
static char second_str[10];

float ambient_temperature,top_temperature,bottom_temperature,chamber_temperature;
int month,day,year,hour,minute,second;

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

void get_rtc(){
  
      DateTime now = RTC.now();
       month=now.month();
       day=now.day();
       year=now.year();
       hour=now.hour();
       minute=now.minute();
       second=now.second();

}

void display()
{
  dtostrf(ambient_temperature,5,1,ambient_temperature_str);
  dtostrf(top_temperature,5,1,top_temperature_str );
  dtostrf(bottom_temperature,5,1,bottom_temperature_str);
  dtostrf(chamber_temperature,5,1,chamber_temperature_str);
  dtostrf(month,2,0,month_str);
  dtostrf(day,2,0,day_str);
  dtostrf(year,3,0,year_str);
  dtostrf(hour,2,0,hour_str);
  dtostrf(minute,2,0,minute_str);
  dtostrf(second,2,0,second_str);
  
 
  tft.setFont(Terminal12x16);
 
  // tft.drawText(3, 12 ,year_str );
  // tft.drawText(6, 4 ,hour_str );
  // tft.drawText(6, 8 ,minute_str );
  // tft.drawText(6, 12 ,second_str );
  tft.setOrientation(3);
  tft.setGFXFont(&FreeSans24pt7b);
  //tft.fillRectangle(120, 30, 140, 35, COLOR_BLACK);
 // tft.clear();
  tft.drawGFXText(30, 115, chamber_temperature_str, COLOR_WHITE);
  tft.drawCircle(162, 85, 3, COLOR_WHITE);
  tft.drawGFXText(165, 115 , "C",COLOR_WHITE);
 // tft.fillRectangle (30, 80, 200, 150, COLOR_BLACK);
  tft.drawText(130 , 34 ,ambient_temperature_str, COLOR_WHITE);
  tft.drawCircle(197, 33, 2, COLOR_WHITE);
  tft.drawText(200, 34 , "C");
  tft.drawText(150, 50 , "AMB");
  
  tft.setFont(Terminal6x8);
  tft.drawText(0, 4 ,month_str );
  tft.drawText(15, 4 ,"/" );
  tft.drawText(25, 4 ,day_str );
  tft.drawText(40, 4 ,"/" );
  tft.drawText(50, 4 ,year_str );
  tft.drawText(0, 24 ,hour_str );
  tft.drawText(15, 24 ,":" );
  tft.drawText(25, 24 ,minute_str );
  tft.drawText(38, 24 ,":" );
  tft.drawText(48, 24 ,second_str );
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

void writeFile(fs::FS &fs, const char * path, const char * message)
{
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

void setup() 
{
  hspi.begin();
  tft.begin(hspi);
  Serial.begin(115200);
  Wire.begin();
   RTC.begin();

  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(PWM_PIN, ledChannel);


  pinMode(AMBIENT_SENSOR_PIN, INPUT);
  pinMode(TOP_SENSOR_PIN, INPUT);
  pinMode(BOTTOM_SENSOR_PIN, INPUT);

  pinMode(LIMIT_SWITCH, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  digitalWrite(buzzerPin,HIGH); 
   // if(!SD.begin()){
  //     Serial.println("Card Mount Failed");
  //     return;
  // }
  // uint8_t cardType = SD.cardType();

  // if(cardType == CARD_NONE){
  //     Serial.println("No SD card attached");
  //     return;
  // }

 }

void loop() 
{ 
  
  unsigned long currentMillis = millis();

  if ((currentMillis - previousMillis) >= 1000)
  {
   int analog_value = get_sensor_average(AMBIENT_SENSOR_PIN);
  ambient_temperature = convert_to_temperature(analog_value);
  
  analog_value = get_sensor_average(TOP_SENSOR_PIN);
  top_temperature = convert_to_temperature(analog_value);
  
  analog_value = get_sensor_average(BOTTOM_SENSOR_PIN);
  bottom_temperature = convert_to_temperature(analog_value);
  
  chamber_temperature = (top_temperature + bottom_temperature) /2;


  previousMillis = currentMillis;
  Serial.print(chamber_temperature);
  get_rtc();
  display();
  }


  // if(chamber_temperature <=2)
  //     ledcWrite(ledChannel, 77);

  // else if (chamber_temperature >=2 && chamber_temperature<=4)
  // {
  //    ledcWrite(ledChannel, 81);
  //   // Blue Led    
  //     digitalWrite(RED_LED, HIGH); 
  //     digitalWrite(GREEN_LED, HIGH);// turn the LED on (HIGH is the voltage level)                    // wait for a second
  //     digitalWrite(BLUE_LED, LOW);
  // }
  // else if(chamber_temperature>4)
  // {
  //      ledcWrite(ledChannel, 92);
  //   // Red Led
  //     //tone(buzzerPin,1000,500);
  //     digitalWrite(RED_LED, HIGH);
  //     digitalWrite(GREEN_LED, LOW);// turn the LED on (HIGH is the voltage level                    // wait for a second
  //     digitalWrite(BLUE_LED,HIGH);
  // }
  

  int reading = digitalRead(LIMIT_SWITCH);
  if (reading != lastButtonState ) 
  {
    // reset the debouncing 
          digitalWrite(buzzerPin,LOW);
          noTone(buzzerPin);
    Serial.println("limitswitch is stopped");
    
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) 
  {
    if (reading != buttonState)
    {
      buttonState = reading;
     }
      if (buttonState == LOW) {
        Serial.println("limitswitch is pressed");
        digitalWrite(buzzerPin,HIGH);
        tone(buzzerPin,784);
        buttonState = HIGH;
      }
    
         }
    
  //   lastButtonState = reading;
//storing the chamber temperature value every minute

  // if (currentMillis - previousMillis1 >= 6000) 
  // {

  //   writeFile(SD, "/phloton.txt",chamber_temperature_str); 
  //   previousMillis1 = currentMillis;
  // }
  }
  



  


