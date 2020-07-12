#define ARRAY_SIZE 100

int units_per_person = 25;
int off_step_thresh = 12;
int p1, p2, p3;
int initial1, initial2, initial3;
long start = 0;
bool on_step1 = false;
bool on_step1_enabled = false;
bool off_step1_enabled = true;
bool off_step1 = true;

bool on_step2 = false;
bool on_step2_enabled = false;
bool off_step2_enabled = true;
bool off_step2 = true;

bool on_step3 = false;
bool on_step3_enabled = false;
bool off_step3_enabled = true;
bool off_step3 = true;

int weight1 = 0, weight2 = 0, weight3 = 0;
int count_threshold = 3;

int on1_count = 0, off1_count = 0;
int on2_count = 0, off2_count = 0;
int on3_count = 0, off3_count = 0;

int step1_array[ARRAY_SIZE] = {0};
int step2_array[ARRAY_SIZE] = {0};
int step3_array[ARRAY_SIZE] = {0};
long elapsed_time = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initial1 = analogRead(A0); //+ units_per_person;
  initial2 = analogRead(A2);
  initial3 = analogRead(A4);
}

void loop() {
  // put your main code here, to run repeatedly:
  
//  delay(0.100);
  start = micros();
  
  p1 = analogRead(A0);
  weight1 = p1 - initial1;
  read_into_array(1, weight1);

  p2 = analogRead(A2);
  weight2 = p2 - initial2;
  read_into_array(2, weight2);

  p3 = analogRead(A4);
  weight3 = p3 - initial3;
  read_into_array(3, weight3);

  elapsed_time = micros() - start;
//  Serial.print("elapsed time(us): ");
//  Serial.println(elapsed_time);
  
  if(elapsed_time < 1000){
    delay(1 - (elapsed_time/ 1000));
  }
}

void print_array(int number){
  if(number == 1){
    Serial.print("step1: ");
    for(int i = (ARRAY_SIZE - 1) ; i >= 0 ; i--){
      Serial.print(step1_array[i]);
      Serial.print(", ");
    }
    Serial.println();
  }
  if(number == 2){
    Serial.print("step2: ");
    for(int i = (ARRAY_SIZE - 1) ; i >= 0 ; i--){
      Serial.print(step2_array[i]);
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

void read_into_array(int number, int weight){
  if(number == 1){
    for(int i = (ARRAY_SIZE - 1); i > 0 ; i--){
      step1_array[i] = step1_array[i-1];
    }
    step1_array[0] = weight;
    check_upper_count(number, weight);
    check_lower_count(number, weight);
  }
  if(number == 2){
    for(int i = (ARRAY_SIZE - 1); i > 0 ; i--){
      step2_array[i] = step2_array[i-1];
    }
    step2_array[0] = weight;
    check_upper_count(number, weight);
    check_lower_count(number, weight);
  }
  if(number == 3){
    for(int i = (ARRAY_SIZE - 1); i > 0 ; i--){
      step3_array[i] = step3_array[i-1];
    }
    step3_array[0] = weight;
    check_upper_count(number, weight);
    check_lower_count(number, weight);
  }
}

void check_upper_count(int number, int weight){
  if (number == 1){
    if(weight > units_per_person && on_step1_enabled == false){
      on_step1 = true;
      off_step1 = false;
      on1_count++ ;     
    }
    else{
      on1_count = 0;
    }
    if(weight > units_per_person && on1_count > count_threshold && on_step1 == true){
      on_step1_enabled = true;
      off_step1_enabled = false;
      on1_count = 0;
      Serial.println("on step1");
      Serial.println(millis());
//      print_array(number);
    }
  }
  if (number == 2){
    if(weight > units_per_person && on_step2_enabled == false){
      on_step2 = true;
      off_step2 = false;
      on2_count++ ;     
    }
    else{
      on2_count = 0;
    }
    if(weight > units_per_person && on2_count > count_threshold && on_step2 == true){
      on_step2_enabled = true;
      off_step2_enabled = false;
      on2_count = 0;
      Serial.println("on step2");
      Serial.println(millis());
//      print_array(number);
    }
  }
  if (number == 3){
    if(weight > units_per_person && on_step3_enabled == false){
      on_step3 = true;
      off_step3 = false;
      on3_count++ ;     
    }
    else{
      on3_count = 0;
    }
    if(weight > units_per_person && on3_count > count_threshold && on_step3 == true){
      on_step3_enabled = true;
      off_step3_enabled = false;
      on3_count = 0;
      Serial.println("on step3");
      Serial.println(millis());
//      print_array(number);
    }
  }
  
}

void check_lower_count(int number, int weight){
  if(number == 1){
    if(weight < off_step_thresh && off_step1_enabled == false){
      off_step1 = true;
      on_step1 = false;
      off1_count++;
    }
    else{
      off1_count = 0;  
    }
    if(weight < off_step_thresh && off1_count > count_threshold && off_step1 == true){
      off_step1_enabled = true;
      on_step1_enabled = false;
      off1_count = 0;
      Serial.println("off step1");
      Serial.println(millis()); 
//      print_array(number);
    }
  }
  if(number == 2){
    if(weight < off_step_thresh && off_step2_enabled == false){
      off_step2 = true;
      on_step2 = false;
      off2_count++;
    }
    else{
      off2_count = 0;  
    }
    if(weight < off_step_thresh && off2_count > count_threshold && off_step2 == true){
      off_step2_enabled = true;
      on_step2_enabled = false;
      off2_count = 0;
      Serial.println("off step2");
      Serial.println(millis()); 
//      print_array(number);
    }
  }
  if(number == 3){
    if(weight < off_step_thresh && off_step3_enabled == false){
      off_step3 = true;
      on_step3 = false;
      off3_count++;
    }
    else{
      off3_count = 0;  
    }
    if(weight < off_step_thresh && off3_count > count_threshold && off_step3 == true){
      off_step3_enabled = true;
      on_step3_enabled = false;
      off3_count = 0;
      Serial.println("off step3");
      Serial.println(millis()); 
//      print_array(number);
    }
  }
   
}    
