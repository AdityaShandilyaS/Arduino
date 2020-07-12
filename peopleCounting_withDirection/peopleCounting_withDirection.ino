#include<string.h>

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
int steps[3] = {0};
int multiple_steps[3] = {0};
int transition = 0;
int jumped_from = 0;

int p1_1, p1_2, p2_1, p2_2, p3_1, p3_2;
int initial1_1, initial1_2, initial2_1, initial2_2, initial3_1, initial3_2;

bool array1_active = true, array2_active = false, array3_active = false;
int step_array1[3] = {0};
int step_array2[3]  = {0};
int step_array3[3] = {0}; 
int on_step1 = 0, on_step2 = 0, on_step3 = 0;
int people_on_setup = 0;
int people_count = 0;

int step1_1_array[ARRAY_SIZE] = {0};
int step1_2_array[ARRAY_SIZE] = {0};
int step2_1_array[ARRAY_SIZE] = {0};
int step2_2_array[ARRAY_SIZE] = {0};
int step3_1_array[ARRAY_SIZE] = {0};
int step3_2_array[ARRAY_SIZE] = {0};
int retrospection_array[COUNT_THRESHOLD] = {0};
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

void check_step_retrospect(int step_number){
  String str = "";
  if(step_number == 1){
    int weight1_1 = p1_1 - initial1_1;
    int weight1_2 = p1_2 - initial1_2;
    int weight_sum = weight1_1 + weight1_2;
    read_into_array(1.1, weight1_1);
    read_into_array(1.2, weight1_2);
    if(on_step1 >= 0 && weight_sum > (VALUE_PER_PERSON * 2 * (on_step1 + 1))){
      count++;
      if(count > COUNT_THRESHOLD){
        on_step1 += 1;
        count = 0;
//        retrospect(1, 'n');
        on_step(1);
        str = "1 n " + String(millis()) + " 2 " +  String(weight_sum);
        Serial.println(str);
  //      print_array(1.1);
  //      print_array(1.2);
      }
    }
    if(on_step1 > 0 && weight_sum < (VALUE_PER_PERSON * 2 * on_step1) && weight_sum > (VALUE_PER_PERSON * on_step1)){
      count++;
      if(count > COUNT_THRESHOLD){
        count = 0;
        check_for_transitions(1);  
      }
    }
    if(on_step1 > 0 && weight_sum < (VALUE_PER_PERSON * on_step1)){
      count++;
      if(count > COUNT_THRESHOLD){
        on_step1 -= 1;
        people_on_setup = on_step1 + on_step2 + on_step3;
        count = 0;      
        retrospect(1, 'f');
        off_step(1);
        str = "1 f " + String(millis()) + " " + String(going_in1) + " " + String(weight_sum);
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
    if(on_step2 >= 0 && weight_sum > (VALUE_PER_PERSON * 2 * (on_step2 + 1))){
      count++;
      if(count > COUNT_THRESHOLD){
        on_step2 += 1;
        count = 0;
//        retrospect(2, 'n');
        on_step(2);
        str = "2 n " + String(millis()) + " 2 " +  String(weight_sum);
        Serial.println(str);
  //      print_array(2.1);
  //      print_array(2.2);
      }
    }
    if(on_step2 > 0 && weight_sum < (VALUE_PER_PERSON * 2 * on_step2) && weight_sum > (VALUE_PER_PERSON * on_step2)){
      count++;
      if(count > COUNT_THRESHOLD){
        count = 0;
        check_for_transitions(2);  
      }
    }
    if(on_step2 > 0 && weight_sum < (VALUE_PER_PERSON * on_step2)){
      count++;
      if(count > COUNT_THRESHOLD){
        on_step2 -= 1;
        people_on_setup = on_step1 + on_step2 + on_step3;
        count = 0;      
        retrospect(2, 'f');
        off_step(2);
        str = "2 f " + String(millis()) + " " + String(going_in2) + " " + String(weight_sum);
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
    if(on_step3 >= 0 && weight_sum > (VALUE_PER_PERSON * 2 * (on_step3 + 1))){
      count++;
      if(count > COUNT_THRESHOLD){
        on_step3 += 1;
        count = 0;
//        retrospect(3, 'n');
        on_step(3);
        str = "3 n " + String(millis()) + " 2 " +  String(weight_sum);
        Serial.println(str);
  //      print_array(3.1);
  //      print_array(3.2);
      }
    }
    if(on_step3 > 0 && weight_sum < (VALUE_PER_PERSON * 2 * on_step3) && weight_sum > (VALUE_PER_PERSON * on_step3)){
      count++;
      if(count > (COUNT_THRESHOLD * 0.5)){
        count = 0;
        check_for_transitions(3);  
      }
    }
    if(on_step3 > 0 && weight_sum < (VALUE_PER_PERSON * on_step3)){
      count++;
      if(count > COUNT_THRESHOLD){
        on_step3 -= 1;
        people_on_setup = on_step1 + on_step2 + on_step3;
        count = 0;      
        retrospect(3, 'f');
        off_step(3);
        str = "3 f " + String(millis()) + " " + String(going_in3) + " " + String(weight_sum);
        Serial.println(str);
  //      print_array(3.1);
  //      print_array(3.2);
      }  
    }
  }
  people_on_setup = on_step1 + on_step2 + on_step3;
}

void check_for_transitions(int step_number){
  if(step_number == 1){
    int weight2_1 = p2_1 - initial2_1;
    int weight2_2 = p2_2 - initial2_2;
    int weight_sum2 = weight2_1 + weight2_2;
    if(weight_sum2 < (VALUE_PER_PERSON * 2) && weight_sum2 > VALUE_PER_PERSON){
      transition = 1;  
    }    
  }
  if(step_number == 2){
    int weight1_1 = p1_1 - initial1_1;
    int weight1_2 = p1_2 - initial1_2;
    int weight_sum1 = weight1_1 + weight1_2;
    if(weight_sum1 < (VALUE_PER_PERSON * 2) && weight_sum1 > VALUE_PER_PERSON){
      transition = -2;  
    }
    int weight3_1 = p3_1 - initial3_1;
    int weight3_2 = p3_2 - initial3_2;
    int weight_sum3 = weight3_1 + weight3_2;
    if(weight_sum3 < (VALUE_PER_PERSON * 2) && weight_sum3 > VALUE_PER_PERSON){
      transition = 2;  
    }    
  }
  if(step_number == 3){
    int weight2_1 = p2_1 - initial2_1;
    int weight2_2 = p2_2 - initial2_2;
    int weight_sum2 = weight2_1 + weight2_2;
    if(weight_sum2 < (VALUE_PER_PERSON * 2) && weight_sum2 > VALUE_PER_PERSON){
      transition = 3;  
    }
  }
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

void on_step(int step_number){
//    check_for_jumps(step_number);  // jump check is useless when on step happens before off step
    jump_check_time = 0;
//    multiple_jump_check_time = 0;
//    start_time = millis();  
}

void check_for_jumps(int step_number){
  if(jump_check_time != 0 && ((millis() - jump_check_time) <= JUMP_DELAY)){
    if(step_number == 1 && transition != 2 && going_in2 == 0){
      transition = 2;
      steps[1] = 2;
      Serial.println("jumped to 1");
    }
    // could have problems with 4th IF stmt - resolved with jumped_from
    if(step_number == 2 && transition != 1 && jumped_from == 1 && going_in1 == 1){ 
      transition = 1;
      steps[0] = 1;
      Serial.println("jumped from 1");
    }
    if(step_number == 3 && transition != 2 && going_in2 == 1){
      transition = 2;
      steps[1] = 2;
      Serial.println("jumped to 3");
    }
    
    if(step_number == 2 && transition != 3 && jumped_from == 3 && going_in3 == 0){
      transition = 3;
      steps[0] = 3;
      Serial.println("jumped from 3");
    } 
  }    
}

void off_step(int step_number){
    process_step(step_number); 
    check_length();
    if(step_number == 1){jumped_from = 1;}
    else if(step_number == 3){jumped_from = 3;}
    else{
      if(going_in2 == 1);jumped_from = 2;
      if(going_in2 == 0);jumped_from = -2;
      }
    jump_check_time = millis();
//    jump_timer();  
}

void process_step(int step_number){
  String str = "people on setup: " + String(people_on_setup);
  Serial.println(str);
  if(people_on_setup > 0){
    process_multiple_people_scenarios(step_number);
  }
  else{
    process_single_person_scenarios(step_number);  
  }  
}

void process_single_person_scenarios(int step_number){
  for(int i = 0;i < 3; i++){
//    if(step_number == 1 && going_in1 == 1){
//    Serial.println("on step 2 before off step 1");
//      if(steps[0] == 0){
//        steps[0] = 1;
//        transition = 1;
//      }
//      else{
//        transition = 1;
//      }
//      break;
//    }
//    if(step_number == 2 && going_in2 == 0){
//      Serial.println("on step 1 before off step 2");
//      if(steps[0] == 3){
//        steps[1] = 2;
//        transition = 2;
//      }
//      else{
//        transition = 2;
//      }
//      break;
//    }
//    if(step_number == 2 && going_in2 == 1){
//      Serial.println("on step 3 before off step 2");
//      if(steps[0] == 1){
//        steps[1] = 2;
//        transition = 2;
//      }
//      else{
//        transition = 2;
//      }
//      break;
//    }
//    if(step_number == 3 && going_in3 == 0){
//      Serial.println("on step 2 before off step 3");
//      if(steps[0] == 0){
//        steps[0] = 3;
//        transition = 3;
//      }
//      else{
//        transition = 3;
//      }
//      break;
//    }
    if(step_number == 1 && steps[0] == 0 && transition == 1){
      steps[0] = step_number;
      break;
    }
    if(step_number == 2 && steps[0] == 1 && transition == 2){
      steps[1] = step_number;
      break;  
    }
    if(step_number == 3 && steps[1] == 2 && transition == 2){
      steps[2] = step_number;
      break;
    }
    if(step_number == 3 && steps[0] == 0 && transition == 3){
      steps[0] = step_number;
      break;
    }
    if(step_number == 2 && steps[0] == 3 && transition == -2){
      steps[1] = step_number;
      break;
    }
    if(step_number == 1 && steps[1] == 2 && transition == -2){
      steps[2] = step_number;
      break;  
    }
  }
}

void process_multiple_people_scenarios(int step_number){
  for(int i = 0; i < 3; i++){
  // single person turning into multiple people (single person serial walk-through) 
    if(step_number == 2 && going_in2 == 1 && steps[0] == 1){
      Serial.println("single person turning into multiple people");
      if(multiple_steps[0] == 0){
        multiple_steps[0] = 1;
      }
      if(multiple_steps[0] == 1){
        multiple_steps[1] = 2;
        transition = 0;
        memset(steps, 0, sizeof(steps));
      }
    }
    if(step_number == 2 && going_in2 == 0 && steps[0] == 3){
      Serial.println("single person turning into multiple people");
      if(multiple_steps[0] == 0){
        multiple_steps[0] = 3;
      }
      if(multiple_steps[0] == 3){
        multiple_steps[1] = 2;
        transition = 0;
        memset(steps, 0, sizeof(steps));
      }
    }  
    if(step_number == 1 && steps[1] == 2 && going_in1 == 0){
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
    if(step_number == 3 && steps[1] == 2 && going_in3 == 1){
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
    //for on step happening before off step
    if(step_number == 1 && going_in1 == 1){
      Serial.println("on step 2 before off step 1");
      // changed the second part of all the below functions from == 0 to == 1 back to == 0
      if(multiple_steps[0] == 0){ // removed  && steps[0] == 0
        multiple_steps[0] = 1;
//        transition = 1;
      }
//      else{
//        transition = 1;
//      }
      break;
    }
    if(step_number == 2 && going_in2 == 0){
      Serial.println("on step 1 before off step 2");
      if(multiple_steps[0] == 3){ // removed   && steps[0] == 3
        multiple_steps[1] = 2;
//        transition = 2;
      }
//      else{
//        transition = 2;
//      }
      break;
    }
    if(step_number == 2 && going_in2 == 1){
      Serial.println("on step 3 before off step 2");
      if(multiple_steps[0] == 1){ //   removed && steps[0] == 1
        multiple_steps[1] = 2;
//        transition = 2;
      }
//      else{
//        transition = 2;
//      }
      break;
    }
    if(step_number == 3 && going_in3 == 0){
      Serial.println("on step 2 before off step 3");
      if(multiple_steps[0] == 0){ //  removed && steps[0] == 0
        multiple_steps[0] = 3;
//        transition = 3;
      }
//      else{
//        transition = 3;
//      }
      break;
    }  
    //for multiple person scenarios      
    if(step_number == 1 && multiple_steps[0] == 0 && going_in1 == 1){
      multiple_steps[0] = 1;
      break;
    }
    if(step_number == 2 && multiple_steps[0] == 1 && going_in2 == 1){
      multiple_steps[1] = 1;
      break;  
    }
    if(step_number == 3 && multiple_steps[1] == 2 && going_in3 == 1){
      multiple_steps[2] = 3;
      break;
    }
    if(step_number == 3 && multiple_steps[0] == 0 && going_in3 == 0){
      multiple_steps[0] = 3;
      break;  
    }
    if(step_number == 2 && multiple_steps[0] == 3 && going_in2 == 0){
      multiple_steps[1] = 2;
      break;
    }
    if(step_number == 1 && multiple_steps[1] == 2 && going_in1 == 0){
      multiple_steps[2] = 1;
      break;  
    }
  }  
}

void check_length(){
  display_steps();
  if(people_on_setup > 0){
    if(multiple_steps[0] == 1 && multiple_steps[1] == 2 && multiple_steps[2] == 3){
      count++;
//      transition = 0;
      jumped_from = 0;
//      display.clear();
//      display.showNumberDec(count, false);
      Serial.print("2nd person count: ");
      Serial.println(count);
      memset(multiple_steps, 0, sizeof(multiple_steps));
    }
    if(multiple_steps[0] == 3 && multiple_steps[1] == 2 && multiple_steps[2] == 1){
      count--;
//      transition = 0;
      jumped_from = 0;
//      display.clear();
//      display.showNumberDec(count, false);
      Serial.print("2nd person count: ");
      Serial.println(count);
      memset(multiple_steps, 0, sizeof(multiple_steps));
    }  
  }
  else{
    if(steps[0] == 1 && steps[1] == 2 && steps[2] == 3){
      count++;
      transition = 0;
      jumped_from = 0;
  //    display.clear();
  //    display.showNumberDec(count, false);
      Serial.println(count);
      memset(steps, 0, sizeof(steps));
    }
    if(steps[0] == 3 && steps[1] == 2 && steps[2] == 1){
      count--;
      transition = 0;
      jumped_from = 0;
  //    display.clear();
  //    display.showNumberDec(count, false);
      Serial.println(count);
      memset(steps, 0, sizeof(steps));
    }
  }
}

void display_steps(){
  if(people_on_setup == 0){
    Serial.print("steps: ");
    for(int i = 0; i < 3 ; i++){
      Serial.print(steps[i]);  
    }
    Serial.println();
  }
  if(people_on_setup > 0){
    Serial.print("multiple_steps: ");
    for(int i = 0; i < 3 ; i++){
      Serial.print(multiple_steps[i]);  
    }
    Serial.println();
  }  
}

void process_step_arrays(int step_number){
  for(int i = 0; i < 3; i++){
    if(array1_active){
      int ret_value = scenarios(step_array1, step_number);
      if(ret_value != 0){
        for(int i = 0; i < 3; i++){
          if(step_array1[i] == 0); step_array1[i] = ret_value;
          check_array(1);
          break;
        }
      }
      else{
        if(!array2_active){
          array2_active = true;
          continue;
        }
      }
    }
    if(array2_active){}
    else{
      if((step_number == 1 && going_in1 == 1)|| (step_number == 3 && going_in3 == 0)){
      }
    }
  }  
}

int scenarios(int steps[], int step_number){
  if(step_number == 2 && going_in2 == 1 && steps[0] == 1);return 2;
  if(step_number == 3 && going_in3 == 1 && steps[1] == 2);return 3;
  
  if(step_number == 2 && going_in2 == 0 && steps[0] == 3);return 2;
  if(step_number == 1 && going_in1 == 0 && steps[1] == 2);return 1;
  
  return 0;
}
