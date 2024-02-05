#include "Temp_sensor.h"
#include "Arduino.h"




/* TODO */
void init_sensor_pins(){

    pinMode(AMBIENT_SENSOR_PIN, INPUT);
    pinMode(TOP_SENSOR_PIN, INPUT);
    pinMode(BOTTOM_SENSOR_PIN, INPUT);
}

/* TODO */
int get_sensor_average(byte sensorPin)
{
  // getting average from the analog pin and returning the value of divided from the sample rate
  int sensor_value = 0;
  for (int i = 0; i < 30; i++)
  {
    sensor_value += analogRead(sensorPin);
  }
  return (int)(sensor_value / 30);
}

/* TODO */
float convert_to_temperature(int analogValue)
{
  // transfering the analog value into temperature
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
