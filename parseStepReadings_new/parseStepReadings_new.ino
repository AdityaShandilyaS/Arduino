#include<string.h>

#define STEP1 A0
#define STEP2_1 A2
#define STEP2_2 A3
#define STEP3 A4

#define SAMPLING_DELAY 5 // milliseconds
#define VALUE_PER_PERSON 20
#define ARRAY_SIZE 100
#define COUNT_THRESHOLD 5 // values before consideration 

int p1, p2_1, p2_2, p3;
int initial1, initial2_1, initial2_2, initial3;

bool on_step1 = false, on_step2 = false, on_step3 = false;

int step1_array[ARRAY_SIZE] = {0};
int step2_1_array[ARRAY_SIZE] = {0};
int step2_2_array[ARRAY_SIZE] = {0};
int step3_array[ARRAY_SIZE] = {0};
int retrospection_array[COUNT_THRESHOLD] = {0};
int going_in = 2;
int count = 0;

void setup() {
  Serial.begin(115200);
  initial1 = calibrate_sensor1();
  initial2_1 = calibrate_sensor2_1();
  initial2_2 = calibrate_sensor2_2();  
  initial3 = calibrate_sensor3();
  
}

void loop() {
  p1 = analogRead(STEP1);
  delay(0.1);
  check_step(1);
  p2_1 = analogRead(STEP2_1);
  delay(0.1);
  p2_2 = analogRead(STEP2_2);
  delay(0.1);
  check_step(2);
  p3 = analogRead(STEP3);
  delay(0.1);
  check_step(3);
//  display_step_values();
  delay(SAMPLING_DELAY - 0.4);
//  differential_values();
//  check_samples_for_2();
//  display_step2_both();
//  check_step2_retrospect();  
}

void display_step_values(){
  String str = "1: " + String(p1 - initial1) + " 2.1: " + String(p2_1 - initial2_1) + " 2.2: " + String(p2_2 - initial2_2) + " 3: " + String(p3 - initial3);
  Serial.println(str);  
}

void differential_values(){
  int diff_values = p2_1 - p2_2;
  read_into_array(1, diff_values);
  print_array(1);  
}

void check_samples_for_2(){
  int weight = (p2_1 - initial2_1);
  read_into_array(1, weight);
  print_array(1);     
}

void display_step2_both(){
  int weight1 = (p2_1 - initial2_1);
  int weight2 = (p2_2 - initial2_2);
//  String step1= "A0: " + String(weight1) + " A2: " + String(weight2);
  String step2= "A0: " + String(p2_1) + " A2: " + String(p2_2);
  Serial.println(step2);  
}

void check_step2_retrospect(){
  String str = "";
  int weight2_1 = p2_1 - initial2_1;
  int weight2_2 = p2_2 - initial2_2;
  int weight_sum = weight2_1 + weight2_2;
  read_into_array(2.1, weight2_1);
  read_into_array(2.2, weight2_2);
  if(!on_step2 && weight_sum > (VALUE_PER_PERSON * 2)){
    count++;
    if(count > COUNT_THRESHOLD){
      on_step2 = true;
      count = 0;
      retrospect('n');
      str = "2 n " + String(micros()) + " " + String(going_in) + " " +  String(weight_sum);
      Serial.println(str);
//      print_array(1);
//      print_array(2);
    }
  }
  if(on_step2 && weight_sum < (VALUE_PER_PERSON)){
    count++;
    if(count > COUNT_THRESHOLD){
      on_step2 = false;
      count = 0;      
      retrospect('f');
      str = "2 f " + String(micros()) + " " + String(going_in) + " " + String(weight_sum);
      Serial.println(str);
//      print_array(2.1);
//      print_array(2.2);
    }  
  }  
}

void retrospect(char on_off){
  int total = 0;
  int avg = 0;
  for(int i = 0; i < COUNT_THRESHOLD ; i++){
    retrospection_array[i] = step2_1_array[COUNT_THRESHOLD + i] - step2_2_array[COUNT_THRESHOLD + i];
    total+= retrospection_array[i];
  }
  avg = total / COUNT_THRESHOLD;
//  Serial.print(avg);
  if(avg > 0){
//    Serial.println("going IN: ");
    if(on_off == 'n')going_in = 1;
    else going_in = 0;
//    print_retrospection_array();
  }
  else if(avg < 0){
//    Serial.println("going OUT:");
    if(on_off == 'n')going_in = 0;
    else going_in = 1;
//    print_retrospection_array();   
  }
  else{
    going_in = 2;  
  }
}

void print_retrospection_array(){
  for(int i = (COUNT_THRESHOLD - 1) ; i >= 0 ; i--){
    Serial.print(retrospection_array[i]);
    Serial.print(", ");
  }
  Serial.println();
}

void check_step(float step_number){
  String str = "";
  if(step_number == 1){
    int weight = (p1 - initial1);
    read_into_array(step_number, weight);
    if(!on_step1  && weight > VALUE_PER_PERSON){  
      count ++;
      if(count > COUNT_THRESHOLD){
        on_step1 = true;
        count = 0;
        str = "1 n " + String(micros()) + " 2 " + String(weight);
        Serial.println(str);
//        print_array(step_number);
      }
    }
    if(on_step1 && weight < (VALUE_PER_PERSON * 0.5)){ 
      count ++;
      if(count > COUNT_THRESHOLD){      
        on_step1 = false;
        count = 0;
        str = "1 f " + String(micros()) + " 2 " + String(weight);
        Serial.println(str);
//        print_array(step_number);
      }      
    }
  }
  if(step_number == 2){
    check_step2_retrospect();
  }
  if(step_number == 3){
    int weight = (p3 - initial3);
    read_into_array(step_number, weight);
    if(!on_step3 && weight > VALUE_PER_PERSON){  
      count ++;
      if(count > COUNT_THRESHOLD){
        on_step3 = true;
        count = 0;
        str = "3 n " + String(micros()) + " 2 " + String(weight);
        Serial.println(str);
//        print_array(step_number);
      }
    }
    if(on_step3 && weight < (VALUE_PER_PERSON * on_step3 * 0.5)){ 
      count ++;
      if(count > COUNT_THRESHOLD){      
        on_step3 = false;
        count = 0;
        str = "3 f " + String(micros()) + " 2 " + String(weight);
        Serial.println(str);
//        print_array(step_number);
      }      
    }
  }  
}

void read_into_array(float number, int weight){
  if(number == 1){
    for(int i = (ARRAY_SIZE - 1); i > 0 ; i--){
      step1_array[i] = step1_array[i-1];
    }
    step1_array[0] = weight;
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
  if(number == 3){
    for(int i = (ARRAY_SIZE - 1); i > 0 ; i--){
      step3_array[i] = step3_array[i-1];
    }
    step3_array[0] = weight;
  }
}

void print_array(float number){
  if(number == 1){
    Serial.print("step1: ");
    for(int i = (ARRAY_SIZE - 1) ; i >= 0 ; i--){
      Serial.print(step1_array[i]);
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
  if(number == 3){
    Serial.print("step3: ");
    for(int i = (ARRAY_SIZE - 1) ; i >= 0 ; i--){
      Serial.print(step3_array[i]);
      Serial.print(", ");
    }
    Serial.println();
  }  
}

int calibrate_sensor1(){
  long total = 0;
  for(int i = 0; i < 100 ; i++){
    total += analogRead(STEP1);
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

int calibrate_sensor3(){
  long total = 0;
  for(int i = 0; i < 100 ; i++){
    total += analogRead(STEP3);
  }
  int avg = total/100;
  return avg; 
}
