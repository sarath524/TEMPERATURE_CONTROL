
#define AMBIENT_SENSOR_PIN 34
#define TOP_SENSOR_PIN 39
#define BOTTOM_SENSOR_PIN 36

unsigned long previousMillis = 0;

float ambient_temperature, top_temperature, bottom_temperature, chamber_temperature;

int get_sensor_average(byte sensorPin)
{
  int sensor_value = 0;
  for (int i = 0; i < 50; i++)
  {
    sensor_value += analogRead(sensorPin);
  }
  return (int)(sensor_value / 50);
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

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
}



void loop() {

  // getting Top,Bottom,Ambient temp value every one sec
  unsigned long currentMillis = millis();

  if ((currentMillis - previousMillis) >= 1000)
  {
    int analog_value = get_sensor_average(AMBIENT_SENSOR_PIN);
    ambient_temperature = convert_to_temperature(analog_value);
    Serial.print("ambient_temperature:  ");
    Serial.println(ambient_temperature);
    analog_value = get_sensor_average(TOP_SENSOR_PIN);
    top_temperature = convert_to_temperature(analog_value);
     Serial.print("top_temperature:  ");
     Serial.println(top_temperature);
    analog_value = get_sensor_average(BOTTOM_SENSOR_PIN);
    bottom_temperature = convert_to_temperature(analog_value);
    Serial.print("bottom_temperature:  ");
     Serial.println(bottom_temperature);
    chamber_temperature = (top_temperature + bottom_temperature) / 2;
       Serial.print("chamber_temperature:  ");
     Serial.println(chamber_temperature);
    previousMillis = currentMillis;
  }
}
