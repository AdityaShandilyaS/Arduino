#include <TM1637Display.h>
int threshold = 500;
int units_per_person = 25;
int off_step_thresh = 12;
int p1, p2, p3, p4, p5;
int initial1, initial2, initial3;
long start = 0;
bool high_flag = false;
bool low_flag = true;
bool on_step = false;
bool on_step_enabled = false;
bool off_step_enabled = true;
bool off_step = true;
int weight = 0;
int count_threshold = 3;
int on_count = 0, off_count = 0;
int on_step_array[10] = {0,0,0,0,0,0,0,0,0,0};
int off_step_array[10] = {0,0,0,0,0,0,0,0,0,0};
TM1637Display display(3, 2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  display.setBrightness(7, true);
  display.clear();
  initial1 = analogRead(A0); //+ units_per_person;
  initial2 = analogRead(A2); //+ units_per_person;
  initial3 = analogRead(A4); //+ units_per_person;
  start = micros();
}

void loop() {
  // put your main code here, to run repeatedly:
  
//  delay(0.100);
//  p1 = analogRead(A0);
//  weight = p1 - initial1;
//  check_upper_count();
//  check_lower_count();
////  Serial.println(weight);
//  delay(5);
//  check_step(1, p1, &initial1);
  p2 = analogRead(A2);
////  check_step(2, p2, initial2);
  p3 = analogRead(A4);
////  check_step(3, p3, initial3);
//  // p4 = analogRead(A4);
//  // p5 = analogRead(A5);
//  Serial.print("step1: ");
//  Serial.print(p1);
    Serial.print("   step2: ");
    Serial.print(p2 - initial2);
    Serial.print("   step3: ");
    Serial.println(p3 - initial3);
    Serial.println(p3 + p2 -initial2 - initial3);
  delay(100);
}

void check_upper_count(){
  if(weight > units_per_person && on_step_enabled == false){
    on_step = true;
    off_step = false;
    on_count++ ;     
  }
  else{
    on_count = 0;
  }
  if(weight > units_per_person && on_count > count_threshold && on_step == true){
    on_step_enabled = true;
    off_step_enabled = false;
    on_count = 0;
    Serial.print("on_step:");
    Serial.println(weight);  
  }
}

void check_lower_count(){
  if(weight < off_step_thresh && off_step_enabled == false){
    off_step = true;
    on_step = false;
    off_count++;
  }
  else{
    off_count = 0;  
  }
  if(weight < off_step_thresh && off_count > count_threshold && off_step == true){
    off_step_enabled = true;
    on_step_enabled = false;
    off_count = 0;
    Serial.print("off_step:");
    Serial.println(weight);  
  }  
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
