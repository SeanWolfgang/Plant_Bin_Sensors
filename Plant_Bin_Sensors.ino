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

// Include the liquid crystal library
#include <LiquidCrystal.h>

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

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
//const int rs = 6, en = 7, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Sensor data struct to store all readings in a central place
struct sensorData {
  String timestamp;
  float HTS221_temp;
  float HTS221_humid;
  float soil_temp;
  float soil_humid;
  float BH1750_light;
  float VPD;
};

typedef struct sensorData SensorData;
SensorData plantSensorData;

// Define loop delay in milliseconds
int loopDelay = 5000;

void setup() {
  // Start serial monitor at 9600
  Serial.begin(9600);

  // set up the LCD's number of columns and rows:
  //lcd.begin(16, 2);
  //lcd.print("Initializing...");
  
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
      //lcd.clear();
      //lcd.print("Init failed.");
    }
  }

  // If initialization succeeded, say so on LCD
  //lcd.clear();
  //lcd.print("Init OK.");

  delay(1000);
}

void loop() {
  // Get light reading in Lux from BH1750 sensor
  //BH1750.start();
  //float lux = BH1750.getLux();
  
  //lcd.clear();
  //lcd.print("Lux: " + String(lux));

  // (note: line 1 is the second row, since counting begins with 0):
  //lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  //lcd.print(upTime());
  
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
  // Log sensor data for each data entry
  populateSensorData();
  
  // Print datetime
  Serial.println(plantSensorData.timestamp);
  
  // Print sensor readings
  Serial.print("HTS221 temp: "); Serial.println(plantSensorData.HTS221_temp);
  Serial.print("HTS221 hum: "); Serial.println(plantSensorData.HTS221_humid);
  Serial.print("SS temp: "); Serial.println(plantSensorData.soil_temp);
  Serial.print("SS moist: "); Serial.println(plantSensorData.soil_humid);
  Serial.print("BH1750 Lux: "); Serial.println(plantSensorData.BH1750_light);
  Serial.print("VPD: "); Serial.println(plantSensorData.VPD);
  Serial.print("\n");
}

void initializeSD(int pinSD) {
  // Serial.print("Initializing SD card...");

  if (!SD.begin(pinSD)) {
    // Serial.println("initialization failed!");
    //lcd.clear();
    //lcd.print("Wait SD...");
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
  logFile.print("Timestamp");   logFile.print("\t");
  logFile.print("HTS221_Temp"); logFile.print("\t");
  logFile.print("HTS221_Hum");  logFile.print("\t");
  logFile.print("SS_Temp");     logFile.print("\t");
  logFile.print("SS_Moist");    logFile.print("\t");
  logFile.print("BH1750_Lux");  logFile.print("\t");
  logFile.print("VPD");         logFile.print("\n");
  logFile.close();
}

void logSensorData() {
  // Log sensor data for each data entry
  populateSensorData();
  
  // Using local file variable to log to
  File logFile;

  // Open log file with formatted file name assigned in assignFileName
  logFile = SD.open(SDFileName, FILE_WRITE);

  // Print datetime
  logFile.print(plantSensorData.timestamp);     logFile.print("\t");
  
  // Print sensor readings
  logFile.print(plantSensorData.HTS221_temp);   logFile.print("\t");
  logFile.print(plantSensorData.HTS221_humid);  logFile.print("\t");
  logFile.print(plantSensorData.soil_temp);     logFile.print("\t");
  logFile.print(plantSensorData.soil_humid);    logFile.print("\t");
  logFile.print(plantSensorData.BH1750_light);  logFile.print("\t");
  logFile.print(plantSensorData.VPD);           logFile.print("\n");
  
  logFile.close();
}

void populateSensorData() {
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

  String dateTimeStamp;
  
  dateTimeStamp.concat(rtc.getDateStr(FORMAT_LONG,FORMAT_MIDDLEENDIAN,'/'));
  dateTimeStamp.concat(" ");
  dateTimeStamp.concat(rtc.getTimeStr());

  plantSensorData.timestamp = dateTimeStamp;
  plantSensorData.HTS221_temp = HTS_temp.temperature;
  plantSensorData.HTS221_humid = HTS_humidity.relative_humidity;
  plantSensorData.soil_temp = ssTempC;
  plantSensorData.soil_humid = ssMoist;
  plantSensorData.BH1750_light = lux;
  plantSensorData.VPD = calcVPD(HTS_temp.temperature, HTS_humidity.relative_humidity);
}

// Return vapor pressure deficit to air
float calcVPD(float temp, float hum) {
  // Calculation from: https://betterorganix.com/blog/what-is-how-to-calculate-vapour-pressure-deficit/#:~:text=To%20Get%20VPD%2C%20we%20need,And%20VOILA%2C%20you%20have%20VPD.
  return ((610.7 * pow(10, (7.5 * temp) / (237.3 + temp))) / 1000) * (hum / 100);
}

String upTime() {
  unsigned long upMillis = millis();

  char outChars[22];
  
  int hours = upMillis / 3600000;
  int minutes = (upMillis / 60000) % 60;
  int seconds = (upMillis / 1000) % 60;

  sprintf(outChars, "%d:%02d:%02d", hours, minutes, seconds);

  return (String(outChars));
}
