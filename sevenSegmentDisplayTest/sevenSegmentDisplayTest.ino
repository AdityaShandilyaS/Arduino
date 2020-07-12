#include <Arduino.h>
#include <TM1637Display.h>

#define CLK 2
#define DIO 3


const uint8_t SEG_HI[] = {
  SEG_C | SEG_G | SEG_E | SEG_F,           // h
  SEG_E                                   // i
  };

const uint8_t SEG_strt[] = {
  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,   // S
  SEG_E | SEG_F | SEG_G | SEG_D,           // t
  SEG_E | SEG_G,                           //r
  SEG_E | SEG_F | SEG_G | SEG_D           // t
  };

TM1637Display display(CLK, DIO);

void setup() {
  display.setBrightness(7, true);
  display.clear();
  display.setSegments(SEG_HI);
  delay(5000);
}

void loop() {
  display.setSegments(SEG_strt);
  delay(1000);
  for(int i = 0 ; i > -11; i--){
    display.clear();
    display.showNumberDec(i, false);
    delay(1000);  
  }
}
