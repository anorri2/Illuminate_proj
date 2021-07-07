#ifndef NODECONFIGS_H
#define NODECONFIGS_H

#include "FixtureFunctions.h"
extern bool hasUltrasonic;
extern bool hasTouch;
extern CRGB leds[];

//Fixture fixtures[10];
Fixture* fix0;
Fixture* fix1;
Fixture* fix2;

void configNode(int node) {

  switch (node) {
    case 0: { //ultrasonic
        Serial.println("Running Setup for Ultrasonic");
        hasUltrasonic = true;
        hasTouch = true;
        break;
      }
    case 1: { //high power
        Serial.println("Running Setup for High power");
        hasUltrasonic = false;
        hasTouch = true;
        fix0 = new WS2811_Fixture(0, 15, 27, leds);
        fix0->setPatternCallback(&add_test_10);
        fix0->setPatternCallback(&add_test_15, 1);
        break;
      }

    case 4: { // nodemcuFixture
        Serial.println("Running Setup for NODEMCU");
        //initFileSystem();
        
        hasUltrasonic = false;
        hasTouch = false;
        // fixtures=new Fixture[10];
        fix0 = new WS2811_Fixture(0, 15, D2, leds);
        fix0->setPatternCallback(&spiffs_test1, 0);
        fix0->setPatternCallback(&spiffs_test2, 1);
       //fix0->setPatternCallback(&grad_test);
        // fixtures[0]=fix;
        // fixtures[0]=first_fixture;
        break;
      }
  }
}

void nodeLoop(int node) {

  switch (node) {
    case 0: { //ultrasonic
 
        break;
      }
    case 1: { //high power
        break;
      }
    case 4: { // nodemcu
        fix0->update();
        break;
      }

  }
}



#endif
