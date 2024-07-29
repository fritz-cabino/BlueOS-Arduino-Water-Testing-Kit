//Currently configured for Json string output to BlueOS, where the data is timestamped and .csv logged in Node-red. 
//Use of the on-board RTC and SD card logging are commented out to save program space.

//#include "RTClib.h"
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "TSYS01.h"
#include "MS5837.h"
#include "Arduino.h"
#include <ArduinoJson.h>

//RTC_PCF8523 rtc;
Sd2Card card;
SdVolume volume;
SdFile root;
MS5837 msensor;
TSYS01 tsensor;

const int chipSelect = 10;
int ledPin = 13;
int leakPin = 2;
int leak = 0;

void setup() {

  Serial.begin(9600);
  while (!Serial) {};  // wait for serial port to connect. Needed for native USB port only

  pinMode(ledPin, OUTPUT);
  pinMode(leakPin, INPUT);

  Wire.begin();
  delay (2000);

  while (!tsensor.init()) {
        Serial.println("TSYS01 device failed to initialize!");
        delay(2000);
    };

  while (!msensor.init()) {
        Serial.println("MS5837 device failed to initialize!");
        delay(2000);
  };

  msensor.setModel(MS5837::MS5837_30BA);
  msensor.setFluidDensity(1029);  // kg/m^3 (997 for freshwater, 1029 for seawater)

//Serial.print("Initializing SD card...");
//if (!SD.begin(chipSelect)) {
  //Serial.println("Card failed, or not present");
  //while (1);
//}
//Serial.println("card initialized.");

//if (!rtc.begin()) {
//  Serial.println("Couldn't find RTC.");
//  Serial.flush();
//  while (1) delay(10);
//}
//rtc.start();
}

void loop() {

//File dataFile = SD.open("datalog.txt", FILE_WRITE);
//String dataString = ""; 

msensor.read();
tsensor.read();

int ORPanalog = analogRead(A0);
  float ORP = ((ORPanalog * (5.0/1023.0)-1.500)*1000) -27.3 ; //voltage conversion, Atlas Datasheet, calibration
int pHsensorValue = analogRead(A1); 
  float pHvoltage = pHsensorValue * (5.0 / 1023.0);
  float pHsensor = (pHvoltage * -5.6548) + 15.509; //calibration values
  float calpH = (pHsensor * 1.005) + 0.1693; //calibration slope

  delay(1000);

//DateTime now = rtc.now();

  digitalWrite(ledPin, leak);

//dataString += String(now.unixtime()) + ",";
//dataString += String(msensor.depth()) + ",";
//dataString += String(tsensor.temperature()) + ",";
//dataString += String(ORP) + ",";
//dataString += String(calpH) + ",";
//dataString += String(pHsensor) + ",";
//dataString += String(digitalRead(leakPin));

//if (dataFile) {
//  dataFile.println(dataString);
//  dataFile.close();
//  Serial.println();
//  Serial.println(dataString);
//  Serial.println();
//}

//else {
//  Serial.println("error opening datalog.txt");
//}
  
  JsonDocument doc;
//doc["unixtime"] = now.unixtime();
  doc["depth"] = msensor.depth();
  doc["temp"] = tsensor.temperature();
//doc["doxy"] = dox.get_last_received_reading();
  doc["oxrp"] = ORP;
  doc["ph"] = calpH;
  doc["phs"] = pHsensor;
//doc["ecn"] = ec.get_last_received_reading();
  doc["leak"] = digitalRead(leakPin);

  serializeJson(doc, Serial);
  Serial.println();

}
