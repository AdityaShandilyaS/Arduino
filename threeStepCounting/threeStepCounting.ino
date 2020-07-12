#define inMat A0
#define outMat A5
#define midMat A3
int inValue = 0;
int midValue = 0;
int outValue = 0;
int count = 0;
long starttime, stoptime, counttime;
int threshold = 900;
bool inFlag = LOW, outFlag = LOW, midFlag = LOW;
void setup() {
  // put your setup code here, to run once:
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  inValue = analogRead(inMat);
  midValue = analogRead(midMat);
  outValue = analogRead(outMat);
  Serial.print("inValue: ");
  Serial.print(inValue);
  Serial.print("   midValue: ");
  Serial.print(midValue);
  Serial.print("      outValue: ");
  Serial.print(outValue);
  if(inValue < threshold && outValue > threshold && midValue > threshold){
        delay(500);
        in_mat();
        counttime = millis();
  }
  if(midValue < threshold && inValue > threshold && outValue > threshold){
        delay(500);
        mid_mat();    
  }
  if(outValue < threshold && inValue > threshold && midValue > threshold){
        delay(500);
        out_mat();
  }
  if(millis() - counttime >= 60000){
      inFlag = LOW;
      midFlag = LOW;
      outFlag = LOW;
    }
  if(inFlag && midFlag && outFlag){
    if((stoptime - starttime) > 0){
        count++;
      }
    if((stoptime - starttime) < 0){
      count--;
    }
    inFlag = LOW;
    midFlag = LOW;
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

void mid_mat(){
    midFlag = HIGH;
  }
