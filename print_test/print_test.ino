long start = 0, elapsed_time = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  start = micros();
Serial.println(start);
//  Serial.print("on step");
//  elapsed_time = micros() - start;
//  Serial.println(elapsed_time);
//  start = micros();
//  Serial.print("off step");
//  elapsed_time = micros() - start;
//  Serial.println(elapsed_time);
//  start = micros();
//  Serial.print("aditya");
//  elapsed_time = micros() - start;
//  Serial.println(elapsed_time);
  
}
