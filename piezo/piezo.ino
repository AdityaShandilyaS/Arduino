/*
  ReadAnalogVoltage

  Reads an analog input on pin 0, converts it to voltage, and prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/ReadAnalogVoltage
*/
int count = 0;
long startTime;
long stopTime;
long waitTime = 2000;
int treshold = 450;
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  Serial.println("Program Start");
  int outMat = analogRead(A0);
  Serial.println(outMat);
  int inMat = analogRead(A5);
  Serial.println(inMat);
  delay(500);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  // float voltage = sensorValue * (5.0 / 1023.0);
  // print out the value you read:
  in_count(outMat, inMat);
  out_count(inMat, outMat);
  delay(1000);
}


void in_count(int value0, int value1){
  Serial.println("inside in_count");
  if(value0 > treshold && value1 <= treshold){
      Serial.println("half in");
      startTime = millis();
      stopTime = millis();
      while((stopTime - startTime) > waitTime){
        if(value1 > treshold && value0 <= treshold){
          count++;
          Serial.print("count: ");
          Serial.println(count);
          break;
        }
        stopTime = millis();
      }
    }
}


void out_count(int value1, int value0){
  Serial.println("inside out_count");
  if(value1 > treshold && value0 <= treshold){
      Serial.println("half out");
      startTime = millis();
      stopTime = millis();
      while((stopTime - startTime) > waitTime){
        if(value0 > treshold && value1 <= treshold){
          count--;
          Serial.print("count: ");
          Serial.println(count);
          break;
        }
        stopTime = millis();
      }
    }
}
