#ifndef TRANSITION_H
#define TRANSITION_H

#include <FastLED.h>
//#include <LedRemapper.h>
class Transition {



  private:
    CRGB _input_array[20];
    CRGB _output_array[20];
    //LedRemapper remapper;

  public:
    Transition(CRGB input[20], CRGB output[20]) {
      for (int i = 0; i < 20; i++) {
        _input_array[i] = input[i];
        _output_array[i] = output[i];
      }
    }

    void transitionToPosition(CRGB* leds, int pos) {

      for (int i = 0; i < 20; i++) {
        int red_dif =  _output_array[i].red - _input_array[i].red;
        int green_dif =_output_array[i].green - _input_array[i].green ;
        int blue_dif = _output_array[i].blue - _input_array[i].blue;
        leds[i].red = _input_array[i].red + ((red_dif * pos) / 255);
        leds[i].green = _input_array[i].green + ((green_dif * pos) / 255);
        leds[i].blue = _input_array[i].blue + ((blue_dif * pos) / 255);
      }

    }
};


#endif
