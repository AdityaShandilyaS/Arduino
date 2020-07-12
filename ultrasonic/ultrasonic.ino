int trigPin = 3;
int echoPin = 4;
int count = 0;
long echoTime = 0;
int defaultDistance = 60;
int line_break = defaultDistance;
void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  refresh_values();
}


void loop() {
  trigger_ustx(trigPin);
  echoTime = pulseIn(echoPin, HIGH);
  line_break = calculate_distance(echoTime);
  single_count(line_break);
  
  refresh_values();
}




void trigger_ustx(int trigger){
    analogWrite(trigPin, 255);
    analogWrite(trigPin, 153);
    delayMicroseconds(200);
  }

int calculate_distance(long duration){
    int distance;
    distance = duration * (0.034/2);
    return distance;
  }

void single_count(int distance){
    if(distance < defaultDistance){
        count++;
      }
  }

void refresh_values(){
    echoPin = LOW;
    echoTime = 0;
    line_break = defaultDistance;
  }
