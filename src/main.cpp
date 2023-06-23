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

#include <ArduinoBleOTA.h>
#include <NimBLEDevice.h>

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#define AMBIENT_SENSOR_PIN 34
#define TOP_SENSOR_PIN 39
#define BOTTOM_SENSOR_PIN 36

// #define RED_LED 19
#define GREEN_LED 23
#define BLUE_LED 18

#define PWM_PIN 27
#define SHUNT_RESIST_CC_PIN 32
#define SHUNT_RESIST_SYM_PIN 15
#define BATTERY_VOLTAGE_PIN 33

#define TFT_RST 26         // IO 26
#define TFT_RS 2           // IO 25
#define TFT_CLK 14         // HSPI-SCK
#define TFT_SDI 13         // HSPI-MOSI
#define TFT_CS 19          // HSPI-SS0
#define TFT_LED 0          // 0 if wired to +5V directly
#define TFT_BRIGHTNESS 200 // Initial brightness of TFT backlight (optional)

#define SCK 14
#define MISO 12
#define MOSI 13
#define CS 5
#define TEST
#define LIMIT_SWITCH 35

// #define FINAL
bool rtc_status;
bool sd_status;
SPIClass spi = SPIClass(HSPI);

int buzzerPin = 4;
const int freq = 500;
const int ledChannel = 0;
const int resolution = 8;

unsigned long previousMillis = 0;
unsigned long previousMillis1 = 0;


int batt_range_bar;
int x;
int y;

int batt_range;
int battery_value;
uint32_t Feedback_Value = 0;
int i_expected;
int i_actual;
int dutycycle = 81;

File myFile;

RTC_DS1307 RTC;
SPIClass hspi(HSPI);
TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, TFT_BRIGHTNESS);
char file_name[19];

OneButton button(LIMIT_SWITCH, true);
static char chamber_temperature_str[6];
static char current_shunt_cc_str[5];
static char current_shunt_sym_str[5];
static char batt_voltage_str[5];
static char batt_bar_str[5];
static char ambient_temperature_str[6];
static char top_temperature_str[6];
static char bottom_temperature_str[6];
static char month_str[4];
static char day_str[4];
static char year_str[6];
static char hour_str[4];
static char minute_str[4];
static char second_str[4];
static char initial_command[50];
static char temp[3] = "00";
bool ota_flag = false;

float ambient_temperature, top_temperature, bottom_temperature, chamber_temperature;
int month, day, year, hour, minute, second;
int battery_range;
float batt_voltage;

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer *pServer)
  {

    deviceConnected = false;
  }
};

bool send_data_flag = false;
String init_data = "";
String st_init_data = "";
std::string tx_data = "";

bool BLE_device_command(String inCmd)
{

  if (inCmd.indexOf("HS") >= 0)
  {
    init_data = "h\0";
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

void OTA_UPDATE()
{
  ArduinoBleOTA.begin("ArduinoBleOTA", InternalStorage);

  ota_flag = true;

  while (ota_flag == 1)
  {
#if defined(BLE_PULL_REQUIRED)
    BLE.poll();
#endif
    ArduinoBleOTA.pull();
  }
}

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
  for (int i = 0; i < 30; i++)
  {
    sensor_value += analogRead(sensorPin);
  }
  return (int)(sensor_value / 30);
}

float convert_to_current(int analogvalue_1)
{
  float voltage_shunt_cc = analogvalue_1 * (3.3 / 4095.00);
  return (voltage_shunt_cc / 21);
}

void get_batt_value()
{

  battery_value=0;
  for (int i = 0; i < 5; i++)
      battery_value = battery_value + analogRead(BATTERY_VOLTAGE_PIN);
  battery_value = battery_value / 5;
  batt_voltage = battery_value * (3.3 / 4095.00);
  Serial.println(batt_voltage);
  batt_voltage =  batt_voltage/0.1886792452830;
  Serial.println(batt_voltage);
  dtostrf(batt_voltage, 3, 1, batt_voltage_str);
  Serial.println(batt_voltage_str);
  battery_range=map(battery_value, 3040,4095,0,100);
  
}

int convert_range_to_bar()
{

  if ((battery_range > 75) && (battery_range <= 100))
    return 4;
  else if ((battery_range > 50) && (battery_range <= 75))
    return 3;
  else if ((battery_range > 25) && (battery_range <= 50))
    return 2;
  else if ((battery_range > 7) && (battery_range <= 25))
    return 1;
  else
    return 0;
}

void display_battery_bars(int bars)
{
int x = 191;
int y = 5;
tft.drawRectangle(x, y, x + 22, y + 10, COLOR_WHITE);
tft.fillRectangle(x + 22, y + 3, x + 24, y + 6, COLOR_WHITE);
  if (bars >= 1)
  tft.fillRectangle(x + 2, y + 2, x + 5, y + 8, COLOR_WHITE);
  if (bars >= 2){
  tft.fillRectangle(x + 7, y + 2, x + 10, y + 8, COLOR_WHITE);
  }
  if (bars >= 3){
  tft.fillRectangle(x + 12, y + 2, x + 15, y + 8, COLOR_WHITE);
   }
  if (bars >= 4){
    tft.fillRectangle(x + 17, y + 2, x + 20, y + 8,COLOR_WHITE );
  }
  if(bars == 0){
    //tft.fillRectangle(0, 0, 240, 200, COLOR_BLACK);
    //tft.setFont(Terminal22x32);
    tft.drawText(20, 50, "Low battery", COLOR_WHITE);
  }

}

void sd_card_logo(){
  x = 20;
  y = 135;
  tft.drawLine(x + 3, y + 6 , x+24, y + 42, COLOR_WHITE);
  tft.drawLine(x + 7, y + 6 , x+27, y + 6, COLOR_WHITE);
  tft.drawLine(x + 27, y + 6 , x+27 ,y + 35, COLOR_WHITE);
  tft.drawLine(x + 27, y + 35 , x+1, y + 35, COLOR_WHITE);
  tft.drawLine(x + 1, y + 35 , x+1, y + 20, COLOR_WHITE);
  tft.drawLine(x + 1, y + 20 , x+7, y + 15, COLOR_WHITE);
  tft.drawLine(x + 7, y + 15 , x+7, y + 6, COLOR_WHITE);
  tft.fillRectangle(x+10, y+10, x+12, y+15, COLOR_WHITE);
  tft.fillRectangle(x+14, y+10, x+16, y+15, COLOR_WHITE);
  tft.fillRectangle(x+18, y+10, x+20, y+15, COLOR_WHITE);
  tft.fillRectangle(x+22, y+10, x+24, y+15, COLOR_WHITE);

}

void get_rtc()
{
  // getting month,day,year,hour,minute,sec using functions
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

void blu_logooff()
{
  x = 175;
  y = 3;
  tft.fillRectangle(x - 18, y - 2, x +7, y + 13, COLOR_BLACK);
}

void time()
{
  tft.drawText(0, 4, day_str);
  tft.drawText(15, 4, "/");
  tft.drawText(25, 4, month_str);
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
  dtostrf(chamber_temperature, 5, 1, chamber_temperature_str);
  sprintf(month_str, "%02d", month);
  sprintf(day_str, "%02d", day);
  sprintf(year_str, "%04d", year);
  sprintf(hour_str, "%02d", hour);
  sprintf(minute_str, "%02d", minute);
  sprintf(second_str, "%02d", second);
  Serial.println(chamber_temperature_str);
  tft.setOrientation(3);
  tft.setFont(Terminal22x32);
  tft.fillRectangle(15, 90, 130, 130, COLOR_BLACK);
  tft.drawText(25, 87, chamber_temperature_str, COLOR_WHITE);
  tft.drawCircle(132, 82, 3, COLOR_WHITE);
  tft.drawText(135, 87, "C", COLOR_WHITE);
  tft.setFont(Terminal12x16);
  tft.fillRectangle(130, 44, 160, 64, COLOR_BLACK);
  tft.drawText(137, 35, ambient_temperature_str, COLOR_WHITE);
  tft.drawCircle(197, 35, 2, COLOR_WHITE);
  tft.drawText(200, 35, "C");
  tft.drawText(150, 55, "AMB");

  tft.setFont(Terminal6x8);

  if (rtc_status == true)
  {
    time();
  }
  else
  {
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

  if (deviceConnected)
  {
    blu_logoon();
  }
  else
  {
    blu_logooff();
  }
}

void display_ota_mode()
{
  tft.fillRectangle(0, 0, 240, 200, COLOR_BLACK);
  tft.setFont(Terminal22x32);
  tft.drawText(5, 87, "OTA MODE ON", COLOR_WHITE);
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

void appendFile(fs::FS &fs, const char *path, const char *message)
{

  Serial.printf("Appending to file: %s\n", path);
  tft.setOrientation(3);
  tft.setFont(Terminal6x8);
  int a = 40;
  int b = 150;
  File file = fs.open(path, FILE_APPEND);
  if (!file)
  {
    Serial.println("Failed to open file for appending");

    sd_card_logo();
    return;
  }
  if (file.print(message))
  {
    Serial.println("Message appended");
      x = 20;
      y = 135;
    tft.fillRectangle(x+0,y+5,x+30,y+40,COLOR_BLACK);
    sd_status = 1;
  }
  else
  {
    Serial.println("Append failed");
    sd_card_logo();
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
      appendFile(SD, file_name, hour_str);
      appendFile(SD, file_name, " :");
      appendFile(SD, file_name, minute_str);
      appendFile(SD, file_name, " :");
      appendFile(SD, file_name, second_str);
      appendFile(SD, file_name, "->  ");
      appendFile(SD, file_name, chamber_temperature_str);
      appendFile(SD, file_name, ", Lid is Open ");
      appendFile(SD, file_name, "\n");
      open_1--;
    }

    digitalWrite(buzzerPin, LOW);
  }
}

void stopBuzzer()
{
  buzzerStatus = false;
  open_1 = 1;
  while (close1 == 1)
  {
    Serial.println(" started storing data of Lid close");
    appendFile(SD, file_name, "Lid close ");
    appendFile(SD, file_name, hour_str);
    appendFile(SD, file_name, " :");
    appendFile(SD, file_name, minute_str);
    appendFile(SD, file_name, " :");
    appendFile(SD, file_name, second_str);
    appendFile(SD, file_name, "->  ");
    appendFile(SD, file_name, chamber_temperature_str);
    appendFile(SD, file_name, ", Lid is Close");
    appendFile(SD, file_name, "\n");
    close1--;
  }
  digitalWrite(buzzerPin, HIGH);
}

void sd_card_init()
{
  sd_status = SD.begin(CS, spi, 10000000);
  Serial.println(sd_status);
  tft.setOrientation(3);
  tft.setFont(Terminal6x8);
  int a = 40;
  int b = 150;
  if (!sd_status)
  {
    Serial.println("Card Mount Failed");
    sd_card_logo();
  }
  else
  {
    x = 20;
    y = 135;
    tft.fillRectangle(x+0,y+5,x+30,y+40,COLOR_BLACK);
  }

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    sd_card_logo();
  }
}

void getFileName()
{

  sprintf(file_name, "/%02d%02d%02d%02d%02d%02d.csv", day, month, year, hour, minute, second);

  Serial.println("Creating test.txt. . .");
  myFile = SD.open(file_name, FILE_WRITE); //"file_name"is the file name to be created and FILE_WRITE is a command to create file.
  myFile.close();                          // Closing the file
  if (SD.exists(file_name))
  { 
    // If the file test.txt exist.
    Serial.println("File Created and SD Card initiated");
    tft.setFont(Terminal12x16);
    tft.drawText(1, 93, "logging started", COLOR_WHITE);
    delay(500);
    tft.clear();
  }

  else
  { 
    // And if not
    Serial.println("File not Created and SD Card not initialized");
  }
  Serial.println(file_name);
}

void setup()
{
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, HIGH);
  Serial.begin(115200);
  hspi.begin();
  tft.begin(hspi);
  spi.begin(SCK, MISO, MOSI, CS);
  sd_card_init();

  Wire.begin(21, 22);
  rtc_status = RTC.begin();

  //RTC.adjust(DateTime(23,06,15,12,26,00));

  get_rtc();
  getFileName();

  init_BLE();

  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(PWM_PIN, ledChannel);
  digitalWrite(PWM_PIN, LOW);

  pinMode(AMBIENT_SENSOR_PIN, INPUT);
  pinMode(TOP_SENSOR_PIN, INPUT);
  pinMode(BOTTOM_SENSOR_PIN, INPUT);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  button.setPressTicks(100);
  button.attachDuringLongPress(runBuzzer);
  button.attachLongPressStop(stopBuzzer);

  writeFile(SD, file_name, "Date,");
  appendFile(SD, file_name, "Time,");
  appendFile(SD, file_name, "chamber_temp,");
  appendFile(SD, file_name, "top_temp,");
  appendFile(SD, file_name, "bot_temp,");
  appendFile(SD, file_name, "ambient_temp,");
  appendFile(SD, file_name, "\n");
}

void loop()
{
  get_rtc();
  button.tick();

  int sensor_value_3 = 0;

  for (int i = 0; i <= 100; i++)
  {
    sensor_value_3 += analogRead(SHUNT_RESIST_CC_PIN);
    delayMicroseconds(200);
  }

  // sensor_value_3 = sensor_value_3 / 100;
  float voltage_shunt_cc = convert_to_current(sensor_value_3);
  float current_shunt_cc = voltage_shunt_cc / 0.03;
  Serial.print("current_shunt_cc");
  Serial.println(current_shunt_cc);

  int sensor_value_1 = analogRead(SHUNT_RESIST_SYM_PIN);
  float voltage_shunt_sym = sensor_value_1 * (3.3 / 4095.00);
  voltage_shunt_sym = voltage_shunt_sym / 21;
  float current_shunt_sym = voltage_shunt_sym / 0.025;
  Serial.print("current_shunt_sym");
  Serial.println(current_shunt_sym);

  get_batt_value();
  batt_range_bar=convert_range_to_bar();
  Serial.println(batt_range_bar);


  // dtostrf(current_shunt_cc, 5, 1, current_shunt_cc_str);
  dtostrf(current_shunt_sym, 5, 1, current_shunt_sym_str);
  sprintf(batt_bar_str, "%02d", batt_range_bar);
   //dtostrf(batt_range_bar,5,0,batt_bar_str);

  // storing the chamber temperature value every minute in SD card
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis1 >= 30000)
  {
    if (rtc_status == false)
    {
      appendFile(SD, file_name, temp);
      appendFile(SD, file_name, " :");
      appendFile(SD, file_name, temp);
      appendFile(SD, file_name, " :");
      appendFile(SD, file_name, temp);
    }

    appendFile(SD, file_name, month_str);
    appendFile(SD, file_name, "/");
    appendFile(SD, file_name, day_str);
    appendFile(SD, file_name, "/");
    appendFile(SD, file_name, year_str);
    appendFile(SD, file_name, "   ,");
    appendFile(SD, file_name, hour_str);
    appendFile(SD, file_name, ":");
    appendFile(SD, file_name, minute_str);
    appendFile(SD, file_name, ":");
    appendFile(SD, file_name, second_str);
    appendFile(SD, file_name, "  ,");
    appendFile(SD, file_name, chamber_temperature_str);
    appendFile(SD, file_name, "  ,");
    appendFile(SD, file_name, top_temperature_str);
    appendFile(SD, file_name, "  ,");
    appendFile(SD, file_name, bottom_temperature_str);
    appendFile(SD, file_name, "  ,");
    appendFile(SD, file_name, ambient_temperature_str);
    appendFile(SD, file_name, "  ,");
    appendFile(SD, file_name, current_shunt_cc_str);
    appendFile(SD, file_name, "  ,");
    appendFile(SD, file_name, current_shunt_sym_str);
    appendFile(SD, file_name, ",");
    appendFile(SD, file_name, batt_voltage_str);
    appendFile(SD, file_name, "\n");

    previousMillis1 = currentMillis;
  }

// getting Top,Bottom,Ambient temp value every one sec

  if ((currentMillis - previousMillis) >= 3000)
  {
    int analog_value = get_sensor_average(AMBIENT_SENSOR_PIN);
    ambient_temperature = convert_to_temperature(analog_value);
    Serial.print("ambient analog_value");
    Serial.println(analog_value);
    int x = 191;
    int y = 5;
    tft.fillRectangle(x+1,y+1,x + 20, y + 9, COLOR_BLACK);// to clear sd card 
    analog_value = get_sensor_average(TOP_SENSOR_PIN);
    top_temperature = convert_to_temperature(analog_value);
    Serial.print("Top analog_value: ");
    Serial.println(analog_value);

    analog_value = get_sensor_average(BOTTOM_SENSOR_PIN);
    bottom_temperature = convert_to_temperature(analog_value);
    Serial.print("Bottom analog_value: ");
    Serial.println(analog_value);
    chamber_temperature = (top_temperature + bottom_temperature) / 2;

    previousMillis = currentMillis;
    display();
    
    display_battery_bars(batt_range_bar);


#ifdef TEST
    if (chamber_temperature <= 2){
      ledcWrite(ledChannel, 150); //
       digitalWrite(GREEN_LED, LOW); // turn the LED on (HIGH is the voltage level)                    // wait for a second
      digitalWrite(BLUE_LED, HIGH);
    }
    else if (chamber_temperature >= 2 && chamber_temperature <= 4)
    {
      ledcWrite(ledChannel, 150); // in this pwm write the current will maintain in 1.2 amps
      digitalWrite(GREEN_LED, LOW); // turn the LED on (HIGH is the voltage level)                    // wait for a second
      digitalWrite(BLUE_LED, HIGH);
    }
    else if (chamber_temperature > 4)
    {
      ledcWrite(ledChannel, 150);
      digitalWrite(GREEN_LED, HIGH); // turn the LED on (HIGH is the voltage level                    // wait for a second
      digitalWrite(BLUE_LED, LOW);
    }

#endif
    
  }

  // sending cham_tem,atm_temp,Bat_level,Volt,current to the bluetooth app
  if (deviceConnected)
  {
    init_data.remove(0);
    init_data += day_str;
    init_data += "/";
    init_data += month_str;
    init_data += "/";
    init_data += year_str;
    init_data += " ,";
    init_data += hour_str;
    init_data += ":";
    init_data += minute_str;
    init_data += ":";
    init_data += second_str;
    init_data += " ,";
    init_data += chamber_temperature;
    init_data += " ,";
    init_data += top_temperature_str;
    init_data += " ,";
    init_data += bottom_temperature_str;
    init_data += " ,";
    init_data += ambient_temperature;
    init_data += " ,";
    init_data += batt_voltage_str;
    init_data += " ,";
    init_data += batt_bar_str;
    init_data += " ,";
    init_data += current_shunt_sym_str;
    init_data += " ,";
    for (int i = 0; i < init_data.length(); i++)
      st_init_data += init_data[i];

    pTxCharacteristic->setValue(st_init_data); // Notify fromSerial.
    pTxCharacteristic->notify();

    for (int i = 0; i < init_data.length(); i++)
      st_init_data.remove(i);

    if(send_data_flag)
    {
        if (init_data = "h\0")
        {
          Serial.println("ota mode enter");
          display_ota_mode();
          OTA_UPDATE();
        }

    }

     send_data_flag = false;

    Serial.println(init_data);

    tx_data = "";

    delay(2000);
  }

  // disconnecting
  if (!deviceConnected && oldDeviceConnected)
  {
    delay(500);                  // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected)
  {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
}
