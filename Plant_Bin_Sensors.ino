// HTS221 Temperature and Humidity Libraries
#include <Wire.h>
#include <Adafruit_HTS221.h>
#include <Adafruit_Sensor.h>

// Soil Sensor Library
#include "Adafruit_seesaw.h"

// BH1750 Light Libraries
#include <Arduino.h>
#include <hp_BH1750.h>  //  include the library

// Declaring class variables for sensors
Adafruit_HTS221 hts;    // Temperature humidity
Adafruit_seesaw ss;     // Soil
hp_BH1750 BH1750;       // Light

void setup() {
  // Start serial monitor at 9600
  Serial.begin(9600);

  // Declar built in LED as output
  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize temperature/humidity, soil, and light sensors
  bool HTS221_avail = hts.begin_I2C();
  bool soil_avail   = ss.begin(0x36);
  bool BH1750_avail = BH1750.begin(BH1750_TO_GROUND);

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
  
  
}

String booleanOutput(bool booleanInput) {
  if (booleanInput) {
    return "True";
  } else {
    return "False";
  }
}

void blinkWithDelay(int millisecondDelay) {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(millisecondDelay);           // wait for a delay
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(millisecondDelay);           // wait for a delay
}
