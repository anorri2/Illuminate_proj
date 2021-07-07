#ifndef FIXTURE_FUNCTIONS_H
#define FIXTURE_FUNCTIONS_H

#include "Fixture.h"
#include "EffectFileSystem.h"

#include "GradientEffect.h"
#include "Transition.h"

GradientEffect gradient0;

GradientEffect gradient1;
GradientEffect gradient2;
GradientEffect gradient3;
GradientEffect gradient4;
GradientEffect gradient5;
GradientEffect gradient6;
GradientEffect gradient7;

bool gotData = false;
CRGB leds[100];


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
  //  for (int i = 0; i < 255; i++) {
  //    analogWrite(fixx->getRedPin(), i);
  //    analogWrite(fixx->getGreenPin(), i);
  //    analogWrite(fixx->getBluePin(), i);
  //    delay(20);
  //  }



}

void grad_test(Fixture* fix) {
  WS2811_Fixture* fixx = (WS2811_Fixture*) fix;
  byte seq[15] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
  int numc = 3;
  byte gradb[12] = {0, 0, 0, 255, 127, 255, 255, 0, 255, 0, 255, 255};


  GradientEffect grad(fixx->getNumLeds(), seq, numc, gradb, 5);

  for (int i = 0; i < 255; i++) {
    grad.updateToPosition(fixx->getLeds(), i);
    FastLED.show();
    FastLED.delay(20);
  }

}

//void spiffs_test1 (Fixture* fix) {
//  WS2811_Fixture* fixx = (WS2811_Fixture*) fix;
//  GradientEffect grad("rainbow.json");
//
//  //gr.formatToJSONString();
//// gradient = new GradientEffect("/rainbow.json");
// // gradient->getDataFromFile(LittleFS, "rainbow.json");
//
////  if (!gotData) {
////    gradient.getDataFromFile(LittleFS, "rainbow.json");
////    gotData = true;
////  }
//
//  // gr.formatToJSONString();
//  Serial.println("RUNNING");
//  for (int i = 0; i < 255; i++) {
//    grad.updateToPosition(fixx->getLeds(), i);
//    //    gr.updateToPosition(fixx->getLeds(), i);
//    FastLED.show();
//    delay(2);
//  }
// // delete gradient;
//}



void spiffs_test1 (Fixture* fix) {
  WS2811_Fixture* fixx = (WS2811_Fixture*) fix;
  //gr.formatToJSONString();
  // gradient = new GradientEffect("/rainbow.json");
  // gradient->getDataFromFile(LittleFS, "rainbow.json");

  if (!gradient0.gotData) {
    gradient0.getDataFromFile(LittleFS, "rainbow.json");
    gradient0.formatToJSONString();
    gradient0.gotData = true;
  }

  // gr.formatToJSONString();
  Serial.println("RUNNING");
  for (int i = 0; i < 255; i++) {
    gradient0.updateToPosition(fixx->getLeds(), i);
    FastLED.show();
    delay(2);
  }
}
void spiffs_test2 (Fixture* fix) {
  WS2811_Fixture* fixx = (WS2811_Fixture*) fix;
  //gr.formatToJSONString();
  // gradient = new GradientEffect("/rainbow.json");
  // gradient->getDataFromFile(LittleFS, "rainbow.json");

  if (!gradient1.gotData) {
    gradient1.getDataFromFile(LittleFS, "rainbow2.json");
    gradient1.formatToJSONString();
    gradient1.gotData = true;
  }

  // gr.formatToJSONString();
  for (int i = 0; i < 255; i++) {
    gradient1.updateToPosition(fixx->getLeds(), i);
    //    gr.updateToPosition(fixx->getLeds(), i);
    FastLED.show();
    delay(2);
  }
  // delete gradient;
}


#endif
