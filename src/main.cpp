// Include application, user and local libraries
// tft display updated
//.
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include "TFT_22_ILI9225.h"

#include "OneButton.h"

#include <Wire.h>
#include "RTClib.h"

#include <math.h>
#include <Arduino_BuiltIn.h>

#include <NimBLEDevice.h>

BLEServer* pServer = NULL;
BLECharacteristic* pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#define AMBIENT_SENSOR_PIN 34
#define TOP_SENSOR_PIN 39
#define BOTTOM_SENSOR_PIN 2

#define RED_LED 19
#define GREEN_LED 23
#define BLUE_LED 18

#define PWM_PIN 27
#define SHUNT_RESIST_FB_PIN 12

#define TFT_RST 26         // IO 26
#define TFT_RS 25          // IO 25
#define TFT_CLK 14         // HSPI-SCK
#define TFT_SDI 13         // HSPI-MOSI
#define TFT_CS 15          // HSPI-SS0
#define TFT_LED 0          // 0 if wired to +5V directly
#define TFT_BRIGHTNESS 200 // Initial brightness of TFT backlight (optional)

#define SCK  14
#define MISO  12
#define MOSI  13
#define CS  5
#define TEST
//#define FINAL
bool rtc_status;
bool sd_status;
SPIClass spi = SPIClass(HSPI);

const int LIMIT_SWITCH = 35;
int buzzerPin = 4;
const int freq = 500;
const int ledChannel = 0;
const int resolution = 8;

unsigned long previousMillis = 0;
unsigned long previousMillis1 = 0;

byte Bat_Bar =3;
byte Voltage=3.4;
byte Current=1.2;
int x; 
int y;

uint32_t Feedback_Value = 0;
int i_expected;
int i_actual;
int dutycycle = 81;

RTC_DS1307 RTC;
SPIClass hspi(HSPI);
TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, TFT_BRIGHTNESS);

OneButton button(LIMIT_SWITCH, true);

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
static char temp[10]=" 00";

float ambient_temperature, top_temperature, bottom_temperature, chamber_temperature;
int month, day, year, hour, minute, second;

class MyServerCallbacks: public BLEServerCallbacks
 {
    void onConnect(BLEServer* pServer) 
    {
      deviceConnected = true;
    };
    void onDisconnect(BLEServer* pServer) {

      deviceConnected = false;
    }
};

bool send_data_flag = false;
String init_data = "";
std::string tx_data = "";

bool BLE_device_command(String inCmd)
{

  if (inCmd.indexOf("BT") >= 0)
  {
    init_data = "h\0";
    return 1;
  }
  else if (inCmd.indexOf("AT") >= 0)
  {
    init_data = "Cm\0";
    return 1;
  }
  else if (inCmd.indexOf("VT") >= 0)
  {
    init_data = "l\0";
    return 1;
  }
  else if (inCmd.indexOf("CT") >= 0)
  {
    init_data = "0\0";
    return 1;
  }
  else
  {
    init_data = "invalidCmd";
    return 1;
  }

}

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string rxValue = pCharacteristic->getValue();
    String rx_data = "";

    if (rxValue.length() > 0)
    {
      for (int i = 0; i < rxValue.length(); i++)
        rx_data += rxValue[i];
    }
    Serial.println(rx_data);
    bool ret = BLE_device_command(rx_data);

    if (ret == 1)
      send_data_flag = true;
  }
};

void init_BLE()
{
  // Create the BLE Device
  BLEDevice::init("Phloton");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_TX,
      NIMBLE_PROPERTY::NOTIFY);

  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_RX,
      NIMBLE_PROPERTY::WRITE);

  pServer->setCallbacks(new MyServerCallbacks());
  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}


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
  return ((1 / result) - 273.15);
}

int get_sensor_average(byte sensorPin)
{
  int sensor_value = 0;
  for (int i = 0; i < 50; i++)
  {
    sensor_value += analogRead(sensorPin);
  }
  return (int)(sensor_value / 50);
}

void get_rtc()
{
  //getting month,day,year,hour,minute,sec using functions
  DateTime now = RTC.now();
  month = now.month();
  day = now.day();
  year = now.year();
  hour = now.hour();
  minute = now.minute();
  second = now.second();
}

void blu_logoon()
{
  x = 175;
  y = 3;
  tft.drawLine(x + 3, y, x + 6, y + 3, COLOR_WHITE);
  tft.drawLine(x + 6, y + 3, x + 3, y + 6, COLOR_WHITE);
  tft.drawLine(x + 3, y + 6, x + 6, y + 9, COLOR_WHITE);
  tft.drawLine(x + 6, y + 9, x + 3, y + 12, COLOR_WHITE);
  tft.drawLine(x + 3, y, x + 3, y + 12, COLOR_WHITE);
  tft.drawLine(x, y + 3, x + 3, y + 6, COLOR_WHITE);
  tft.drawLine(x + 3, y + 6, x, y + 9, COLOR_WHITE);
}

void blu_logooff(){

 tft.fillRectangle(x-18,y-2,x-3,y+10,COLOR_BLACK);
         
}

void time(){
  tft.drawText(0, 4, month_str);
  tft.drawText(15, 4, "/");
  tft.drawText(25, 4, day_str);
  tft.drawText(40, 4, "/");
  tft.drawText(50, 4, year_str);
  tft.drawText(0, 24, hour_str);
  tft.drawText(15, 24, ":");
  tft.drawText(25, 24, minute_str);
  tft.drawText(38, 24, ":");
  tft.drawText(48, 24, second_str);
}

void display()
{
  dtostrf(ambient_temperature, 5, 1, ambient_temperature_str);
  dtostrf(top_temperature, 5, 1, top_temperature_str);
  dtostrf(bottom_temperature, 5, 1, bottom_temperature_str);
  dtostrf(chamber_temperature , 5, 1, chamber_temperature_str);
  sprintf(month_str, "%02d", month);
  sprintf(day_str, "%02d", day);
  sprintf(year_str, "%04d", year);
  sprintf(hour_str, "%02d", hour);
  sprintf(minute_str, "%02d", minute);
  sprintf(second_str, "%02d", second);

  tft.setOrientation(3);
  tft.setFont(Terminal22x32);
  tft.drawText(10, 93, chamber_temperature_str, COLOR_WHITE);
  tft.drawCircle(132, 89, 3, COLOR_WHITE);
  tft.drawText(135, 93, "C", COLOR_WHITE);
  tft.setFont(Terminal12x16);
  tft.drawText(130, 44, ambient_temperature_str, COLOR_WHITE);
  tft.drawCircle(197, 43, 2, COLOR_WHITE);
  tft.drawText(200, 44, "C");
  tft.drawText(150, 70, "AMB");

  tft.setFont(Terminal6x8);


  if(rtc_status == true)
  {
     time();
  }
  else {
   tft.drawText(36, 20, "CLOCK");
   tft.drawText(15, 30, "NOT DETECTED");
   }
  tft.drawText(120, 150, "TOP ");
  tft.drawText(150, 150, top_temperature_str);
  tft.drawCircle(188, 151, 2, COLOR_WHITE);
  tft.drawText(190, 150, "C");
  tft.drawText(120, 165, "BOT ");
  tft.drawText(150, 165, bottom_temperature_str);
  tft.drawCircle(188, 166, 2, COLOR_WHITE);
  tft.drawText(190, 165, "C");

  x = 191;
  y = 5;
  tft.drawRectangle(x, y, x + 22, y + 10, COLOR_WHITE);
  tft.fillRectangle(x + 2, y + 2, x + 5, y + 8, COLOR_WHITE);
  tft.fillRectangle(x + 7, y + 2, x + 10, y + 8, COLOR_WHITE);
  tft.fillRectangle(x + 12, y + 2, x + 15, y + 8, COLOR_WHITE);
  tft.fillRectangle(x + 17, y + 2, x + 20, y + 8, COLOR_WHITE);
  tft.fillRectangle(x + 22, y + 3, x + 24, y + 6, COLOR_WHITE);

  if (deviceConnected)
  {
    blu_logoon();
  }
  else
  {
    
    blu_logooff();
  }

}

void writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    Serial.println("File written");
  }
  else
  {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

bool buzzerStatus = false;
unsigned long buzzerStartTime = 0;
int open_1 = 1;
int close1 = 1;


void runBuzzer()
{
  if (!buzzerStatus)
  {
    buzzerStatus = true;
    buzzerStartTime = millis();
  }
  if ((millis() - buzzerStartTime) > 6000 && buzzerStatus)
  {
    close1 = 1;
    while (open_1 == 1)
      {
        Serial.println("  Lid is Open");
        appendFile(SD, "/phloton.txt",hour_str);
        appendFile(SD, "/phloton.txt"," :");
        appendFile(SD, "/phloton.txt",minute_str);
        appendFile(SD, "/phloton.txt"," :");
        appendFile(SD, "/phloton.txt",second_str);
        appendFile(SD, "/phloton.txt","->  ");
        appendFile(SD, "/phloton.txt",chamber_temperature_str);
        appendFile(SD, "/phloton.txt",", Lid is Open ");
        appendFile(SD, "/phloton.txt","\n");
        open_1--;
      }
    
    analogWrite(buzzerPin, 128);
  }
    
}

void stopBuzzer()
{
   buzzerStatus = false;
   open_1=1;
    while (close1 == 1)
      {
        Serial.println(" started storing data of Lid close");
        appendFile(SD, "/phloton.txt","Lid close ");
        appendFile(SD, "/phloton.txt",hour_str);
        appendFile(SD, "/phloton.txt"," :");
        appendFile(SD, "/phloton.txt",minute_str);
        appendFile(SD, "/phloton.txt"," :");
        appendFile(SD, "/phloton.txt",second_str);
        appendFile(SD, "/phloton.txt","->  ");
        appendFile(SD, "/phloton.txt",chamber_temperature_str);
        appendFile(SD, "/phloton.txt",", Lid is Close");
        appendFile(SD, "/phloton.txt","\n");
        close1--;
      }
  analogWrite(buzzerPin, 0);
}

void sd_card_init()
{
sd_status=SD.begin(CS,spi,80000000);

    if(!sd_status){
        Serial.println("Card Mount Failed");
        tft.setOrientation(3);
        tft.setFont(Terminal6x8);
        int a=40;
        int b=150;
        tft.drawRectangle(a, b, a+20,b+10, COLOR_WHITE);
        tft.drawText(a+4, b+2, "SD");
        tft.drawLine( a,b-2,a+22,b+12,COLOR_WHITE);
    }
     uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        
    }
}

void setup()
{
  Serial.begin(115200);
  hspi.begin();
  tft.begin(hspi);
  spi.begin(SCK, MISO, MOSI, CS);
  sd_card_init();
   
  Wire.begin();
  rtc_status= RTC.begin();

  init_BLE();

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
  digitalWrite(buzzerPin, LOW);
  button.setPressTicks(100);
  button.attachDuringLongPress(runBuzzer);
  button.attachLongPressStop(stopBuzzer);
  
  #ifdef TEST
  appendFile(SD, "/phloton.txt","Time             ");
  appendFile(SD, "/phloton.txt","chamber_temp   ");
  appendFile(SD, "/phloton.txt","top_temp   ");
  appendFile(SD, "/phloton.txt","bot_temp");
  appendFile(SD, "/phloton.txt","\n");
  #endif


}

void loop()
{
   // getting Top,Bottom,Ambient temp value every one sec
  unsigned long currentMillis = millis();
  
  if ((currentMillis - previousMillis) >= 1000)
  {
    int analog_value = get_sensor_average(AMBIENT_SENSOR_PIN);
    ambient_temperature = convert_to_temperature(analog_value);

    analog_value = get_sensor_average(TOP_SENSOR_PIN);
    top_temperature = convert_to_temperature(analog_value);

    analog_value = get_sensor_average(BOTTOM_SENSOR_PIN);
    bottom_temperature = convert_to_temperature(analog_value);

    chamber_temperature = (top_temperature + bottom_temperature) / 2;

    previousMillis = currentMillis;
    Serial.println(chamber_temperature);
    Serial.println(day_str);
  // using temperature_value controlling the constant_current in the container 
  #ifdef TEST
    if (chamber_temperature <= 2)
      ledcWrite(ledChannel, 77);//

    else if (chamber_temperature >= 2 && chamber_temperature <= 4)
    {
      ledcWrite(ledChannel, 81);//in this pwm write the current will maintain in 1.2 amps 
      // Blue Led
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, HIGH); // turn the LED on (HIGH is the voltage level)                    // wait for a second
      digitalWrite(BLUE_LED, LOW);
    }
    else if (chamber_temperature > 4)
    {
      ledcWrite(ledChannel, 92);//in this pwm write the current will maintain in 4 amps for pre_cooling 
      // Red Led
      // tone(buzzerPin,1000,500);
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW); // turn the LED on (HIGH is the voltage level                    // wait for a second
      digitalWrite(BLUE_LED, HIGH);
    }
    
      
  }
  #endif
  
  display();
  get_rtc();
  button.tick();
  
 // storing the chamber temperature value every minute in SD card
  #ifdef TEST
      if (currentMillis - previousMillis1 >= 30000)
      { 
        if(rtc_status==false)
        {       
            appendFile(SD, "/phloton.txt",temp);
            appendFile(SD, "/phloton.txt"," :");
            appendFile(SD, "/phloton.txt",temp);
            appendFile(SD, "/phloton.txt"," :");
            appendFile(SD, "/phloton.txt",temp);
            }
          appendFile(SD, "/phloton.txt",hour_str);
          appendFile(SD, "/phloton.txt"," :");
          appendFile(SD, "/phloton.txt",minute_str);
          appendFile(SD, "/phloton.txt"," :");
          appendFile(SD, "/phloton.txt",second_str);
          appendFile(SD, "/phloton.txt","->  ");
          appendFile(SD, "/phloton.txt","    ");
          appendFile(SD, "/phloton.txt",chamber_temperature_str);
          appendFile(SD, "/phloton.txt","        ");
          appendFile(SD, "/phloton.txt",top_temperature_str);
          appendFile(SD, "/phloton.txt","      ");
          appendFile(SD, "/phloton.txt",bottom_temperature_str);
          appendFile(SD, "/phloton.txt","\n");
          previousMillis1 = currentMillis;
      }
  #endif


  #ifdef FINAL
      if (currentMillis - previousMillis1 >= 60000)
      {
        if(rtc_status==false)
        {       
            appendFile(SD, "/phloton.txt",temp);
            appendFile(SD, "/phloton.txt"," :");
            appendFile(SD, "/phloton.txt",temp);
            appendFile(SD, "/phloton.txt"," :");
            appendFile(SD, "/phloton.txt",temp);
            }
        appendFile(SD, "/phloton.txt",hour_str);
        appendFile(SD, "/phloton.txt"," :");
        appendFile(SD, "/phloton.txt",minute_str);
        appendFile(SD, "/phloton.txt"," :");
        appendFile(SD, "/phloton.txt",second_str);
        appendFile(SD, "/phloton.txt","->  ");
        appendFile(SD, "/phloton.txt","    ");
        appendFile(SD, "/phloton.txt",chamber_temperature_str);
        appendFile(SD, "/phloton.txt","\n");
        previousMillis1 = currentMillis;
      }
  #endif

// sending cham_tem,atm_temp,Bat_level,Volt,current to the bluetooth app
  if (deviceConnected)
   {
    if(send_data_flag)
    {
      if(init_data == "Cm\0")
      {
        init_data.remove(1);
        init_data += chamber_temperature;
        init_data += "Ab";
        init_data += ambient_temperature;
        init_data += "Bt ";
        init_data += Bat_Bar;
      }
      else 
      {
        init_data.remove(1);
        init_data += "V ";
        init_data += Voltage;
        init_data += "C ";
        init_data += Current;
      }

      for (int i = 0; i < init_data.length(); i++)
            tx_data += init_data[i];
   
          pTxCharacteristic->setValue(tx_data); // Notify fromSerial.
          pTxCharacteristic->notify();
          send_data_flag = false;
          
       Serial.println(init_data);

        tx_data = "";
    }

   }
   
    // disconnecting
   if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }

}
