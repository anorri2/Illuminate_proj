#include "GradientEffect.h"


extern void writeFile(fs::FS &fs, const char * path, const char * message);


void GradientEffect::getDataFromFile(fs::FS &fs, String filename) {

  File file = fs.open(filename.c_str());
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading:" + filename);
  }
  DynamicJsonDocument doc(2000);
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    Serial.println(filename);
    return;
  }
  JsonObject obj = doc.as<JsonObject>();

  uint8_t ref = obj["ref"];
  uint8_t num_leds = obj["numL"];
  uint8_t* seq;
  seq = new uint8_t[num_leds];
  uint8_t num_cols = doc["numC"];
  uint8_t* grad_bytes;
  grad_bytes = new uint8_t[num_cols * 4];
  uint8_t spacing = doc["spac"];

  JsonArray seq_arr = obj["seq"].as<JsonArray>();
  for (int i = 0; i < num_leds; i++) {
    seq[i] = (uint8_t)seq_arr[i];
  }

  JsonArray pal_arr = obj["pal"].as<JsonArray>();
  for (int i = 0; i < 4 * num_cols; i++) {
    grad_bytes[i] = (uint8_t)pal_arr[i];
  }


  _numLeds = num_leds;
  for (int i = 0; i < _numLeds; i++) {
    _ledSequence[i] = seq[i];
  }
  _num_colours = num_cols;
  pal.loadDynamicGradientPalette(grad_bytes);
  for (int i = 0; i < 4 * _num_colours; i++) {
    _gradbytes[(i * 4) + 0] = grad_bytes[(i * 4) + 0];
  }

  _spacing = spacing;
  delete[] grad_bytes;
  delete[] seq;
}
void GradientEffect::getDataFromClient(WiFiClient client) {

  _duration = client.read();         // First byte shows
  Serial.print("duration = " );
  Serial.println(_duration);
  _numLeds = client.read();
  Serial.print("LED sequence :- ");
  for (int i = 0; i < _numLeds; i++) {
    _ledSequence[i] = client.read();
  }
  _num_colours = client.read();
  byte alpha[16];
  byte red[16];
  byte green[16];
  byte blue[16];
  byte gradbytes[4 * _num_colours];
  for (int i = 0; i < _num_colours; i++) {
    byte data;
    data = client.read();
    alpha[i] = data;

    data = client.read();
    red[i] = data;

    data = client.read();
    green[i] = data;

    data = client.read();
    blue[i] = data;
  }
  _spacing = client.read();

  //Build the gradient
  for (int i = 0; i < _num_colours; i++) {
    gradbytes[4 * i] = (255 * i) / (_num_colours - 1);
    gradbytes[1 + (4 * i)] = red[i];
    gradbytes[2 + (4 * i)] = green[i];
    gradbytes[3 + (4 * i)] = blue[i];
//    Serial.printf("\r\n%d,%d,%d,%d,\r\n", gradbytes[4 * i], gradbytes[1 + (4 * i)], gradbytes[2 + (4 * i)], gradbytes[3 + (4 * i)]);
  }

  for (int i = 0; i < 4 * _num_colours; i++) {
    _gradbytes[i] = gradbytes[i];
  }
  pal.loadDynamicGradientPalette(gradbytes);
  Serial.println("Gradient received from client");
}


void GradientEffect::show(CRGB* leds, int duration, bool dir) {
  float del = (duration * 1000 / 255);
  int d = (int)del;

  if (dir == FORWARD) {
    for (int s = 0; s <= 255; s++) {
      for (int i = 0; i < _numLeds; i++) {
        leds[remapper.remapped[_ledSequence[i]]] = ColorFromPalette(pal, ((_spacing * i) + s) % 256, 255, LINEARBLEND);
      }
      FastLED.delay(d);
      FastLED.show();
    }
  }
  else {
    for (int s = 255; s >= 0; s--) {
      for (int i = 0; i < _numLeds; i++) {
        leds[remapper.remapped[_ledSequence[i]]] = ColorFromPalette(pal, ((_spacing * i) + s) % 256, 255, LINEARBLEND);
      }
      FastLED.delay(d);
      FastLED.show();
    }
  }
}


void GradientEffect::updateToPosition(CRGB* leds, uint8_t position) {
  for (int i = 0; i < _numLeds; i++) {
    if (!sfxEn) {
      leds[remapper.remapped[_ledSequence[i]]] = ColorFromPalette(pal, ((_spacing * i) + position) % 256, 255, LINEARBLEND);
    }
    else {
      bool assigned = false;
      int count_to = (_numLeds > sound_fx.numLeds) ? sound_fx.numLeds : _numLeds;
      for (int q = 0; q < count_to; q++) {
        if (_ledSequence[i] == sound_fx.ledSequence[q]) {
          assigned = true;
        }
      }
      if (!assigned) {
        leds[remapper.remapped[_ledSequence[i]]] = ColorFromPalette(pal, ((_spacing * i) + position) % 256, 255, LINEARBLEND);
      }
    }
  }
}


int GradientEffect::getSpacing() {
  return _spacing;
}
void GradientEffect::setSpacing(int spac) {
  _spacing = spac;
}
int GradientEffect::getDuration() {
  return _duration;
}
void GradientEffect::setDuration(int dur) {
  _duration = dur;
}

String GradientEffect::formatToJSONString() {
  String txt = "{""ref"":";
  txt += "0,""numL"":";
  txt += String(_numLeds);
  txt += ",""seq"":[";
  for (int i = 0; i < _numLeds; i++) {
    txt += (String)_ledSequence[i];
    if (i < (_numLeds - 1)) {
      txt += ",";
    }
  }
  txt += "],""numC"":";
  txt += (String)_num_colours;
  txt += ",""pal"":[";
  for (int i = 0; i < 4 * _num_colours; i++) {
    txt += (String)_gradbytes[i];
    if (i < ((4 * _num_colours) - 1)) {
      txt += ",";
    }
  }
  txt += "],""spac"":";
  txt += (String)_spacing;
  txt += "}";
  Serial.println(txt);

  return txt;
}

bool GradientEffect::saveToFile(String filename, String directory) {
  String str = formatToJSONString();
  String fname = directory + filename + ".json";
  writeFile(LITTLEFS, fname.c_str(), str.c_str());
  return true;
}
