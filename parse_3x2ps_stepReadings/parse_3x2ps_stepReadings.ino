#include<string.h>

#define STEP1_1 A0
#define STEP1_2 A1
#define STEP2_1 A2
#define STEP2_2 A3
#define STEP3_1 A4
#define STEP3_2 A5

#define SAMPLING_DELAY 5 // milliseconds
#define VALUE_PER_PERSON 4
#define ARRAY_SIZE 100
#define COUNT_THRESHOLD 5 // values before consideration 

int p1_1, p1_2, p2_1, p2_2, p3_1, p3_2;
int initial1_1, initial1_2, initial2_1, initial2_2, initial3_1, initial3_2;

bool on_step1 = false, on_step2 = false, on_step3 = false;

int step1_1_array[ARRAY_SIZE] = {0};
int step1_2_array[ARRAY_SIZE] = {0};
int step2_1_array[ARRAY_SIZE] = {0};
int step2_2_array[ARRAY_SIZE] = {0};
int step3_1_array[ARRAY_SIZE] = {0};
int step3_2_array[ARRAY_SIZE] = {0};
int retrospection_array[COUNT_THRESHOLD] = {0};
int difference_array[ARRAY_SIZE] = {0};
int going_in1 = 2, going_in2 = 2, going_in3 = 2;
int count = 0;

void setup() {
  Serial.begin(115200);
  initial1_1 = calibrate_sensor1_1();
  initial1_2 = calibrate_sensor1_2();
  initial2_1 = calibrate_sensor2_1();
  initial2_2 = calibrate_sensor2_2();  
  initial3_1 = calibrate_sensor3_1();
  initial3_2 = calibrate_sensor3_2();
  
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
//  differential_values();
//  check_samples_for_2();
//  display_step2_both(); 
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

void check_step_retrospect(int step_number){
  String str = "";
  if(step_number == 1){
    int weight1_1 = p1_1 - initial1_1;
    int weight1_2 = p1_2 - initial1_2;
    int weight_sum = weight1_1 + weight1_2;
    read_into_array(1.1, weight1_1);
    read_into_array(1.2, weight1_2);
    if(!on_step1 && weight_sum > (VALUE_PER_PERSON * 2)){
      count++;
      if(count > COUNT_THRESHOLD){
        on_step1 = true;
        count = 0;
        retrospect(1, 'n');
        str = "1 n " + String(millis()) + " 2 " +  String(weight_sum);
        Serial.println(str);
//        print_array(1.1);
//        print_array(1.2);
      }
    }
    if(on_step1 && weight_sum < (VALUE_PER_PERSON)){
      count++;
      if(count > COUNT_THRESHOLD){
        on_step1 = false;
        count = 0;      
        retrospect(1, 'f');
        str = "1 f " + String(millis()) + " " + String(going_in1) + " " + String(weight_sum);
        Serial.println(str);
//        print_array(1.1);
//        print_array(1.2);
      }  
    }
  }
  
  if(step_number == 2){
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
        retrospect(2, 'n');
        str = "2 n " + String(millis()) + " 2 " +  String(weight_sum);
        Serial.println(str);
  //      print_array(2.1);
  //      print_array(2.2);
      }
    }
    if(on_step2 && weight_sum < (VALUE_PER_PERSON)){
      count++;
      if(count > COUNT_THRESHOLD){
        on_step2 = false;
        count = 0;      
        retrospect(2, 'f');
        str = "2 f " + String(millis()) + " " + String(going_in2) + " " + String(weight_sum);
        Serial.println(str);
//        print_array(2.1);
//        print_array(2.2);
      }  
    }
  }

  if(step_number == 3){
    int weight3_1 = p3_1 - initial3_1;
    int weight3_2 = p3_2 - initial3_2;
    int weight_sum = weight3_1 + weight3_2;
    read_into_array(3.1, weight3_1);
    read_into_array(3.2, weight3_2);
    if(!on_step3 && weight_sum > (VALUE_PER_PERSON * 2)){
      count++;
      if(count > COUNT_THRESHOLD){
        on_step3 = true;
        count = 0;
        retrospect(3, 'n');
        str = "3 n " + String(millis()) + " 2 " +  String(weight_sum);
        Serial.println(str);
  //      print_array(3.1);
  //      print_array(3.2);
      }
    }
    if(on_step3 && weight_sum < (VALUE_PER_PERSON)){
      count++;
      if(count > COUNT_THRESHOLD){
        on_step3 = false;
        count = 0;      
        retrospect(3, 'f');
        str = "3 f " + String(millis()) + " " + String(going_in3) + " " + String(weight_sum);
        Serial.println(str);
//        print_array(3.1);
//        print_array(3.2);
      }  
    }
  }
}

void retrospect(int step_number, char on_off){
  int total = 0;
  int avg = 0;
//  print_difference_array(step_number);
  if(step_number == 1){
    for(int i = 0; i < COUNT_THRESHOLD ; i++){
      retrospection_array[i] = step1_1_array[COUNT_THRESHOLD + i] - step1_2_array[COUNT_THRESHOLD + i];
      total+= retrospection_array[i];
    }
    avg = total / COUNT_THRESHOLD;
  //  Serial.print(avg);
    if(avg >= 0){
  //    Serial.println("going IN: ");
      if(on_off == 'n')going_in1 = 1;
      else going_in1 = 0;
//      print_retrospection_array();
    }
    else if(avg < 0){
  //    Serial.println("going OUT:");
      if(on_off == 'n')going_in1 = 0;
      else going_in1 = 1;
//      print_retrospection_array();   
    }
//    else{
//      going_in1 = 2;  
//    }
  }
  if(step_number == 2){
    for(int i = 0; i < COUNT_THRESHOLD ; i++){
      retrospection_array[i] = step2_1_array[COUNT_THRESHOLD + i] - step2_2_array[COUNT_THRESHOLD + i];
      total+= retrospection_array[i];
    }
    avg = total / COUNT_THRESHOLD;
  //  Serial.print(avg);
    if(avg >= 0){
  //    Serial.println("going IN: ");
      if(on_off == 'n')going_in2 = 1;
      else going_in2 = 0;
//      print_retrospection_array();
    }
    else if(avg < 0){
  //    Serial.println("going OUT:");
      if(on_off == 'n')going_in2 = 0;
      else going_in2 = 1;
//      print_retrospection_array();   
    }
//    else{
//      going_in2 = 2;  
//    }
  }

  if(step_number == 3){
    for(int i = 0; i < COUNT_THRESHOLD ; i++){
      retrospection_array[i] = step3_1_array[COUNT_THRESHOLD + i] - step3_2_array[COUNT_THRESHOLD + i];
      total+= retrospection_array[i];
    }
    avg = total / COUNT_THRESHOLD;
  //  Serial.print(avg);
    if(avg >= 0){
  //    Serial.println("going IN: ");
      if(on_off == 'n')going_in3 = 1;
      else going_in3 = 0;
//      print_retrospection_array();
    }
    else if(avg < 0){
  //    Serial.println("going OUT:");
      if(on_off == 'n')going_in3 = 0;
      else going_in3 = 1;
//      print_retrospection_array();   
    }
//    else{
//      going_in3 = 2;  
//    }
  }
}

void print_difference_array(int step_number){
  if(step_number == 1){
    memset(difference_array, 0, sizeof(difference_array));
    for(int i = (ARRAY_SIZE - 1); i > 0 ; i--){
      difference_array[i] = step1_1_array[i] - step1_2_array[i];
      Serial.print(" " + String(difference_array[i]));    
    }
    Serial.println();
  }
  if(step_number == 2){
    memset(difference_array, 0, sizeof(difference_array));
    for(int i = (ARRAY_SIZE - 1); i > 0 ; i--){
      difference_array[i] = step2_1_array[i] - step2_2_array[i];
      Serial.print(" " + String(difference_array[i]));    
    }
    Serial.println();
  }
  if(step_number == 3){
    memset(difference_array, 0, sizeof(difference_array));
    for(int i = (ARRAY_SIZE - 1); i > 0 ; i--){
      difference_array[i] = step3_1_array[i] - step3_2_array[i];
      Serial.print(" " + String(difference_array[i]));    
    }
    Serial.println();
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
