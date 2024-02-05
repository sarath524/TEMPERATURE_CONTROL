#include "Arduino.h"

#define AMBIENT_SENSOR_PIN 34
#define TOP_SENSOR_PIN 39
#define BOTTOM_SENSOR_PIN 36


void init_sensor_pins();

int get_sensor_average(byte sensorPin);

float convert_to_temperature(int analogValue);