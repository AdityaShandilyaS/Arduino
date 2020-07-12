#include "WiFi.h"

const String ssid = "Akshara2.4GHz";
const String pw = "9686501250";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  connectToNetwork();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void connectToNetwork() {
  WiFi.begin(ssid, pw);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
 
  Serial.println("Connected to network");
 
}
