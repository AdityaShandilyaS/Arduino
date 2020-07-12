#define mat0 A4

float value = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(mat0, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  value = analogRead(mat0);
  delay(250);
  Serial.print("voltage drop black to red: ");
  Serial.println(value);
}
