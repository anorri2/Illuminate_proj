#ifndef FIXTURE_FUNCTIONS_H
#define FIXTURE_FUNCTIONS_H

#include "Fixture.h"
#include "GradientEffect.h"
#include "Transition.h"


//GradientEffect gradient;
//Transition transition;

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
  Serial.println("RUNNING SINGLE TEST");
  int rcol = 0;
  int rv, gv, bv;
  Single_Fixture* fixx = (Single_Fixture*) fix;
  for (int i = 0; i < 255; i++) {
    rv = i;
    gv = 255;
    bv = 255;
    ledcWrite(fixx->getRedChannel(), rv);
    ledcWrite(fixx->getGreenChannel(), gv);
    ledcWrite(fixx->getBlueChannel(), bv);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  for (int i = 0; i < 255; i++) {
    rv = 255;
    gv = i;
    bv = i;
    ledcWrite(fixx->getRedChannel(), rv);
    ledcWrite(fixx->getGreenChannel(), gv);
    ledcWrite(fixx->getBlueChannel(), bv);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

}


void grad_test(Fixture* fix) {
  WS2811_Fixture* fixx = (WS2811_Fixture*) fix;
  byte seq[15] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
  int numc = 3;
  byte gradb[12] = {0, 0, 0, 255, 127, 255, 255, 0, 255, 0, 255, 255};


  GradientEffect grad(fixx->getNumLeds(), seq, numc, gradb, 5);
  Serial.println("RUNNING");
  for (int i = 0; i < 255; i++) {
    grad.updateToPosition(fixx->getLeds(), i);
    FastLED.show();
    FastLED.delay(20);
  }
}

void single_grad(Fixture* fix) {
  Single_Fixture* fixx = (Single_Fixture*) fix;
  byte seq[1] = {0};
  int numc = 7;
  byte gradb[4*7] = {0,255,0,0,42,255,255,0,85,9,255,7,127,9,255,255,170,9,0,255,212,255,0,255,255,255,0,7};


  GradientEffect grad(1, seq, numc, gradb,0);
 // GradientEffect grad("/rainbow.json");
  Serial.println("RUNNING");
  CRGB temp[1];
  for (int i = 0; i < 255; i++) {

    grad.updateToPosition(temp, i);

    ledcWrite(fixx->getRedChannel(), temp[0].red);
    ledcWrite(fixx->getGreenChannel(), temp[0].green);
    ledcWrite(fixx->getBlueChannel(), temp[0].blue);
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }

}





#endif
