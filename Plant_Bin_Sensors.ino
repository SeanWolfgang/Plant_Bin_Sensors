// HTS221 Temperature and Humidity Libraries
#include <Wire.h>
#include <Adafruit_HTS221.h>
#include <Adafruit_Sensor.h>

// Soil Sensor Library
#include "Adafruit_seesaw.h"

// BH1750 Light Libraries
#include <Arduino.h>
#include <hp_BH1750.h>  //  include the library

// RTC library
#include <DS3231.h>

// SD Library
#include "SdFat.h"

// Declaring class variables for sensors
Adafruit_HTS221 hts;    // Temperature humidity
Adafruit_seesaw ss;     // Soil
hp_BH1750 BH1750;       // Light

// Initialize the DS3231 using the hardware interface
DS3231  rtc(SDA,SCL);

// Initialize SD module variable
#define SD_PIN 10
String SDFileName;
SdFat SD;

// Define loop delay in milliseconds
int loopDelay = 5000;

void setup() {
  // Start serial monitor at 9600
  Serial.begin(9600);

  // Declar built in LED as output
  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize temperature/humidity, soil, and light sensors
  bool HTS221_avail = hts.begin_I2C();
  bool soil_avail   = ss.begin(0x36);
  bool BH1750_avail = BH1750.begin(BH1750_TO_GROUND);

  // Initalize the RTC
  rtc.begin();

  // Initialize SD card
  initializeSD(SD_PIN);
  assignFileName();

  logHeader();
  
  // Check if all the sensors initialized
  if (!HTS221_avail || !soil_avail || !BH1750_avail) {

    // Print status of sensor initialization if something failed
    Serial.println("Some sensor was not started correctly.");
    Serial.println("HTS221 = " + booleanOutput(HTS221_avail));
    Serial.println("Soil = " + booleanOutput(soil_avail));
    Serial.println("BH1750 = " + booleanOutput(BH1750_avail));

    // Stick in infinite loop and blink LED to notify failure
    while(1) {
      blinkWithDelay(250);
    }
  }
  
  // Long blink twice to indicate successful initialization
  blinkWithDelay(1000);
  blinkWithDelay(1000);
}

void loop() {
  logSensorData();
  delay(loopDelay);
}

// Takes a boolean input and returns the string of what it is
String booleanOutput(bool booleanInput) {
  if (booleanInput) {
    return "True";
  } else {
    return "False";
  }
}

// Blink the built in LED defined in setup with the specified delay
void blinkWithDelay(int millisecondDelay) {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(millisecondDelay);           // wait for a delay
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(millisecondDelay);           // wait for a delay
}

void serialPrintSensors() {
  // Get temperature and humidity readings from HTS221 sensor
  sensors_event_t HTS_temp;
  sensors_event_t HTS_humidity;
  hts.getEvent(&HTS_humidity, &HTS_temp);

  // Get soil moisture from soil sensor
  float ssTempC = ss.getTemp();
  uint16_t ssMoist = ss.touchRead(0);
  
  // Get light reading in Lux from BH1750 sensor
  BH1750.start();
  float lux = BH1750.getLux();
  
  // Print datetime
  Serial.print(rtc.getDateStr(FORMAT_LONG,FORMAT_MIDDLEENDIAN,'/'));
  Serial.print(" ");
  Serial.println(rtc.getTimeStr());
  
  // Print sensor readings
  Serial.print("HTS221 temp: "); Serial.println(HTS_temp.temperature);
  Serial.print("HTS221 hum: "); Serial.println(HTS_humidity.relative_humidity);
  Serial.print("SS temp: "); Serial.println(ssTempC);
  Serial.print("SS moist: "); Serial.println(ssMoist);
  Serial.print("BH1750 Lux: "); Serial.println(lux);
}

void initializeSD(int pinSD) {
  // Serial.print("Initializing SD card...");

  if (!SD.begin(pinSD)) {
    // Serial.println("initialization failed!");
    blinkWithDelay(1000);
    delay(5000);
    initializeSD(SD_PIN);
  }
  // Serial.println("initialization done.");
}

void assignFileName() {
  // Print formatted time to character buffer
  char buff[50];
  String dateString = rtc.getDateStr(FORMAT_LONG,FORMAT_MIDDLEENDIAN,'-');
  String timeString = rtc.getTimeStr();
  timeString.replace(':','-');
  String togetherString = dateString + "_" + timeString + ".txt";
  
  togetherString.toCharArray(buff, 50);
  
  // Set global SD file name to the formatted timestamp
  SDFileName = buff;
}

void logHeader() {
  // Using local file variable to log to
  File logFile;

  // Open log file with formatted file name assigned in assignFileName
  logFile = SD.open(SDFileName, FILE_WRITE);

  // Log headers for each data entry
  logFile.print("Timestamp");
  logFile.print("\t");
  logFile.print("HTS221_Temp");
  logFile.print("\t");
  logFile.print("HTS221_Hum");
  logFile.print("\t");
  logFile.print("SS_Temp");
  logFile.print("\t");
  logFile.print("SS_Moist");
  logFile.print("\t");
  logFile.print("BH1750_Lux");
  logFile.print("\t");
  logFile.print("VPD");
  logFile.print("\n");
  logFile.close();
}

void logSensorData() {
  // Using local file variable to log to
  File logFile;

  // Open log file with formatted file name assigned in assignFileName
  logFile = SD.open(SDFileName, FILE_WRITE);

  // Log sensor data for each data entry
  // Get temperature and humidity readings from HTS221 sensor
  sensors_event_t HTS_temp;
  sensors_event_t HTS_humidity;
  hts.getEvent(&HTS_humidity, &HTS_temp);

  // Get soil moisture from soil sensor
  float ssTempC = ss.getTemp();
  uint16_t ssMoist = ss.touchRead(0);
  
  // Get light reading in Lux from BH1750 sensor
  BH1750.start();
  float lux = BH1750.getLux();
  
  // Print datetime
  logFile.print(rtc.getDateStr(FORMAT_LONG,FORMAT_MIDDLEENDIAN,'/'));
  logFile.print(" ");
  logFile.print(rtc.getTimeStr());
  logFile.print("\t");
  
  // Print sensor readings
  logFile.print(HTS_temp.temperature);
  logFile.print("\t");
  logFile.print(HTS_humidity.relative_humidity);
  logFile.print("\t");
  logFile.print(ssTempC);
  logFile.print("\t");
  logFile.print(ssMoist);
  logFile.print("\t");
  logFile.print(lux);
  logFile.print("\t");
  logFile.print(calcVPD(HTS_temp.temperature, HTS_humidity.relative_humidity));
  logFile.print("\n");
  logFile.close();
}

// Return vapor pressure deficit to air
float calcVPD(float temp, float hum) {
  // Calculation from: https://betterorganix.com/blog/what-is-how-to-calculate-vapour-pressure-deficit/#:~:text=To%20Get%20VPD%2C%20we%20need,And%20VOILA%2C%20you%20have%20VPD.
  return ((610.7 * pow(10, (7.5 * temp) / (237.3 + temp))) / 1000) * (hum / 100);
}
