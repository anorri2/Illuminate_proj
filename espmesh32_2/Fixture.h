#ifndef FIXTURE_H
#define FIXTURE_H

//#define NODEMCU_DATA_PIN_4  D4
//#define NODEMCU_DATA_PIN_2  D2
//#define NODEMCU_DATA_PIN_12  D1
#define ESP_32_DATA_PIN_27 27


extern CRGB leds[];

class Fixture {
  public:
    typedef enum fixture_type_t {FIXTURE_TYPE_WS2811, FIXTURE_TYPE_SINGLE} fixture_type_t;
    virtual void update();

    void (*pattern0)(Fixture* fix) = NULL;
    void (*pattern1)(Fixture* fix) = NULL;
    void (*pattern2)(Fixture* fix) = NULL;
    void (*pattern3)(Fixture* fix) = NULL;
    void (*pattern4)(Fixture* fix) = NULL;

    void setPatternCallback(void (*callBack)(Fixture* fix), int pattern_num = 0) {
      _num_patterns++;
      switch (pattern_num) {
        case 0: {
            pattern0 = callBack;
            break;
          }
        case 1: {
            pattern1 = callBack;
            break;
          }
        case 2: {
            pattern2 = callBack;
            break;
          }
        case 3: {
            pattern3 = callBack;
            break;
          }
        case 4: {
            pattern4 = callBack;
            break;
          }
        default: {
            Serial.println("ERROR INVALID CALLBACK NUM");
            while (1);
          }
      }
    }
  protected:
    int _num_patterns = 0;
};

#include <FastLED.h>

class WS2811_Fixture : public Fixture {
  private:
    int _start_led = 0;
    int _end_led = 0;
    int _num_leds = 0;
    int _data_pin = 0;
    CRGB* _leds;


  public:
    WS2811_Fixture() {

    }
    void operator=(WS2811_Fixture input) { // compound assignment (does not need to be a member,
      _start_led = input._start_led;
      _num_leds = input._num_leds;
      _end_led = input._end_led;
      _data_pin = input._data_pin;
      _leds = input._leds;
      return;
    }

    WS2811_Fixture(int startled, int numleds, int datapin, CRGB* ledarr) {
      _start_led = startled;
      _num_leds = numleds;
      _end_led = _start_led + _num_leds;
      _data_pin = datapin;
      _leds = ledarr;

      Serial.printf("Attatching %d leds to pin %d \r\n", _num_leds, _data_pin);
      switch (_data_pin) {
        case  27: {
            FastLED.addLeds<WS2812B, ESP_32_DATA_PIN_27>(_leds, _num_leds);
            break;
          }

        default : {
            Serial.println("ERROR UNKNOWN PIN SELECTED");
            while (1);
          }
      }
    }

    void update() { //virtual function from Fixture

      if (pattern0 != NULL) {
        pattern0(this);
      }
      if (pattern1 != NULL) {
        pattern1(this);
      }
      if (pattern2 != NULL) {
        pattern2(this);
      }
      if (pattern3 != NULL) {
        pattern3(this);
      }
      if (pattern4 != NULL) {
        pattern4(this);
      }
    }

    int getNumLeds() {
      return _num_leds;
    }
    int getStartLed() {
      return _start_led;
    }
    int getEndLed() {
      return _end_led;
    }

    CRGB* getLeds() {
      return _leds;
    }

};

class Single_Fixture : public Fixture {
  private:
    int _r_pin;
    int _g_pin;
    int _b_pin;
    int _r_chan, _g_chan, _b_chan;

  public:

    void update() { //virtual function from Fixture
      if (pattern0 != NULL) {
        pattern0(this);
      }
      if (pattern1 != NULL) {
        pattern1(this);
      }
      if (pattern2 != NULL) {
        pattern2(this);
      }
      if (pattern3 != NULL) {
        pattern3(this);
      }
      if (pattern4 != NULL) {
        pattern4(this);
      }
    }

    Single_Fixture(int rpin, int rchan, int gpin, int gchan, int bpin, int bchan, int pos = 0) {
      _r_pin = rpin;
      _g_pin = rpin;
      _b_pin = rpin;
      _r_chan = rchan;
      _g_chan = gchan;
      _b_chan = bchan;
      ledcSetup(_r_chan, 2000, 8);
      ledcSetup(_g_chan, 2000, 8);
      ledcSetup(_b_chan, 2000, 8);
      ledcAttachPin(rpin, _r_chan);
      ledcAttachPin(gpin, _g_chan);
      ledcAttachPin(bpin, _b_chan);
    }

    int getRedPin() {
      return _r_pin;
    }
    int getRedChannel() {
      return _r_chan;
    }
    int getGreenPin() {
      return _g_pin;
    }
    int getGreenChannel() {
      return _g_chan;
    }
    int getBluePin() {
      return _b_pin;
    }
    int getBlueChannel() {
      return _b_chan;
    }
};


#endif
