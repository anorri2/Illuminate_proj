#ifndef NODECONFIGS_H
#define NODECONFIGS_H

#include "FixtureFunctions.h"
#include <WiFiUdp.h>


extern bool hasUltrasonic;
extern bool hasTouch;

//Fixture fixtures[10];
Fixture* fix0;
Fixture* fix1;
Fixture* fix2;

extern WiFiUDP udp;

void configNode(int node) {

  switch (node) {
    case 0: { //ultrasonic
        hasUltrasonic = true;
        hasTouch = true;
        fix0 = new Single_Fixture(21, 0, 22, 1, 23, 2, 0);
        fix0->setPatternCallback(&single_grad, 0);

        break;
      }
    case 1: { //high power
        hasUltrasonic = false;
        hasTouch = false;
        // fixtures=new Fixture[10];
        fix0 = new WS2811_Fixture(0, 15, 27, leds);
        fix0->setPatternCallback(&grad_test);
        fix0->setPatternCallback(&grad_test);

        fix1 = new Single_Fixture(21, 0, 22, 1, 23, 2, 0);
        fix1->setPatternCallback(&single_test_0, 0);
        // fix1->setPatternCallback(&single_test_0,1);
        break;
      }

    case 4: { // nodemcuFixture
        hasUltrasonic = false;
        hasTouch = false;
        // fixtures=new Fixture[10];
        fix0 = new WS2811_Fixture(0, 10, 27, leds);
        fix0->setPatternCallback(&add_test_10);
        fix0->setPatternCallback(&add_test_10_2, 1);
        break;
      }
  }
}

void nodeLoop(int node) {

  switch (node) {
    case 0: { //ultrasonic
        fix0->update();
        break;
      }
    case 1: { //high power
        fix0->update();
        fix1->update();
        break;
      }
    case 4: { // nodemcu
        fix0->update();
        break;
      }

  }
}



#endif
