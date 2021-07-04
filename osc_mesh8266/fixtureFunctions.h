#ifndef FIXTURE_FUNCTIONS_H
#define FIXTURE_FUNCTIONS_H

#include "Fixture.h"
#include "EffectFileSystem.h"


void add_test_10(Fixture* fix) {
  WS2811_Fixture* fixx = (WS2811_Fixture*) fix;
  CRGB* ledss = fixx->getLeds();
  for (int i = fixx->getStartLed(); i <= fixx->getEndLed(); i++) {
    ledss[i] = CRGB(0, 127, 255);
  }
  FastLED.show();
  delay(200);

  for (int i = fixx->getStartLed(); i <= fixx->getEndLed(); i++) {
    ledss[i] = CRGB(255, 0, 255);
  }
  FastLED.show();
  delay(200);
  //update stuff

}

void add_test_10_2(Fixture* fix) {
  WS2811_Fixture* fixx = (WS2811_Fixture*) fix;
  CRGB* ledss = fixx->getLeds();
  for (int i = fixx->getStartLed(); i <= fixx->getEndLed(); i++) {
    ledss[i] = CRGB(0, 0, 0);
  }
  FastLED.show();
  delay(200);

  for (int i = fixx->getStartLed(); i <= fixx->getEndLed(); i++) {
    ledss[i] = CRGB(255, 255, 0);
  }
  FastLED.show();
  delay(200);
  //update stuff

}

void add_test_15(Fixture* fix) {
  Single_Fixture* fixx = (Single_Fixture*) fix;
  CRGB* ledss = fixx->getLeds();
  for (int i = fixx->getStartLed(); i <= fixx->getEndLed(); i++) {
    ledss[i] = CRGB(0, 0, 0);
  }
  FastLED.show();
  delay(200);

  for (int i = fixx->getStartLed(); i <= fixx->getEndLed(); i++) {
    ledss[i] = CRGB(255, 255, 0);
  }
  FastLED.show();
  delay(200);
  //update stuff

}







#endif
