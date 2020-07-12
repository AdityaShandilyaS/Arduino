#define mat1 A0
#define mat2 A2
#define mat3 A4

int units_per_person = 8;
int threshold_mat1, threshold_mat2, threshold_mat3;
int temp_threshold1, temp_threshold2, temp_threshold3;
int value_mat1, value_mat2 , value_mat3;
int on_mat1 = 0, on_mat2 = 0, on_mat3 = 0;
int inStep = 0, midStep = 0, outStep = 0;
int count = 0;
int total = 0;
int processing_delay = 250;
long starttime, stoptime;

void setup() {
  // put your setup code here, to run once:
  threshold_mat1 = analogRead(mat1) + units_per_person;
  threshold_mat2 = analogRead(mat2) + units_per_person;
  threshold_mat3 = analogRead(mat3) + units_per_person;
  starttime = millis();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  value_mat1 = analogRead(mat1);
  value_mat2 = analogRead(mat2);
  value_mat3 = analogRead(mat3);

  check_mat(value_mat1, threshold_mat1, on_mat1, temp_threshold1, inStep, 1);
  Serial.println(threshold_mat1);
  check_mat(value_mat2, threshold_mat2, on_mat2, temp_threshold2, midStep, 2);

  check_mat(value_mat3, threshold_mat3, on_mat3, temp_threshold3, outStep, 3);
  
  stoptime = millis();
  timeout_refresh();
}


void check_mat(int mat, int threshold, int people_on_mat, int temporary_threshold, int step_count, int multiplier){
  if(mat > threshold){
    delay(processing_delay);
    Serial.print("at step ");
    Serial.print(multiplier);
    Serial.print(": ");
    Serial.print(mat);
    Serial.print("  threshold: ");
    Serial.println(threshold); 
    people_on_mat += 1;
    threshold = mat + units_per_person;
    Serial.println(threshold);
    starttime = millis();
  }
  temporary_threshold = threshold - (2 * units_per_person);
  
  if(mat < temporary_threshold && people_on_mat > 0){
    step_count += multiplier;
    threshold = mat + units_per_person;
    people_on_mat -= 1;
    if(multiplier == 1){
      count_people(true);  
    }
    if(multiplier == 3){
      count_people(false);  
    }
  }
}

void count_people(bool trueForOutCount){
  int precount = 0;
  total = midStep + inStep + outStep;
  precount = total/6;
//  Serial.print("precount: ");
//  Serial.println(precount);
  if(precount > 0){
    if(trueForOutCount){ 
      count += -precount;
    }
    else{
      count += precount;
    }
    total = 0;
    inStep -= precount;
    midStep -= precount * 2;
    outStep -= precount * 3;
    threshold_mat1 = value_mat1 + units_per_person;
    threshold_mat2 = value_mat2 + units_per_person;
    threshold_mat3 = value_mat3 + units_per_person;
    Serial.print("count: ");
    Serial.println(count);
  }
}

void timeout_refresh(){
  if((stoptime - starttime) > 15000){
    Serial.println("timeout, refreshing values");
    inStep = 0;
    midStep = 0;
    outStep = 0;
    on_mat1 = 0;
    on_mat2 = 0;
    on_mat3 = 0;
    starttime = millis();
    threshold_mat1 = value_mat1 + units_per_person;
    threshold_mat2 = value_mat2 + units_per_person;
    threshold_mat3 = value_mat3 + units_per_person;
  }
}
