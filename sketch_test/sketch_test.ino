class Test{
  public:
  bool exist = true;
  int steps[3] = {0};
  Test(int number){
    steps[0] = number;  
  }  
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
//  Test t1(1);
  Serial.println(t1.exist);
}

void loop() {
  // put your main code here, to run repeatedly:
}
