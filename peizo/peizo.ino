#define inMat A0
#define outMat A5
int inValue = 0;
int outValue = 0;
int count = 0;
long starttime, stoptime;
int threshold = 900;
bool inFlag = LOW, outFlag = LOW;
void setup() {
  // put your setup code here, to run once:
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(2), in_mat, RISING);
  attachInterrupt(digitalPinToInterrupt(3), out_mat, RISING);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  inValue = analogRead(inMat);
  outValue = analogRead(outMat);
  Serial.print("inValue: ");
  Serial.print(inValue);
  Serial.print("      outValue: ");
  Serial.print(outValue);
  if(inValue > threshold && outValue < threshold){
        delay(500);
        digitalWrite(7, HIGH);
        digitalWrite(7, LOW);
  }
  if(outValue > threshold && inValue < threshold){
        delay(500);
        digitalWrite(8, HIGH);
        digitalWrite(8, LOW);
  }
  if(inFlag && outFlag){
    if((stoptime - starttime) > 0){
        count++;
      }
    if((stoptime - starttime) < 0){
      count--;
    }
    inFlag = LOW;
    outFlag = LOW;
  }
  Serial.print("       count: ");
  Serial.println(count);
}

void in_mat(){
    starttime = millis();
    inFlag = HIGH;
  }
  
void out_mat(){
    stoptime = millis();
    outFlag = HIGH;
  }
