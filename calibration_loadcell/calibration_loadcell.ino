/*
 Example using the SparkFun HX711 breakout board with a scale
 By: Nathan Seidle
 SparkFun Electronics
 Date: November 19th, 2014
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

 This is the calibration sketch. Use it to determine the calibration_factor that the main example uses. It also
 outputs the zero_factor useful for projects that have a permanent mass on the scale in between power cycles.

 Setup your scale and start the sketch WITHOUT a weight on the scale
 Once readings are displayed place the weight on the scale
 Press +/- or a/z to adjust the calibration_factor until the output readings match the known weight
 Use this calibration_factor on the example sketch

 This example assumes pounds (lbs). If you prefer kilograms, change the Serial.print(" lbs"); line to kg. The
 calibration factor will be significantly different but it will be linearly related to lbs (1 lbs = 0.453592 kg).

 Your calibration factor may be very positive or very negative. It all depends on the setup of your scale system
 and the direction the sensors deflect from zero state
 This example code uses bogde's excellent library: https://github.com/bogde/HX711
 bogde's library is released under a GNU GENERAL PUBLIC LICENSE
 Arduino pin 2 -> HX711 CLK
 3 -> DOUT
 5V -> VCC
 GND -> GND

 Most any pin on the Arduino Uno will be compatible with DOUT/CLK.

 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.

*/

#include "HX711.h"

#define LOADCELL1_CLK 2
#define LOADCELL1_DIO 3

#define LOADCELL2_CLK 6
#define LOADCELL2_DIO 7

HX711 scale1;
HX711 scale2;

float calibration_factor1 = -20000; //-7050 worked for my 440lb max scale setup
float calibration_factor2 = -20000; // -20000 and -15000 perfect combo so far

int spike_count = 0;
float total_weight = 0;

float start_time = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");

  scale1.begin(LOADCELL1_DIO, LOADCELL1_CLK);
  scale1.set_scale();
  scale1.tare(); //Reset the scale to 0

  long zero_factor1 = scale1.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor1);

  scale2.begin(LOADCELL2_DIO, LOADCELL2_CLK);
  scale2.set_scale();
  scale2.tare(); //Reset the scale to 0

  long zero_factor2 = scale2.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor2);
}

void loop() {

  scale1.set_scale(calibration_factor1); //Adjust to this calibration factor
  scale2.set_scale(calibration_factor2); //Adjust to this calibration factor
  
//  Serial.print("Reading: ");
//  Serial.print(scale1.get_units(), 3);
//  Serial.print(" kg"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
//  Serial.print(" Reading: ");
//  Serial.print(scale2.get_units(), 3);
//  Serial.println(" kg");

//  scale1.set_gain(128);
//  Serial.print("LC combo 1 Reading: ");
//  Serial.print(scale1.get_units(), 3);
//  Serial.print(" kg"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
////  scale1.set_gain(32);
//  Serial.print("  LC combo 2 Reading: ");
//  Serial.print(scale2.get_units(), 3);
//  Serial.print(" kg");
  start_time = millis();
  total_weight = (scale1.get_units() + scale2.get_units());
  Serial.print("time elapsed: ");
  Serial.print(millis() - start_time);
  Serial.print(" Total weight: ");
  Serial.println(total_weight , 3);
//  if(abs(total_weight) > 10){
//    spike_count++;
//    Serial.print(" Total weight: ");
//    Serial.print(total_weight , 3);
//    Serial.print(" Spike_count: ");
//    Serial.println(spike_count);
//    if(spike_count % 5 == 0){
//      Serial.println("----------------");  
//    }  
//  }
  

//  Serial.println(scale1.get_value() + scale2.get_value(), 3);
//  delay()

  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+')
      calibration_factor1 += 10;
    else if(temp == '-')
      calibration_factor1 -= 10;
    else if(temp == 'a')
      calibration_factor2 += 10;
    else if(temp == 'z')
      calibration_factor2 -= 10;
  }
}
