#include <Arduino.h>
#include <TM1637Display.h>
#include <HX711.h>

#define DISPLAY_CLK 3
#define DISPLAY_DIO 2

#define inMat A0
#define midMat A2
#define outMat A4

int processing_delay = 400;
int units_per_person = 20;
int inValue = 0, midValue = 0, outValue = 0;
int count = 0;
long start_time, stop_time, jump_check_time, multiple_jump_check_time;
int threshold_inMat, threshold_midMat, threshold_outMat;
int inFlag = 0, midFlag = 0, outFlag = 0;  
int steps[3] = {0, 0, 0};
int multiple_steps[3] = {0, 0, 0};
int transition = 0;
int jumped_from = 0;

const uint8_t SEG_strt[] = {
  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,   // S
  SEG_E | SEG_F | SEG_G | SEG_D,           // t
  SEG_E | SEG_G,                           //r
  SEG_E | SEG_F | SEG_G | SEG_D           // t
  };

const uint8_t SEG_wait[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,          //w
  SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,  //a
  SEG_B | SEG_C,                                  //i
  SEG_A | SEG_B | SEG_C                           //t
  };
  
TM1637Display display(DISPLAY_CLK, DISPLAY_DIO);

void setup() {
  Serial.begin(9600);  
  threshold_inMat = calibrate_inMat() + units_per_person;
  threshold_midMat = calibrate_midMat() + units_per_person;
  threshold_outMat = calibrate_outMat() + units_per_person;
  
  start_time = millis();
  display.setBrightness(7, true);
  display.clear();
  display.setSegments(SEG_strt);
}

void loop() {
  inValue = analogRead(inMat);
  midValue = analogRead(midMat);
  outValue = analogRead(outMat);

  off_step(1, inValue, &threshold_inMat, &inFlag);
//  delay(50);
  on_step(2, midValue, &threshold_midMat, &midFlag);
  transition_step();
  off_step(2, midValue, &threshold_midMat, &midFlag);
  on_step(3, outValue, &threshold_outMat, &outFlag);
  transition_step();
  off_step(3, outValue, &threshold_outMat, &outFlag);  
//  delay(50);
  on_step(1, inValue, &threshold_inMat, &inFlag);
  transition_step();
  off_step(1, inValue, &threshold_inMat, &inFlag);
//  delay(50);
  on_step(2, midValue, &threshold_midMat, &midFlag);
  transition_step();
  off_step(2, midValue, &threshold_midMat, &midFlag);
  on_step(3, outValue, &threshold_outMat, &outFlag);
  transition_step();
  off_step(3, outValue, &threshold_outMat, &outFlag);
  on_step(1, inValue, &threshold_inMat, &inFlag);
  
  stop_time = millis();
   timeout_refresh();
}

int calibrate_inMat(){
  long total = 0;
  for(int i = 0; i < 100 ; i++){
    total += analogRead(inMat);
  }
  int avg = total/100;
  return avg; 
}

int calibrate_midMat(){
  long total = 0;
  for(int i = 0; i< 100 ; i++){
    total += analogRead(midMat);
  }
  int avg = total/100;
  return avg;
}

int calibrate_outMat(){
  long total = 0;
  for(int i = 0; i< 100 ; i++){
    total += analogRead(outMat);
  }
  int avg = total/100;
  return avg;
}

void on_step(int number, int step_value, int *threshold, int *flag){
  if(step_value > *threshold){
    
    Serial.print("step value: ");
    Serial.println(step_value);
    *flag += 1;
    *threshold = step_value + units_per_person;
    Serial.print("at step ");
    Serial.println(number);
//    Serial.print("    threshold: ");
//    Serial.println(*threshold);
    check_for_jumps(number);
    jump_check_time = 0;
    multiple_jump_check_time = 0;
    start_time = millis();
  }    
}

void check_for_jumps(int number){
  Serial.print("time taken: ");
  Serial.println(millis() - jump_check_time);
//  if(inFlag > 0 || midFlag > 0 || (inFlag == midFlag)){
////    Serial.println("checking jumps for multiple people");
////    Serial.print(inFlag);
////    Serial.print(" outFlag: ");
////    Serial.println(outFlag);
//    if(multiple_jump_check_time != 0 && (millis() - multiple_jump_check_time) <= processing_delay){
//      if(number == 1 && transition != 2){
//        transition = 2;
//        multiple_steps[0] = 2;
//        Serial.println("jumped");
//      }
//      
//      if(number == 2 && transition != 1){
//        transition = 1;
//        multiple_steps[0] = 1;
//        Serial.println("jumped");
//      }  
//    }
//  }
//  else if(outFlag > 0 || midFlag > 0 || (outFlag == midFlag)){
////    Serial.println("checking jumps for multiple people");
////    Serial.print(inFlag);
////    Serial.print(" outFlag: ");
////    Serial.println(outFlag);
//    if(multiple_jump_check_time != 0 && (millis() - multiple_jump_check_time) <= processing_delay){
//      if(number == 3 && transition != 2){
//        transition = 2;
//        multiple_steps[0] = 2;
//        Serial.println("jumped");
//      }
//      if(number == 2 && transition != 3){
//        transition = 3;
//        multiple_steps[0] = 3;
//        Serial.println("jumped");
//      }  
//    }
//  }
//  else{    
    if(jump_check_time != 0 && (millis() - jump_check_time) <= processing_delay){
      if(number == 1 && transition != 2){
        transition = 2;
        steps[1] = 2;
        Serial.println("jumped from 2");
      }
      // could have problems with 4th IF stmt - resolved with jumped_from
      if(number == 2 && transition != 1 && jumped_from == 1){ 
        transition = 1;
        steps[0] = 1;
        Serial.println("jumped from 1");
      }
      if(number == 3 && transition != 2){
        transition = 2;
        steps[1] = 2;
        Serial.println("jumped from 2");
      }
      
      if(number == 2 && transition != 3 && jumped_from == 3){
        transition = 3;
        steps[0] = 3;
        Serial.println("jumped from 3");
      } 
    }
//  }
}

void transition_step(){
  if((inFlag + midFlag + outFlag) < 2){
    int half_in_thresh = threshold_inMat - (units_per_person / 2);
    int half_mid_thresh = threshold_midMat - (units_per_person / 2);
    int half_out_thresh = threshold_outMat - (units_per_person / 2);
    
    if(inFlag > 0){
      half_in_thresh = threshold_inMat - ((3 * units_per_person) / 2);
      if(inValue < half_in_thresh && midValue > half_mid_thresh && midValue < threshold_midMat){
        transition = 1;
        Serial.print("transition: ");
        Serial.println(transition);
      }
    }

    if(midFlag > 0){
      half_mid_thresh = threshold_midMat - ((3 * units_per_person) / 2);
      if(midValue < half_mid_thresh && inValue > half_in_thresh && inValue < threshold_inMat){
        transition = 2;
        Serial.print("transition to step 1: ");
        Serial.println(transition);
      }
      if(midValue < half_mid_thresh && outValue > half_out_thresh && outValue < threshold_outMat){
        transition = 2;
        Serial.print("transition to step 3: ");
        Serial.println(transition);
      }
    }
    
    if(outFlag > 0){
      half_out_thresh = threshold_outMat - ((3 * units_per_person) / 2);
      if(outValue < half_out_thresh && midValue > half_mid_thresh && midValue < threshold_midMat){
        transition = 3;
        Serial.print("transition: ");
        Serial.println(transition);
      }
    }
  }
}

void off_step(int number, int step_value, int *threshold, int *flag){
  int temporary_thresh = *threshold - (2 * units_per_person);
  if(step_value < temporary_thresh && *flag > 0){
    Serial.print("off step ");
    Serial.println(number);
    *threshold = step_value + units_per_person;
    *flag -= 1;
//    Serial.println("in between flag and process step");
    process_step(number); 
    check_length();
//    Serial.print("inFlag: ");
//    Serial.print(inFlag);
//    Serial.print(" outFlag: ");
//    Serial.println(outFlag);
    if(number == 1){jumped_from = 1;}
    else if(number == 3){jumped_from = 3;}
    else{jumped_from = 2;}
    jump_check_time = millis();
//    if((inFlag + midFlag + outFlag) > 0){
//      Serial.println("starting multiple check time timer");
////      multiple_jump_check_time = millis();
//    }
//    else{
//      jump_check_time = millis(); 
//    }
  }
}

void process_step(int number){
//  if((inFlag + midFlag + outFlag) > 1){ 
////      Serial.println("process step for multiple people");
//    process_multiple_people_scenarios(number);
//  }
//  // for single person scenarios
//  else{
    //for on step happening before off step
    process_single_person_scenarios(number);
//  }
//  Serial.print("multiple_steps: ");
//  for(int i =0; i < 3; i++){Serial.print(multiple_steps[i]);}
  Serial.print(" steps: ");
  for(int i =0; i < 3; i++){Serial.print(steps[i]);}
  Serial.println();
}

void process_multiple_people_scenarios(int number){
  Serial.println("process step for multiple people");
  for(int i = 0;i < 2; i++){
    //for on step happening before off step
    if(number == 1 && midFlag >= 1){
      Serial.println("on step 2 before off step 1");
      if(midFlag == 1 && inFlag == 0){ // removed  && steps[0] == 0
        multiple_steps[0] = 1;
        transition = 1;
      }
      else{
        transition = 1;
      }
      break;
    }
    if(number == 2 && inFlag >= 1){
      Serial.println("on step 1 before off step 2");
      if(inFlag == 1 && midFlag == 0){ // removed   && steps[0] == 3
        multiple_steps[1] = 2;
        transition = 2;
      }
      else{
        transition = 2;
      }
      break;
    }
    if(number == 2 && outFlag >= 1){
      Serial.println("on step 3 before off step 2");
      if(outFlag == 1 && midFlag == 0){ //   removed && steps[0] == 1
        multiple_steps[1] = 2;
        transition = 2;
      }
      else{
        transition = 2;
      }
      break;
    }
    if(number == 3 && midFlag >= 1){
      Serial.println("on step 2 before off step 3");
      if(midFlag == 1 && outFlag == 0){ //  removed && steps[0] == 0
        multiple_steps[0] = 3;
        transition = 3;
      }
      else{
        transition = 3;
      }
      break;
    }
    //for single person turning into two people scenario
    if(number == 2 && jumped_from == 1 && steps[0] == 1 && (inFlag > 0 || midFlag > 0 || outFlag > 0)){
      Serial.println("single person turning into multiple people");
      if(multiple_steps[0] == 0){
        multiple_steps[0] = 1;
      }
      if(multiple_steps[0] == 1){
        multiple_steps[1] = 2;
        memset(steps, 0, sizeof(steps));
      }
    }
    if(number == 2 && jumped_from == 3 && steps[0] == 3 && (inFlag > 0 || midFlag > 0 || outFlag > 0)){
      Serial.println("single person turning into multiple people");
      if(multiple_steps[0] == 0){
        multiple_steps[0] = 3;
      }
      if(multiple_steps[0] == 3){
        multiple_steps[1] = 2;
        memset(steps, 0, sizeof(steps));
      }
    }  
    if(number == 1 && steps[1] == 2 && (inFlag > 0 || midFlag > 0 || outFlag > 0)){
      Serial.println("single person turning into multiple people");
      if(multiple_steps[0] == 0){
        multiple_steps[0] = 3;
      }
      if(multiple_steps[0] == 3){
        multiple_steps[1] = 2;
        multiple_steps[2] = 1;
        memset(steps, 0, sizeof(steps));
      }
    }
    if(number == 3 && steps[1] == 2 && (inFlag > 0 || midFlag > 0 || outFlag > 0)){
      Serial.println("single person turning into multiple people");
      if(multiple_steps[0] == 0){
        multiple_steps[0] = 1;
      }
      if(multiple_steps[0] == 1){
        multiple_steps[1] = 2;
        multiple_steps[2] = 3;
        memset(steps, 0, sizeof(steps));
      }
    }  
    //for multiple person scenarios      
    if(number == 1 && multiple_steps[0] == 0 && transition == 1){
      multiple_steps[0] = number;
      break;
    }
    if(number == 2 && multiple_steps[0] == 1 && transition == 2){
      multiple_steps[1] = number;
      break;  
    }
    if(number == 3 && multiple_steps[1] == 2 && transition == 2){
      multiple_steps[2] = number;
      break;
    }
    if(number == 3 && multiple_steps[0] == 0 && transition == 3){
      multiple_steps[0] = number;
      break;  
    }
    if(number == 2 && multiple_steps[0] == 3 && transition == 2){
      multiple_steps[1] = number;
      break;
    }
    if(number == 1 && multiple_steps[1] == 2 && transition == 2){
      multiple_steps[2] = number;
      break;  
    }
  }
}

void process_single_person_scenarios(int number){
  for(int i = 0;i < 2; i++){
    if(number == 1 && midFlag >= 1){
    Serial.println("on step 2 before off step 1");
      if(midFlag == 1 && inFlag == 0 && steps[0] == 0){
        steps[0] = 1;
        transition = 1;
      }
      else{
        transition = 1;
      }
      break;
    }
    if(number == 2 && inFlag >= 1){
      Serial.println("on step 1 before off step 2");
      if(inFlag == 1 && midFlag == 0  && steps[0] == 3){
        steps[1] = 2;
        transition = 2;
      }
      else{
        transition = 2;
      }
      break;
    }
    if(number == 2 && outFlag >= 1){
      Serial.println("on step 3 before off step 2");
      if(outFlag == 1 && midFlag == 0  && steps[0] == 1){
        steps[1] = 2;
        transition = 2;
      }
      else{
        transition = 2;
      }
      break;
    }
    if(number == 3 && midFlag >= 1){
      Serial.println("on step 2 before off step 3");
      if(midFlag == 1 && outFlag == 0 && steps[0] == 0){
        steps[0] = 3;
        transition = 3;
      }
      else{
        transition = 3;
      }
      break;
    }
    if(number == 1 && steps[0] == 0 && transition == 1){
      steps[0] = number;
      break;
    }
    if(number == 2 && steps[0] == 1 && transition == 2){
      steps[1] = number;
      break;  
    }
    if(number == 3 && steps[1] == 2 && transition == 2){
      steps[2] = number;
      break;
    }
    if(number == 3 && steps[0] == 0 && transition == 3){
      steps[0] = number;
      break;
    }
    if(number == 2 && steps[0] == 3 && transition == 2){
      steps[1] = number;
      break;
    }
    if(number == 1 && steps[1] == 2 && transition == 2){
      steps[2] = number;
      break;  
    }  
  }
}

void check_length(){
//  if(inFlag > 0 || outFlag > 0){
//    if(multiple_steps[0] == 1 && multiple_steps[1] == 2 && multiple_steps[2] == 3){
//      count++;
////      transition = 0;
//      display.clear();
//      display.showNumberDec(count, false);
//      Serial.print("2nd person count: ");
//      Serial.println(count);
//      memset(multiple_steps, 0, sizeof(multiple_steps));
//    }
//    if(multiple_steps[0] == 3 && multiple_steps[1] == 2 && multiple_steps[2] == 1){
//      count--;
////      transition = 0;
//      display.clear();
//      display.showNumberDec(count, false);
//      Serial.print("2nd person count: ");
//      Serial.println(count);
//      memset(multiple_steps, 0, sizeof(multiple_steps));
//    }  
//  }
  if(steps[0] == 1 && steps[1] == 2 && steps[2] == 3){
    count++;
    transition = 0;
    display.clear();
    display.showNumberDec(count, false);
    Serial.println(count);
    memset(steps, 0, sizeof(steps));
  }
  if(steps[0] == 3 && steps[1] == 2 && steps[2] == 1){
    count--;
    transition = 0;
    display.clear();
    display.showNumberDec(count, false);
    Serial.println(count);
    memset(steps, 0, sizeof(steps));
  }
}

void timeout_refresh(){
  if((stop_time - start_time) >= 20000 && (inFlag + midFlag + outFlag) == 0){
    long start = millis();
    Serial.println("Refreshing values, please wait");
    display.clear();
    display.setSegments(SEG_wait);
    start_time = millis();
    memset(steps, 0, sizeof(steps));
    memset(multiple_steps, 0, sizeof(multiple_steps));
    inFlag = 0;
    outFlag = 0;
    transition = 0;
//    threshold_inMat = calibrate_inMat() + units_per_person;
//    threshold_outMat = calibrate_outMat() + units_per_person;
    display.clear();
    display.showNumberDec(count, false);
    long stopp = millis();
    Serial.println(stopp - start);
  }  
}
