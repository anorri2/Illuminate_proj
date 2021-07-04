#ifndef GradientEffect_h
#define GradientEffect_h

#include <Arduino.h>
#include <WiFi.h>
#include "ArduinoJson.h"
#include "LITTLEFS.h"
//#include "SharedData.h"
//#include "main.h"
//#include "SoundFX.h"
//#include "EffectFileSystem.h"


extern SoundFX sound_fx;
extern bool sfxEn;
extern bool ambientFXEn;

class GradientEffect {

  private:
    uint8_t _numLeds=0;
    byte _ledSequence[200];
    uint8_t _num_colours=0;
    CRGBPalette16 pal;
    int _spacing=0;
    byte _duration = 10;
    byte _gradbytes[16 * 4];


  public:
    enum {FORWARD, BACKWARD};


    GradientEffect() {
    }

    GradientEffect(WiFiClient client) {    // Build a gradient from client data
      getDataFromClient(client);
    }

    GradientEffect(String filename) {    // Build a gradient from client data
      getDataFromFile(LITTLEFS, filename);
    }

    GradientEffect(uint8_t num_leds, byte sequence[20], int numofcolours, byte* gradbytes, int spacing) { //Build a gradient from a byte array
      _numLeds = num_leds;
      for (int i = 0; i < num_leds; i++) {
        _ledSequence[i] = sequence[i];
      }
      _num_colours = numofcolours;
      _spacing = spacing;
      for (int i = 0; i < 4 * numofcolours; i++) {
        _gradbytes[i] = gradbytes[i];
      }
      pal.loadDynamicGradientPalette(gradbytes);
    }

    void getArrayAtPosition(CRGB* arr, int pos) {

      for (int i = 0; i < _numLeds; i++) {
        arr[_ledSequence[i]] = ColorFromPalette(pal, ((_spacing * i) + pos) % 256, 255, LINEARBLEND);
      }
    }
    void getDataFromClient(WiFiClient client);
    void show(CRGB* leds, int duration, bool dir);
    void updateToPosition(CRGB* leds, uint8_t position);
    int getSpacing();
    void setSpacing(int spac);
    int getDuration();
    void setDuration(int dur);
    void getDataFromFile(fs::FS &fs, String filename);
    bool saveToFile(String filename, String directory);
    String formatToJSONString();

};


#endif
