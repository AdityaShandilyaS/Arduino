#include <Arduino.h>
#include <TM1637Display.h>
#include <HX711.h>

#define LOADCELL1_1_CLK 2
#define LOADCELL1_1_DIO 3

#define LOADCELL1_2_CLK 4
#define LOADCELL1_2_DIO 5

#define LOADCELL2_1_CLK 6
#define LOADCELL2_1_DIO 7

#define LOADCELL2_2_CLK 8
#define LOADCELL2_2_DIO 9

int i = 0;
float calibration_factor = -20000;
float step1 = 0, step2 = 0, step3 = 0;
long start_time = 0, elapsed_time = 0;

HX711 scale1_1;
HX711 scale1_2;
HX711 scale2_1;
HX711 scale2_2;

void setup() {
  Serial.begin(115200);
  calibrate_step2();
  calibrate_step3();
  // put your setup code here, to run once:

}

void loop() {
  step1 = analogRead(A0);
  start_time = millis();
  step2 = scale1_1.get_units() + scale1_2.get_units();
  delay(0.5);
  step3 = scale2_1.get_units() + scale2_2.get_units();
  delay(0.5);
  Serial.print("1:");
  Serial.print(step1);
  Serial.print(" 2:");
  Serial.print(step2);
  Serial.print(" 3:");
  Serial.println(step3);
  elapsed_time = millis() - start_time;
  delay(100 - elapsed_time);
  // put your main code here, to run repeatedly:
}

int calibrate_step2(){
  scale1_1.begin(LOADCELL1_1_DIO, LOADCELL1_1_CLK);
  scale1_1.set_scale();
  scale1_1.tare(); //Reset the scale to 0

  long zero_factor1_1 = scale1_1.read_average();
  scale1_1.set_scale(calibration_factor);

  scale1_2.begin(LOADCELL1_2_DIO, LOADCELL1_2_CLK);
  scale1_2.set_scale();
  scale1_2.tare(); //Reset the scale to 0

  long zero_factor1_2 = scale1_2.read_average();
  scale1_2.set_scale(calibration_factor);
}

int calibrate_step3(){
  scale2_1.begin(LOADCELL2_1_DIO, LOADCELL2_1_CLK);
  scale2_1.set_scale();
  scale2_1.tare(); //Reset the scale to 0

  long zero_factor2_1 = scale2_1.read_average();
  scale2_1.set_scale(calibration_factor);

 scale2_2.begin(LOADCELL2_2_DIO, LOADCELL2_2_CLK);
  scale2_2.set_scale();
  scale2_2.tare(); //Reset the scale to 0

  long zero_factor2_2 = scale2_2.read_average();
  scale2_2.set_scale(calibration_factor); 
}
