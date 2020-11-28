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

// Declaring class variables for sensors
Adafruit_HTS221 hts;    // Temperature humidity
Adafruit_seesaw ss;     // Soil
hp_BH1750 BH1750;       // Light

// Initialize the DS3231 using the hardware interface
DS3231  rtc(SDA,SCL);


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

  delay(500);
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
