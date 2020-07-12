#include <TM1637Display.h>
int threshold = 500;
int units_per_person = 30;
int p1, p2, p3, p4, p5;
int initial1, initial2, initial3;
long start = 0;
bool high_flag = false;
bool low_flag = true;

TM1637Display display(3, 2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  display.setBrightness(7, true);
  display.clear();
  initial1 = analogRead(A0) + units_per_person;
  initial2 = analogRead(A2) + units_per_person;
  initial3 = analogRead(A4) + units_per_person;
  start = micros();
}

void loop() {
  // put your main code here, to run repeatedly:
  
//  delay(0.100);
//  start = micros();
  p1 = analogRead(A0);
  delay(0.100);
//  check_step(1, p1, &initial1);
  p2 = analogRead(A2);
  delay(0.100);
//  check_step(2, p2, initial2);
  p3 = analogRead(A4);
  delay(0.100);
//  check_step(3, p3, initial3);
  // p4 = analogRead(A4);
  // p5 = analogRead(A5);
  Serial.print("1:");
  Serial.print(p1);
  Serial.print(" 2:");
  Serial.print(p2);
  Serial.print(" 3:");
  Serial.println(p3);
//  Serial.println(p2 - p3);
//  Serial.println(micros() - start);
  delay(100);
}

void check_step(int number, int value, int *initial){
  if(value > *initial && high_flag){
//    Serial.print("on step in (us): ");
    Serial.println(micros() - start, 6);
    start = micros();
//    *initial = value;
    high_flag = false;
    low_flag = true;
//    delay(6000);
    int disp = (number * 1000) + value;
    display.clear();
    display.showNumberDec(disp, false);
  }
  if(value < *initial && low_flag){
//    Serial.print("off the step in (us): ");
    Serial.println(micros() - start);
        start = micros();

//    *initial = value;
    low_flag = false;
    high_flag = true;  
  }
  if(value > (initial- 25)){
    start = micros();
  }
    
}
