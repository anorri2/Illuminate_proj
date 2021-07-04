#ifndef FIXTURE_FUNCTIONS_H
#define FIXTURE_FUNCTIONS_H

#include "Fixture.h"
#include "GradientEffect.h"
#include "Transition.h"


GradientEffect gradient;
Transition transition;

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
  vTaskDelay(200 / portTICK_PERIOD_MS);

  for (int i = fixx->getStartLed(); i <= fixx->getEndLed(); i++) {
    ledss[i] = CRGB(255, 255, 0);
  }
  FastLED.show();
  vTaskDelay(200 / portTICK_PERIOD_MS);
  //update stuff

}

void add_test_15(Fixture* fix) {
  WS2811_Fixture* fixx = (WS2811_Fixture*) fix;
  CRGB* ledss = fixx->getLeds();
  for (int i = fixx->getStartLed(); i <= fixx->getEndLed(); i++) {
    ledss[i] = CRGB(0, 0, 0);
  }
  FastLED.show();
  vTaskDelay(200 / portTICK_PERIOD_MS);

  for (int i = fixx->getStartLed(); i <= fixx->getEndLed(); i++) {
    ledss[i] = CRGB(255, 255, 0);
  }
  FastLED.show();
  vTaskDelay(200 / portTICK_PERIOD_MS);
  //update stuff

}

void single_test_0(Fixture* fix) {
  int rcol = 0;
  Single_Fixture* fixx = (Single_Fixture*) fix;
  for (int i = 0; i < 255; i++) {
    ledcWrite(fixx->getRedChannel(), 255);
    ledcWrite(fixx->getGreenChannel(), i);
    ledcWrite(fixx->getBlueChannel(), 255);
    vTaskDelay(2 / portTICK_PERIOD_MS);
  }
  for (int i = 0; i < 255; i++) {
    ledcWrite(fixx->getRedChannel(), i);
    ledcWrite(fixx->getGreenChannel(), 255);
    ledcWrite(fixx->getBlueChannel(), 255);
    vTaskDelay(2 / portTICK_PERIOD_MS);
  }

}



#endif
