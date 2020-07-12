#include <Arduino.h>
#include <TM1637Display.h>
#include <HX711.h>

#define DISPLAY_CLK 5
#define DISPLAY_DIO 4

#define LOADCELL1_CLK 3
#define LOADCELL1_DIO 2

#define LOADCELL2_CLK 7
#define LOADCELL2_DIO 6

int processing_delay = 400;
int units_per_person = 35;
int inValue = 0, outValue = 0;
int count = 0;
long start_time, stop_time, jump_check_time, multiple_jump_check_time;
int threshold_inMat, threshold_outMat;
int inFlag = 0, outFlag = 0;  
int steps[2] = {0, 0};
int multiple_steps[2] = {0, 0};
int transition = 0;
float calibration_factor1 = -20000; // -25000 best value for general CF so far
float calibration_factor2 = -15000; // -20000 and -15000 perfect CF1 and 2 combo so far
int person_weight1 = 0;
int person_weight2 = 0;

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

HX711 scale1;
HX711 scale2;
  
TM1637Display display(DISPLAY_CLK, DISPLAY_DIO);

void setup() {
  Serial.begin(9600);
  calibrate_inMat();
  calibrate_outMat();
  threshold_inMat = scale1.get_units() + units_per_person;
  threshold_outMat = scale2.get_units() + units_per_person;
  
  start_time = millis();
  display.setBrightness(7, true);
  display.clear();
  display.setSegments(SEG_strt);
}

void loop() {
  inValue = scale1.get_units();
  outValue = scale2.get_units();

  off_step(1, inValue, &threshold_inMat, &inFlag);
//  delay(50);
  on_step(2, outValue, &threshold_outMat, &outFlag);
  transition_step();
  off_step(2, outValue, &threshold_outMat, &outFlag);
//  delay(50);
  on_step(1, inValue, &threshold_inMat, &inFlag);
  transition_step();
  off_step(1, inValue, &threshold_inMat, &inFlag);
//  delay(50);
  on_step(2, outValue, &threshold_outMat, &outFlag);
  
  stop_time = millis();
  timeout_refresh();
}

int calibrate_inMat(){
  scale1.begin(LOADCELL1_DIO, LOADCELL1_CLK);
  scale1.set_scale();
  scale1.tare(); //Reset the scale to 0

  long zero_factor = scale1.read_average();
  scale1.set_scale(calibration_factor1);
}

int calibrate_outMat(){
  scale2.begin(LOADCELL2_DIO, LOADCELL2_CLK);
  scale2.set_scale();
  scale2.tare(); //Reset the scale to 0

  long zero_factor = scale2.read_average();
  scale2.set_scale(calibration_factor2); 
}

void transition_step(){
  if((inFlag + outFlag) < 2){
    int half_in_thresh = threshold_inMat - (units_per_person / 2);
    int half_out_thresh = threshold_outMat - (units_per_person / 2);
    
    if(inFlag > 0){
      half_in_thresh = threshold_inMat - ((3 * units_per_person) / 2);
      if(inValue < half_in_thresh && outValue > half_out_thresh && outValue < threshold_outMat){
        transition = 1;
        Serial.print("transition: ");
        Serial.println(transition);
      }
    }
    
    if(outFlag > 0){
      half_out_thresh = threshold_outMat - ((3 * units_per_person) / 2);
      if(outValue < half_out_thresh && inValue > half_in_thresh && inValue < threshold_inMat){
        transition = 2;
        Serial.print("transition: ");
        Serial.println(transition);
      }
    }
  }
}

void check_for_jumps(int number){
  if(inFlag > 1 || outFlag > 1 || (inFlag == outFlag)){
//    Serial.println("checking jumps for multiple people");
//    Serial.print(inFlag);
//    Serial.print(" outFlag: ");
//    Serial.println(outFlag);
    if(multiple_jump_check_time != 0 && (millis() - multiple_jump_check_time) <= processing_delay){
      if(number == 1 && transition != 2){
        transition = 2;
        multiple_steps[0] = 2;
        Serial.println("jumped");
      }
      
      if(number == 2 && transition != 1){
        transition = 1;
        multiple_steps[0] = 1;
        Serial.println("jumped");
      }  
    }
  }
  else{    
    if(jump_check_time != 0 && (millis() - jump_check_time) <= processing_delay){
      if(number == 1 && transition != 2){
        transition = 2;
        steps[0] = 2;
        Serial.println("jumped");
      }
      
      if(number == 2 && transition != 1){
        transition = 1;
        steps[0] = 1;
        Serial.println("jumped");
      }  
    }
  }
}

int weight_averager(int number){
  int step_total = 0;
  int step_average;
  if(number == 1){
    for(int i = 0; i < 10 ; i++){
      step_total += scale1.get_units();
    }
    return step_average = step_total/10;  
  }
  if(number == 2){
    for(int i = 0; i < 10 ; i++){
      step_total += scale2.get_units();
    }
    return step_average = step_total/10;  
  }  
}

void on_step(int number, int step_value, int *threshold, int *flag){
  if(step_value > *threshold){
    step_value = weight_averager(number);
    *flag += 1;
    *threshold = step_value + units_per_person;
    assign_weights(step_value, number);
//    Serial.print("step value: ");
//    Serial.println(step_value);
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

void assign_weights(int step_value, int number){
  if(person_weight1 == 0)
  person_weight1 = step_value;
  else if((person_weight1 + 5) > step_value && step_value > (person_weight1 - 5)){
    if(number == 1){
      transition = 2;
      steps[0] = 2;
    }
    if(number == 2){
      transition = 1;
      steps[0] = 1;  
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
    if(inFlag + outFlag > 1){
      Serial.println("starting multiple check time timer");
      multiple_jump_check_time = millis();
    }
    else{
      jump_check_time = millis(); 
    }
  }
}

void process_step(int number){
  for(int i = 0;i < 2; i++){
    if(inFlag > outFlag || inFlag < outFlag){
//      Serial.println("process step for multiple people");
      //for on step happening before off step
      if(number == 2 && inFlag >= 1 && steps[0] == 0){
        Serial.println("on step happening before off step");
        if(inFlag == 1 && outFlag == 0){
          steps[0] = 2;
          transition = 2;
        }
        else{
          multiple_steps[0] = 2;
          transition = 2;
        }
        break;
      }
      if(number == 1 && outFlag >= 1 && steps[0] == 0){
        Serial.println("on step happening before off step");
        if(outFlag == 1 && inFlag == 0){
          steps[0] = 1;
          transition = 1;
        }
        else{
          multiple_steps[0] = 1;
          transition = 1;
        }
        break;
      }
      //for single person turning into two people scenario
      if(number == 2 && steps[0] == 1 && (inFlag > 0 || outFlag > 0)){
        Serial.println("single person turning into multiple people");
        if(multiple_steps[0] == 0){
          multiple_steps[0] = 1;
        }
        if(multiple_steps[0] == 1){
          multiple_steps[1] = 2;
          memset(steps, 0, sizeof(steps));
        }
      }
      if(number == 1 && steps[0] == 2 && (inFlag > 0 || outFlag > 0)){
        Serial.println("single person turning into multiple people");
        if(multiple_steps[0] == 0){
          multiple_steps[0] = 2;
        }
        if(multiple_steps[0] == 1){
          multiple_steps[1] = 1;
          memset(steps, 0, sizeof(steps));
        }
      }
      //for multiple person scenarios      
      if(number == 1 && multiple_steps[0] == 0 && transition == 1){
        multiple_steps[0] = number;
        break;
      }
      if(number == 2 && multiple_steps[0] == 1 && transition == 1){
        multiple_steps[1] = number;
        break;  
      }
      if(number == 2 && multiple_steps[0] == 0 && transition == 2){
        multiple_steps[0] = number;
        break;
      }
      if(number == 1 && multiple_steps[0] == 2 && transition == 2){
        multiple_steps[1] = number;
        break;  
      }
    }
    //for single person scenarios
    else{
      if(number == 1 && steps[0] == 0 && transition == 1){
        steps[0] = number;
        break;
      }
      if(number == 2 && steps[0] == 1 && transition == 1){
        steps[1] = number;
        break;  
      }
      if(number == 2 && steps[0] == 0 && transition == 2){
        steps[0] = number;
        break;
      }
      if(number == 1 && steps[0] == 2 && transition == 2){
        steps[1] = number;
        break;  
      }
    }
  }
}

void check_length(){
  if(inFlag > 0 || outFlag > 0){
    if(multiple_steps[0] == 1 && multiple_steps[1] == 2){
      count++;
//      transition = 0;
      display.clear();
      display.showNumberDec(count, false);
      Serial.print("2nd person count: ");
      Serial.println(count);
      memset(multiple_steps, 0, sizeof(multiple_steps));
    }
    if(multiple_steps[0] == 2 && multiple_steps[1] == 1){
      count--;
//      transition = 0;
      display.clear();
      display.showNumberDec(count, false);
      Serial.print("2nd person count: ");
      Serial.println(count);
      memset(multiple_steps, 0, sizeof(multiple_steps));
    }  
  }
  else if(person_weight1 != inValue || person_weight1 != outValue){
    if(steps[0] == 1 && steps[1] == 2){
      count++;
      transition = 0;
      person_weight1 = 0;
      display.clear();
      display.showNumberDec(count, false);
      Serial.println(count);
      memset(steps, 0, sizeof(steps));
    }
    if(steps[0] == 2 && steps[1] == 1){
      count--;
      transition = 0;
      person_weight1 = 0;
      display.clear();
      display.showNumberDec(count, false);
      Serial.println(count);
      memset(steps, 0, sizeof(steps));
    }
  }
}

void timeout_refresh(){
  if((stop_time - start_time) >= 30000){
    display.clear();
    display.setSegments(SEG_wait);
    start_time = millis();
    memset(steps, 0, sizeof(steps));
    memset(multiple_steps, 0, sizeof(multiple_steps));
    inFlag = 0;
    outFlag = 0;
    transition = 0;
    calibrate_inMat();
    calibrate_outMat();
    threshold_inMat = scale1.get_units() + units_per_person;
    threshold_outMat = scale2.get_units() + units_per_person;
    display.clear();
    display.showNumberDec(count, false);
  }  
}
