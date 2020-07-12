#define inMat A0
#define outMat A2

int processing_delay = 250;
int units_per_person = 20;
int inValue = 0, outValue = 0;
int count = 0;
long start_time, stop_time;
int threshold_inMat, threshold_outMat;
int inFlag = 0, outFlag = 0;
int steps[2] = {0, 0};
int transition = 0;

void setup() {
  Serial.begin(9600);
  threshold_inMat = calibrate_inMat() + units_per_person;
  Serial.print("threshold_inMat: ");
  Serial.println(threshold_inMat);
  threshold_outMat = calibrate_outMat() + units_per_person;
  Serial.print("threshold_outMat: ");
  Serial.println(threshold_outMat);
  start_time = millis();
}


void loop() {
  inValue = analogRead(inMat);
  outValue = analogRead(outMat);

  on_step(1, inValue, &threshold_inMat, &inFlag);
  transition_step();
  off_step(1, inValue, &threshold_inMat, &inFlag);
  
  on_step(2, outValue, &threshold_outMat, &outFlag);
  transition_step();
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

void transition_step(){
  int half_in_thresh = threshold_inMat - (units_per_person / 2);
  int half_out_thresh = threshold_outMat - (units_per_person / 2);
  if(inFlag > 0){
    half_in_thresh = threshold_inMat - ((3 * units_per_person) / 2);
    if(inValue < half_in_thresh && outValue > half_out_thresh){
      Serial.println("transition from 1 to 2");
      Serial.print("inFlag: ");
      Serial.println(inFlag);
      Serial.print("half_in_thresh: ");
      Serial.println(half_in_thresh);
      Serial.print("half_out_thresh: ");
      Serial.println(half_out_thresh);
      Serial.print("inValue: ");
      Serial.println(inValue);
      transition = 1;
    }
  }
  
  if(outFlag > 0){
    half_out_thresh = threshold_outMat - ((3 * units_per_person) / 2);
    if(outValue < half_out_thresh && inValue > half_in_thresh){
      Serial.println("transition from 2 to 1");
      Serial.print("outFlag: ");
      Serial.println(outFlag);
      Serial.print("half_out_thresh: ");
      Serial.println(half_out_thresh);
      Serial.print("half_in_thresh: ");
      Serial.println(half_in_thresh);
      Serial.print("outValue: ");
      Serial.println(outValue);
      transition = 2;
    }
  }
}

void on_step(int number, int step_value, int *threshold, int *flag){
  if(step_value > *threshold){
    *flag += 1;
    *threshold = step_value + units_per_person;
    Serial.print("at step ");
    Serial.print(number);
    Serial.print("    threshold: ");
    Serial.println(*threshold);
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
  }
}

void process_step(int number){
  for(int i = 0;i < 2; i++){
    if(number == 1 && steps[0] == 0 && transition == 1){
      Serial.println("                                      from step 1");
      steps[0] = number;
      break;
    }
    if(number == 2 && steps[0] == 1 && transition == 1){
      Serial.println("                                      from step 2 after 1, counting in");
      steps[1] = number;
      break;  
    }
    if(number == 2 && steps[0] == 0 && transition == 2){
      Serial.println("                                      from step 2");
      steps[0] = number;
      break;
    }
    if(number == 1 && steps[0] == 2 && transition == 2){
      Serial.println("                                      from step 1 after 2, counting out");
      steps[1] = number;
      break;  
    }
  }
}

void check_length(){
  if(steps[0] == 1 && steps[1] == 2){
    count++;
    transition = 0;
    memset(steps, 0, sizeof(steps));
    Serial.print("                                          count: ");
    Serial.println(count);
  }
  if(steps[0] == 2 && steps[1] == 1){
    count--;
    transition = 0;
    memset(steps, 0, sizeof(steps));
    Serial.print("                                          count: ");
    Serial.println(count);
  }
}

void timeout_refresh(){
  if((stop_time - start_time) >= 20000){
    Serial.println("timeout, resetting values");
    start_time = millis();
    memset(steps, 0, sizeof(steps));
    inFlag = 0;
    outFlag = 0;
    transition = 0;
    threshold_inMat = calibrate_inMat() + units_per_person;
    Serial.print("threshold_inMat: ");
    Serial.println(threshold_inMat);
    threshold_outMat = calibrate_outMat() + units_per_person;
    Serial.print("threshold_outMat: ");
    Serial.println(threshold_outMat);
  }  
}
