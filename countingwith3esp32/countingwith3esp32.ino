#define mat1 36
#define mat2 39
#define mat3 34

int person = 4;
int threshold1, threshold2, threshold3;
int step1, step2 , step3;
int inStep = 0, midStep = 0, outStep = 0;
bool s1, s2, s3;
int count = 0;
long starttime, stoptime;
void setup() {
  // put your setup code here, to run once:
  pinMode(mat1, INPUT);
  pinMode(mat2, INPUT);
  pinMode(mat3, INPUT);
  threshold1 = analogRead(mat1) + person;
  threshold2 = analogRead(mat2) + person; 
  threshold3 = analogRead(mat3) + person;
  starttime = millis();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  step1 = analogRead(mat1);
  step2 = analogRead(mat2);
  step3 = analogRead(mat3);
  if(step1 > threshold1){
    delay(500);
    Serial.print("at step1: ");
    Serial.print(step1);
    Serial.print("  threshold1: ");
    Serial.println(threshold1); 
    // inStep += 1;
    s1 = true;
    threshold1 = step1 + person;
    starttime = millis();
    count_people(true);
  }
  if(step2 > threshold2){
    delay(500);
    Serial.print("at step2: ");
    Serial.print(step2);
    Serial.print("  threshold2: ");
    Serial.println(threshold2);
    // midStep += 1;
    s2 = true;
    threshold2 = step2 + person;
    starttime = millis();
  }
  if(step3 > threshold3){
    delay(500);
    Serial.print("at step3: ");
    Serial.print(step3);
    Serial.print("  threshold3: ");
    Serial.println(threshold3);
    //outStep += 1;
    s3 = true;
    threshold3 = step3 + person;
    starttime = millis();
    count_people(false);
  }
  stoptime = millis();
  timout_refresh();
}

void count_people(bool trueforoutcount){
  if(s1 && s2 && s3){
    if(trueforoutcount){ 
      count -= 1;
    }
    else{
      count += 1;
    }
    s1 = false;
    s2 = false;
    s3 = false;
    threshold1 = step1 + person;
    threshold2 = step2 + person;
    threshold3 = step3 + person;
    Serial.print("count: ");
    Serial.println(count);
  }
}

void timout_refresh(){
  if((stoptime - starttime) > 15000){
    Serial.println("timeout, refreshing values");
//    inStep = 0;
//    midStep = 0;
//    outStep = 0;
    starttime = millis();
    s1 = false;
    s2 = false;
    s3 = false;
    threshold1 = step1 + person;
    threshold2 = step2 + person;
    threshold3 = step3 + person;
  }
}
