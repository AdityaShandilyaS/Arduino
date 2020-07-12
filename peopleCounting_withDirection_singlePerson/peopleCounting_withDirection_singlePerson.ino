#include<Arduino.h>
#include<string.h>
#include <TM1637Display.h>

#define DISPLAY_CLK 3
#define DISPLAY_DIO 2

#define STEP1_1 A0
#define STEP1_2 A1
#define STEP2_1 A2
#define STEP2_2 A3
#define STEP3_1 A4
#define STEP3_2 A5

#define SAMPLING_DELAY 5 // milliseconds
#define VALUE_PER_PERSON 20
#define ARRAY_SIZE 30
#define COUNT_THRESHOLD 5 // values before consideration 

#define JUMP_DELAY 300 // milliseconds
long jump_check_time = 0;

int p1_1, p1_2, p2_1, p2_2, p3_1, p3_2;
int initial1_1, initial1_2, initial2_1, initial2_2, initial3_1, initial3_2;
int onstep1_threshold, onstep2_threshold, onstep3_threshold;
int offstep1_threshold, offstep2_threshold, offstep3_threshold;

bool array1_active = false, array2_active = false, array3_active = false;
int step_array1[3] = {0};
int step_array2[3]  = {0};
int step_array3[3] = {0}; 
bool on_step1 = false, on_step2 = false, on_step3 = false;
int people_on_setup = 0;
int people_count = 0;
bool number_captured = false;
bool reset_needed = 0;

int step1_1_array[ARRAY_SIZE] = {0};
int step1_2_array[ARRAY_SIZE] = {0};
int step2_1_array[ARRAY_SIZE] = {0};
int step2_2_array[ARRAY_SIZE] = {0};
int step3_1_array[ARRAY_SIZE] = {0};
int step3_2_array[ARRAY_SIZE] = {0};
int retrospection_array[COUNT_THRESHOLD] = {0};
int going_in1 = 2, going_in2 = 2, going_in3 = 2;
int count = 0;
int start_time = 0;

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
  Serial.begin(115200);
  initial1_1 = calibrate_sensor1_1();
  initial1_2 = calibrate_sensor1_2();
  initial2_1 = calibrate_sensor2_1();
  initial2_2 = calibrate_sensor2_2();  
  initial3_1 = calibrate_sensor3_1();
  initial3_2 = calibrate_sensor3_2();
//  start_time = millis();
  display.setBrightness(7, true);
  display.clear();
  display.setSegments(SEG_strt);
}

void loop() {
  p1_1 = analogRead(STEP1_1);
  delay(0.1);
  p1_2 = analogRead(STEP1_2);
  delay(0.1);
  check_step_retrospect(1);
  p2_1 = analogRead(STEP2_1);
  delay(0.1);
  p2_2 = analogRead(STEP2_2);
  delay(0.1);
  check_step_retrospect(2);
  p3_1 = analogRead(STEP3_1);
  delay(0.1);
  p3_2 = analogRead(STEP3_2);
  delay(0.1);
  check_step_retrospect(3);
//  display_step_values();
  delay(SAMPLING_DELAY - 0.6);
//  idle_reset(); 
}

void display_step_values(){
  int weight1_1 = p1_1 - initial1_1;
  int weight1_2 = p1_2 - initial1_2;
  int weight2_1 = p2_1 - initial2_1;
  int weight2_2 = p2_2 - initial2_2;
  int weight3_1 = p3_1 - initial3_1;
  int weight3_2 = p3_2 - initial3_2;
  String str = "1.1: " + String(weight1_1) + " 1.2: " + String(weight1_2) + " 2.1: " + String(weight2_1) + " 2.2: " + String(weight2_2) + " 3.1: " + String(weight3_1) + " 3.2: " + String(weight3_2);
  Serial.println(str);
  delay(250);  
}

void check_step_retrospect(int step_number){
  String str = "";
  if(step_number == 1){
    int weight1_1 = p1_1 - initial1_1;
    int weight1_2 = p1_2 - initial1_2;
    int weight_sum = weight1_1 + weight1_2;
    read_into_array(1.1, weight1_1);
    read_into_array(1.2, weight1_2);
    onstep1_threshold = (VALUE_PER_PERSON * 2 * (on_step1 + 1));
    if(!on_step1 && weight_sum > (VALUE_PER_PERSON * 2)){
      count++;
      if(count > COUNT_THRESHOLD){
        on_step1 = true;
        people_on_setup = on_step1 + on_step2 + on_step3;
        count = 0;
//        retrospect(1, 'n');
        on_step(1);
        str = "1 n " + String(on_step1) + " " + String(millis()) + " 2 " +  String(weight_sum);
//        Serial.println(str);
  //      print_array(1.1);
  //      print_array(1.2);
      }
    }
    offstep1_threshold = (VALUE_PER_PERSON * 2 * on_step1) - VALUE_PER_PERSON;
    if(on_step1 && weight_sum < (VALUE_PER_PERSON)){
      count++;
      if(count > COUNT_THRESHOLD){
        on_step1 = false;
        people_on_setup = on_step1 + on_step2 + on_step3;
        count = 0;      
        retrospect(1, 'f');
        off_step(1);
        str = "1 f " + String(on_step1) + " " + String(millis()) + " " + String(going_in1) + " " + String(weight_sum);
        Serial.println(str);
  //      print_array(1.1);
  //      print_array(1.2);
      }  
    }
  }
  
  if(step_number == 2){
    int weight2_1 = p2_1 - initial2_1;
    int weight2_2 = p2_2 - initial2_2;
    int weight_sum = weight2_1 + weight2_2;
    read_into_array(2.1, weight2_1);
    read_into_array(2.2, weight2_2);
    onstep2_threshold = (VALUE_PER_PERSON * 2 * (on_step2 + 1));
    if(!on_step2 && weight_sum > (VALUE_PER_PERSON * 2)){
      count++;
      if(count > COUNT_THRESHOLD){
        on_step2 = true;
        people_on_setup = on_step1 + on_step2 + on_step3;
        count = 0;
//        retrospect(2, 'n');
        on_step(2);
        str = "2 n " + String(on_step2) + " " + String(millis()) + " 2 " +  String(weight_sum);
//        Serial.println(str);
  //      print_array(2.1);
  //      print_array(2.2);
      }
    }
    offstep2_threshold = (VALUE_PER_PERSON * 2 * on_step2) - VALUE_PER_PERSON;
    if(on_step2 && weight_sum < (VALUE_PER_PERSON)){
      count++;
      if(count > COUNT_THRESHOLD){
        on_step2 = false;
        people_on_setup = on_step1 + on_step2 + on_step3;
        count = 0;      
        retrospect(2, 'f');
        off_step(2);
        str = "2 f " + String(on_step2) + " " + String(millis()) + " " + String(going_in2) + " " + String(weight_sum);
        Serial.println(str);
  //      print_array(2.1);
  //      print_array(2.2);
      }  
    }
  }

  if(step_number == 3){
    int weight3_1 = p3_1 - initial3_1;
    int weight3_2 = p3_2 - initial3_2;
    int weight_sum = weight3_1 + weight3_2;
    read_into_array(3.1, weight3_1);
    read_into_array(3.2, weight3_2);
    onstep3_threshold = (VALUE_PER_PERSON * 2 * (on_step3 + 1));
    if(!on_step3 && weight_sum > (VALUE_PER_PERSON * 2)){
      count++;
      if(count > COUNT_THRESHOLD){
        on_step3 = true;
        people_on_setup = on_step1 + on_step2 + on_step3;
        count = 0;
//        retrospect(3, 'n');
        on_step(3);
        str = "3 n " + String(on_step3) + " " + String(millis()) + " 2 " +  String(weight_sum);
//        Serial.println(str);
  //      print_array(3.1);
  //      print_array(3.2);
      }
    }
    offstep3_threshold = (VALUE_PER_PERSON * 2 * on_step3) - VALUE_PER_PERSON;
    if(on_step3 && weight_sum < (VALUE_PER_PERSON)){
      count++;
      if(count > COUNT_THRESHOLD){
        on_step3 = false;
        people_on_setup = on_step1 + on_step2 + on_step3;
        count = 0;      
        retrospect(3, 'f');
        off_step(3);
        str = "3 f " + String(on_step3) + " " + String(millis()) + " " + String(going_in3) + " " + String(weight_sum);
        Serial.println(str);
  //      print_array(3.1);
  //      print_array(3.2);
      }  
    }
  }
  people_on_setup = on_step1 + on_step2 + on_step3;
}

void retrospect(int step_number, char on_off){
  int total = 0;
  int avg = 0;

  if(step_number == 1){
    for(int i = 0; i < COUNT_THRESHOLD ; i++){
      retrospection_array[i] = step1_1_array[COUNT_THRESHOLD + i] - step1_2_array[COUNT_THRESHOLD + i];
      total+= retrospection_array[i];
    }
    avg = total / COUNT_THRESHOLD;
  //  Serial.print(avg);
    if(avg > 0){
  //    Serial.println("going IN: ");
      if(on_off == 'n')going_in1 = 1;
      else going_in1 = 0;
  //    print_retrospection_array();
    }
    else if(avg < 0){
  //    Serial.println("going OUT:");
      if(on_off == 'n')going_in1 = 0;
      else going_in1 = 1;
  //    print_retrospection_array();   
    }
    else{
      going_in1 = 2;  
    }
  }
  if(step_number == 2){
    for(int i = 0; i < COUNT_THRESHOLD ; i++){
      retrospection_array[i] = step2_1_array[COUNT_THRESHOLD + i] - step2_2_array[COUNT_THRESHOLD + i];
      total+= retrospection_array[i];
    }
    avg = total / COUNT_THRESHOLD;
  //  Serial.print(avg);
    if(avg > 0){
  //    Serial.println("going IN: ");
      if(on_off == 'n')going_in2 = 1;
      else going_in2 = 0;
  //    print_retrospection_array();
    }
    else if(avg < 0){
  //    Serial.println("going OUT:");
      if(on_off == 'n')going_in2 = 0;
      else going_in2 = 1;
  //    print_retrospection_array();   
    }
    else{
      going_in2 = 2;  
    }
  }

  if(step_number == 3){
    for(int i = 0; i < COUNT_THRESHOLD ; i++){
      retrospection_array[i] = step3_1_array[COUNT_THRESHOLD + i] - step3_2_array[COUNT_THRESHOLD + i];
      total+= retrospection_array[i];
    }
    avg = total / COUNT_THRESHOLD;
  //  Serial.print(avg);
    if(avg > 0){
  //    Serial.println("going IN: ");
      if(on_off == 'n')going_in3 = 1;
      else going_in3 = 0;
  //    print_retrospection_array();
    }
    else if(avg < 0){
  //    Serial.println("going OUT:");
      if(on_off == 'n')going_in3 = 0;
      else going_in3 = 1;
  //    print_retrospection_array();   
    }
    else{
      going_in3 = 2;  
    }
  }
}

void on_step(int step_number){
//    check_for_jumps(step_number);  // jump check is useless when on step happens before off step
//    jump_check_time = 0;
//    multiple_jump_check_time = 0;
//    start_time = millis();
  String str = "people on setup@onstep: " + String(people_on_setup);
//  Serial.println(str);  
}

void off_step(int step_number){ 
  process_step_arrays(step_number);
  String str = "people on setup@offstep: " + String(people_on_setup);
//  Serial.println(str); 
}

void process_step_arrays(int step_number){
  String str = "arrays active: " + String(array1_active) + String(array2_active) + String(array3_active);
//  Serial.println(str);
//  if(array1_active){
//    print_array(1);
//  }
//  if(array2_active){
//    print_array(2);
//  }
//  if(array3_active){
//    print_array(3);
//  }
  if(array1_active || array2_active || array3_active){
    number_captured = false;
    if(array1_active && !number_captured){
      int ret_value = scenarios(step_array1, step_number);
//      Serial.println("return value: " + String(ret_value));
      if(ret_value != 0){
        for(int i = 0; i < 3; i++){
          if(step_array1[i] == 0){
            step_array1[i] = ret_value;
            number_captured = true;
            check_array(1);
            break;
          }
        }
      }
      else{
        if(!array2_active){
          if((step_number == 1 && going_in1 == 1)|| (step_number == 3 && going_in3 == 0)){
            array2_active = true;
            step_array2[0] = step_number;
            number_captured = true;
          }
        }
      }
    }
    if(array2_active && !number_captured){
      int ret_value = scenarios(step_array2, step_number);
//      Serial.println("return value: " + String(ret_value));
      if(ret_value != 0){
        for(int i = 0; i < 3; i++){
          if(step_array2[i] == 0){
            step_array2[i] = ret_value;
            number_captured = true;
            check_array(2);
            break;
          }
        }
      }
      else{
        if(!array3_active){
          if((step_number == 1 && going_in1 == 1)|| (step_number == 3 && going_in3 == 0)){
            array3_active = true;
            step_array3[0] = step_number;
            number_captured = true;
          }
        }
      }  
    }
    if(array3_active && !number_captured){
      int ret_value = scenarios(step_array3, step_number);
//      Serial.println("return value: " + String(ret_value));
      if(ret_value != 0){
        for(int i = 0; i < 3; i++){
          if(step_array3[i] == 0){
            step_array3[i] = ret_value;
            number_captured = true;
            check_array(3);
            break;
          }
        }
      }
      else{
        if(!array1_active){
          if((step_number == 1 && going_in1 == 1)|| (step_number == 3 && going_in3 == 0)){
            array1_active = true;
            step_array1[0] = step_number;
            number_captured = true;
          }
        }
      }  
    }
    if(!number_captured){
      check_array_before_resetting(step_number);
//      if((step_number == 1 && going_in1 == 1)|| (step_number == 3 && going_in3 == 0)){
//        Serial.println("number not captured, sacrificing array 1");
//        memset(step_array1, 0, sizeof(step_array1));
//        step_array1[0] = step_number;
//        number_captured = true;
//      }    
    }
//    reset_needed = true;
//    start_time = millis(); 
  }
  else{
//    Serial.println("else-all case");
    if((step_number == 1 && going_in1 == 1)|| (step_number == 3 && going_in3 == 0)){
      memset(step_array1, 0, sizeof(step_array1));
      array1_active = true;
      step_array1[0] = step_number;
      number_captured = true;
    }  
  } 
}

int scenarios(int steps[], int step_number){
//  for(int i = 0; i < 3 ; i++){
//    Serial.println(steps[i]);  
//  }
  if(step_number == 2 && going_in2 == 1 && steps[0] == 1 && steps[1] == 0){return 2;}
  if(step_number == 3 && going_in3 == 1 && steps[0] == 1 && steps[1] == 2 && steps[2] == 0){return 3;}
  
  if(step_number == 2 && going_in2 == 0 && steps[0] == 3 && steps[1] == 0){return 2;}
  if(step_number == 1 && going_in1 == 0 && steps[0] == 3 && steps[1] == 2 && steps[2] == 0){return 1;}
  
  return 0;
}

void check_array(int array_number){
//  print_array(array_number);
  if(array_number == 1){
    if(step_array1[0] == 1 && step_array1[1] == 2 && step_array1[2] == 3){
      check_monkey_business(array_number);
      people_count += 1;
      memset(step_array1, 0, sizeof(step_array1));
      array1_active = false;
      String str = "array: 1 COUNT: " + String(people_count);
      display.clear();
      display.showNumberDec(people_count, false);
      Serial.println(str);
    }
    if(step_array1[0] == 3 && step_array1[1] == 2 && step_array1[2] == 1){
      check_monkey_business(array_number);
      people_count -= 1;
      memset(step_array1, 0, sizeof(step_array1));
      array1_active = false;
      String str = "array: 1 COUNT: " + String(people_count);
      display.clear();
      display.showNumberDec(people_count, false);
      Serial.println(str);
    }  
  }
  if(array_number == 2){
    if(step_array2[0] == 1 && step_array2[1] == 2 && step_array2[2] == 3){
      check_monkey_business(array_number);
      people_count += 1;
      memset(step_array2, 0, sizeof(step_array2));
      array2_active = false;
      String str = "array: 2 COUNT: " + String(people_count);
      display.clear();
      display.showNumberDec(people_count, false);
      Serial.println(str);
    }
    if(step_array2[0] == 3 && step_array2[1] == 2 && step_array2[2] == 1){
      check_monkey_business(array_number);
      people_count -= 1;
      memset(step_array2, 0, sizeof(step_array2));
      array2_active = false;
      String str = "array: 2 COUNT: " + String(people_count);
      display.clear();
      display.showNumberDec(people_count, false);      
      Serial.println(str);
    }  
  }
  if(array_number == 3){
    if(step_array3[0] == 1 && step_array3[1] == 2 && step_array3[2] == 3){
      check_monkey_business(array_number);
      people_count += 1;
      memset(step_array3, 0, sizeof(step_array3));
      array3_active = false;
      String str = "array: 3 COUNT: " + String(people_count);
      display.clear();
      display.showNumberDec(people_count, false);      
      Serial.println(str);
    }
    if(step_array3[0] == 3 && step_array3[1] == 2 && step_array3[2] == 1){
      check_monkey_business(array_number);
      people_count -= 1;
      memset(step_array3, 0, sizeof(step_array3));
      array3_active = false;
      String str = "array: 3 COUNT: " + String(people_count);
      display.clear();
      display.showNumberDec(people_count, false);      
      Serial.println(str);
    }  
  }
}

void check_array_before_resetting(int step_number){
  for(int i = 0; i < 3; i++){
    int array_content = 0;
    if(i == 0){
      for(int j = 0; j < 3; j++){
        if(step_array1[j] != 0){array_content += 1;}  
      }
      if(array_content < 2 && ((step_number == 1 && going_in1 == 1)|| (step_number == 3 && going_in3 == 0))){
        Serial.println("number not captured, sacrificing array 1");
        memset(step_array1, 0, sizeof(step_array1));
        step_array1[0] = step_number;
        number_captured = true;
        break;
      }
    }
    if(i == 1){
      for(int j = 0; j < 3; j++){
        if(step_array2[j] != 0){array_content += 1;}  
      }
      if(array_content < 2 && ((step_number == 1 && going_in1 == 1)|| (step_number == 3 && going_in3 == 0))){
        Serial.println("number not captured, sacrificing array 2");
        memset(step_array2, 0, sizeof(step_array2));
        step_array2[0] = step_number;
        number_captured = true;
        break;
      }
    }
    if(i == 2){
      for(int j = 0; j < 3; j++){
        if(step_array3[j] != 0){array_content += 1;}  
      }
      if(array_content < 2 && ((step_number == 1 && going_in1 == 1)|| (step_number == 3 && going_in3 == 0))){
        Serial.println("number not captured, sacrificing array 3");
        memset(step_array3, 0, sizeof(step_array3));
        step_array3[0] = step_number;
        number_captured = true;
        break;
      }
    }
  }
  if(!number_captured){
    if(step_array1[0] == 1 && step_array1[1] == 2){people_count += 1;}
    else if(step_array1[0] == 3 && step_array1[1] == 2){people_count -= 1;}
    Serial.println("number not captured normally, sacrificing array 1");
    memset(step_array1, 0, sizeof(step_array1));
    step_array1[0] = step_number;
    number_captured = true;    
  }    
}

void check_monkey_business(int array_number){ 
  if(people_on_setup == 0){
    if(array_number == 1){
      if(array3_active){
        if(step_array3[1] == 2){
          if(step_array3[0] == 1 && step_array3[1] == 2){people_count += 1;}
          else if(step_array3[0] == 3 && step_array3[1] == 2){people_count -= 1;}
          memset(step_array3, 0, sizeof(step_array3));
          array3_active = false;
          Serial.println("monkey business spotted and resolved at array 3");  
        }  
      }
      if(array2_active){
        if(step_array2[1] == 2){
          if(step_array2[0] == 1 && step_array2[1] == 2){people_count += 1;}
          else if(step_array2[0] == 3 && step_array2[1] == 2){people_count -= 1;}
          memset(step_array2, 0, sizeof(step_array2));
          array2_active = false;
          Serial.println("monkey business spotted and resolved at array 2");  
        }  
      }  
    }
    if(array_number == 2){
      if(array1_active){
        if(step_array1[1] == 2){
          if(step_array1[0] == 1 && step_array1[1] == 2){people_count += 1;}
          else if(step_array1[0] == 3 && step_array1[1] == 2){people_count -= 1;}
          memset(step_array1, 0, sizeof(step_array1));
          array1_active = false;
          Serial.println("monkey business spotted and resolved at array 1");  
        }  
      }
      if(array3_active){
        if(step_array3[1] == 2){
          if(step_array3[0] == 1 && step_array3[1] == 2){people_count += 1;}
          else if(step_array3[0] == 3 && step_array3[1] == 2){people_count -= 1;}
          memset(step_array3, 0, sizeof(step_array3));
          array3_active = false;
          Serial.println("monkey business spotted and resolved at array 3");  
        }  
      }  
    }
    if(array_number == 3){
      if(array2_active){
        if(step_array2[1] == 2){
          if(step_array2[0] == 1 && step_array2[1] == 2){people_count += 1;}
          else if(step_array2[0] == 3 && step_array2[1] == 2){people_count -= 1;}
          memset(step_array2, 0, sizeof(step_array2));
          array2_active = false;
          Serial.println("monkey business spotted and resolved at array 2");  
        }  
      }
      if(array1_active){
        if(step_array1[1] == 2){
          if(step_array1[0] == 1 && step_array1[1] == 2){people_count += 1;}
          else if(step_array1[0] == 3 && step_array1[1] == 2){people_count -= 1;}
          memset(step_array1, 0, sizeof(step_array1));
          array1_active = false;
          Serial.println("monkey business spotted and resolved at array 1");  
        }  
      }  
    }    
  }      
}

void print_array(int array_number){
  String str = "array: " + String(array_number);
  if(array_number == 1){
    str += " steps:";
    for(int i = 0; i < 3; i++){
      str += " " + String(step_array1[i]); 
    }
    Serial.println(str);  
  }
  if(array_number == 2){
    str += " steps:";
    for(int i = 0; i < 3; i++){
      str += " " + String(step_array2[i]); 
    }
    Serial.println(str);  
  }
    if(array_number == 3){
    str += " steps:";
    for(int i = 0; i < 3; i++){
      str += " " + String(step_array3[i]); 
    }
    Serial.println(str);  
  }  
}

void idle_reset(){
  if(people_on_setup == 0 && reset_needed && ((millis() - start_time) > 10000)){
    display.clear();
    display.setSegments(SEG_wait);
    Serial.println("resetting arrays since no one is on step...");
    memset(step_array1, 0, sizeof(step_array1));
    array1_active = false;
    memset(step_array2, 0, sizeof(step_array2));
    array2_active = false;
    memset(step_array3, 0, sizeof(step_array3));
    array3_active = false;
    start_time = millis();
    reset_needed = false;
    display.clear();
    display.showNumberDec(people_count, false);       
  }  
}

void print_retrospection_array(){
  for(int i = (COUNT_THRESHOLD - 1) ; i >= 0 ; i--){
    Serial.print(retrospection_array[i]);
    Serial.print(", ");
  }
  Serial.println();
}

void read_into_array(float number, int weight){
  if(number == 1.1){
    for(int i = (ARRAY_SIZE - 1); i > 0 ; i--){
      step1_1_array[i] = step1_1_array[i-1];
    }
    step1_1_array[0] = weight;
  }
  if(number == 1.2){
    for(int i = (ARRAY_SIZE - 1); i > 0 ; i--){
      step1_2_array[i] = step1_2_array[i-1];
    }
    step1_2_array[0] = weight;
  }
  if(number == 2.1){
    for(int i = (ARRAY_SIZE - 1); i > 0 ; i--){
      step2_1_array[i] = step2_1_array[i-1];
    }
    step2_1_array[0] = weight;
  }
  if(number == 2.2){
    for(int i = (ARRAY_SIZE - 1); i > 0 ; i--){
      step2_2_array[i] = step2_2_array[i-1];
    }
    step2_2_array[0] = weight;
  }
  if(number == 3.1){
    for(int i = (ARRAY_SIZE - 1); i > 0 ; i--){
      step3_1_array[i] = step3_1_array[i-1];
    }
    step3_1_array[0] = weight;
  }
  if(number == 3.2){
    for(int i = (ARRAY_SIZE - 1); i > 0 ; i--){
      step3_2_array[i] = step3_2_array[i-1];
    }
    step3_2_array[0] = weight;
  }
}

void print_array(float number){
  if(number == 1.1){
    Serial.print("step1_1: ");
    for(int i = (ARRAY_SIZE - 1) ; i >= 0 ; i--){
      Serial.print(step1_1_array[i]);
      Serial.print(", ");
    }
    Serial.println();
  }
  if(number == 1.2){
    Serial.print("step1_2: ");
    for(int i = (ARRAY_SIZE - 1) ; i >= 0 ; i--){
      Serial.print(step1_2_array[i]);
      Serial.print(", ");
    }
    Serial.println();
  }
  if(number == 2.1){
    Serial.print("step2_1: ");
    for(int i = (ARRAY_SIZE - 1) ; i >= 0 ; i--){
      Serial.print(step2_1_array[i]);
      Serial.print(", ");
    }
    Serial.println();
  }
  if(number == 2.2){
    Serial.print("step2_2: ");
    for(int i = (ARRAY_SIZE - 1) ; i >= 0 ; i--){
      Serial.print(step2_2_array[i]);
      Serial.print(", ");
    }
    Serial.println();
  }
  if(number == 3.1){
    Serial.print("step3_1: ");
    for(int i = (ARRAY_SIZE - 1) ; i >= 0 ; i--){
      Serial.print(step3_1_array[i]);
      Serial.print(", ");
    }
    Serial.println();
  }
  if(number == 3.2){
    Serial.print("step3_2: ");
    for(int i = (ARRAY_SIZE - 1) ; i >= 0 ; i--){
      Serial.print(step3_2_array[i]);
      Serial.print(", ");
    }
    Serial.println();
  }  
}

int calibrate_sensor1_1(){
  long total = 0;
  for(int i = 0; i < 100 ; i++){
    total += analogRead(STEP1_1);
  }
  int avg = total/100;
  return avg; 
}

int calibrate_sensor1_2(){
  long total = 0;
  for(int i = 0; i < 100 ; i++){
    total += analogRead(STEP1_2);
  }
  int avg = total/100;
  return avg; 
}

int calibrate_sensor2_1(){
  long total = 0;
  for(int i = 0; i < 100 ; i++){
    total += analogRead(STEP2_1);
  }
  int avg = total/100;
  return avg; 
}

int calibrate_sensor2_2(){
  long total = 0;
  for(int i = 0; i < 100 ; i++){
    total += analogRead(STEP2_2);
  }
  int avg = total/100;
  return avg; 
}

int calibrate_sensor3_1(){
  long total = 0;
  for(int i = 0; i < 100 ; i++){
    total += analogRead(STEP3_1);
  }
  int avg = total/100;
  return avg; 
}

int calibrate_sensor3_2(){
  long total = 0;
  for(int i = 0; i < 100 ; i++){
    total += analogRead(STEP3_2);
  }
  int avg = total/100;
  return avg; 
}
