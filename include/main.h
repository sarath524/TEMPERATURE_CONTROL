


int buzzerPin = 4;
const int freq = 500;
const int ledChannel = 0;
const int resolution = 8;

unsigned long previousMillis = 0;
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;

int batt_range_bar;
int x;
int y;

int batt_range;
int prev_battery_range;
int battery_value;
uint32_t Feedback_Value = 0;
int i_expected;
int i_actual;
int dutycycle = 81;


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