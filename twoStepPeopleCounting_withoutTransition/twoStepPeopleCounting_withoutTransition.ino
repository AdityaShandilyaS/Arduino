#include <Arduino.h>
#include <TM1637Display.h>

#define CLK 2
#define DIO 3

#define inMat A0
#define outMat A2

int processing_delay = 500;
int units_per_person = 20;
int inValue = 0, outValue = 0;
int count = 0;
long start_time, stop_time, jump_check_time;
int threshold_inMat, threshold_outMat;
int inFlag = 0, outFlag = 0;  
int steps[2] = {0, 0};

const uint8_t SEG_strt[] = {
  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,    // S
  SEG_E | SEG_F | SEG_G | SEG_D,            // t
  SEG_E | SEG_G,                            //r
  SEG_E | SEG_F | SEG_G | SEG_D             // t
  };

const uint8_t SEG_wait[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,          //U
  SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,  //A
  SEG_B | SEG_C,                                  //I
  SEG_A | SEG_B | SEG_C                           //T
  };
  
TM1637Display display(CLK, DIO);

void setup() {
  Serial.begin(9600);
  threshold_inMat = calibrate_inMat() + units_per_person;
  Serial.print("threshold_inMat: ");
  Serial.println(threshold_inMat);
  threshold_outMat = calibrate_outMat() + units_per_person;
  Serial.print("threshold_outMat: ");
  Serial.println(threshold_outMat);
  start_time = millis();
  display.setBrightness(7, true);
  display.clear();
  display.setSegments(SEG_strt);
}

void loop() {
  inValue = analogRead(inMat);
  outValue = analogRead(outMat);

  on_step(1, inValue, &threshold_inMat, &inFlag);
  off_step(1, inValue, &threshold_inMat, &inFlag);
  
  on_step(2, outValue, &threshold_outMat, &outFlag);
  off_step(2, outValue, &threshold_outMat, &outFlag);
  
  
  stop_time = millis();
  timeout_refresh();
}

int calibrate_inMat(){
  long total = 0;
  for(int i = 0; i < 100 ; i++){
    total += analogRead(inMat);
    delay(10);
  }
  int avg = total/100;
  Serial.print("inMat avg: ");
  Serial.println(avg);
  return avg;  
}

int calibrate_outMat(){
  long total = 0;
  for(int i = 0; i< 100 ; i++){
    total += analogRead(outMat);
    delay(10);
  }
  int avg = total/100;
  Serial.print("outMat avg: ");
  Serial.println(avg);
  return avg;  
}

void on_step(int number, int step_value, int *threshold, int *flag){
  if(step_value > *threshold){
    check_for_jumps(number);
    //delay(processing_delay);
    *flag += 1;
    *threshold = step_value + units_per_person;
    Serial.print("at step ");
    Serial.print(number);
    Serial.print("    threshold: ");
    Serial.println(*threshold);
    jump_check_time = 0;
    start_time = millis();
  }    
}

void off_step(int number, int step_value, int *threshold, int *flag){
  int temporary_thresh = *threshold - (2 * units_per_person);
  if(step_value < temporary_thresh && *flag > 0){
    *threshold = step_value + units_per_person;
    *flag -= 1;
    process_step(number); 
    check_length();
    jump_check_time = millis(); 
  }
}

void process_step(int number){
  for(int i = 0;i < 2; i++){
    if(number == 1 && steps[0] == 0){
      Serial.println("                                      from step 1");
      steps[0] = number;
      break;
    }
    if(number == 2 && steps[0] == 1){
      Serial.println("                                      from step 2 after 1, counting in");
      steps[1] = number;
      break;  
    }
    if(number == 2 && steps[0] == 0){
      Serial.println("                                      from step 2");
      steps[0] = number;
      break;
    }
    if(number == 1 && steps[0] == 2){
      Serial.println("                                      from step 1 after 2, counting out");
      steps[1] = number;
      break;  
    }
  }
}

void check_length(){
  if(steps[0] == 1 && steps[1] == 2){
    count++;
    display.clear();
    display.showNumberDec(count, false);
    memset(steps, 0, sizeof(steps));
    Serial.print("                                          count: ");
    Serial.println(count);
  }
  if(steps[0] == 2 && steps[1] == 1){
    count--;
    display.clear();
    display.showNumberDec(count, false);
    memset(steps, 0, sizeof(steps));
    Serial.print("                                          count: ");
    Serial.println(count);
  }
}

void timeout_refresh(){
  if((stop_time - start_time) >= 20000){
    Serial.println("timeout, resetting values");
    display.clear();
    display.setSegments(SEG_wait);
    start_time = millis();
    memset(steps, 0, sizeof(steps));
    inFlag = 0;
    outFlag = 0;
    threshold_inMat = calibrate_inMat() + units_per_person;
    Serial.print("threshold_inMat: ");
    Serial.println(threshold_inMat);
    threshold_outMat = calibrate_outMat() + units_per_person;
    Serial.print("threshold_outMat: ");
    Serial.println(threshold_outMat);
    display.clear();
    display.showNumberDec(count, false);
  }  
}
