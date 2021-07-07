#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
//#include <WiFi.h>
#endif


#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <ArduinoOTA.h>
#include "painlessMesh.h"
#include <FastLED.h>
#include "nodeConfigs.h"
#include "Fixture.h"
#include "EffectFileSystem.h"

void checkForSerialCommands();
void checkForOSC();
void sendOSC();


//char ssid[] = "Psynet";          // your network SSID (name)
//char pass[] = "serenity@shangrila";                    // your network password

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(10, 73, 241, 2);     // remote IP of your computer
const unsigned int outPort = 7000;          // remote port to receive OSC
const unsigned int localPort = 7001;        // local port to listen for OSC packets (actually not used for sending)

OSCErrorCode error;


//DELETE ME
int otherNodeTouchVal = 0;
//


//Mesh attributes
#define   MESH_PREFIX     "Eywa"
#define   MESH_PASSWORD   "avitarrr"
#define   MESH_PORT       5555

int NODE_REF = 1;

float measureDistance();

const byte maxNumNodes = 8;
uint32_t node_ids[maxNumNodes] = {0xe24229, 0xf149f1, 0xe4e8bc, 0xe28477, 0x2d8f8f };
String node_names[maxNumNodes] = {"ultrasonic node", "high power node", "8266 node0", "8266 node1", "nodemcu node"};

typedef struct {
  uint32_t nodeId;
  String nodeName;
  uint8_t sensorVal[3];
} NODE_INFO;
NODE_INFO nodes[maxNumNodes];

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;


bool hasUltrasonic = false;
bool hasTouch = false;



//Addressable LED variables
const int numLeds = 20; // CHANGE FOR YOUR SETUP
const byte dataPin = 27;
//CRGB leds[numLeds];



//Button input
const int buttonPin = 34;


// Ultrasonic input
const int trigPin = 32;
const int echoPin = 35;
int distanceMapped = 0;


// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void ultrasonicTask(void* params);
void sendData();

Task taskSendMessage( TASK_SECOND * 0.2 , TASK_FOREVER, &sendData );

int count = 0;

void sendData() {
  String msg = "SINF:";
  msg += (String)255;
  msg += ',';
  msg += (String)255;
  msg += ',';
  msg += (String)0;
  mesh.sendBroadcast( msg );
  Serial.println("Sent message" + msg);
  count++;

}

int getNodeId(uint32_t from) {
  int id = -1;
  for (int i = 0; i < maxNumNodes; i++) {
    uint32_t fromChip = from & 0xffffff;

    if (fromChip == node_ids[i]) {
      id = i;
    }
  }
  return id;
}

void initNodes() {
  for (int i = 0; i < maxNumNodes; i++) {
    nodes[i].nodeId = node_ids[i];
    nodes[i].nodeName = node_names[i];
    nodes[i].sensorVal[0] = 0;
    nodes[i].sensorVal[1] = 0;
    nodes[i].sensorVal[2] = 0;
  }
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  int id = getNodeId(from);
  String commandString;
  byte vals[3];
  int invals[3];
  String nodeName = (id >= 0) ? node_names[id].c_str() : "Unknown Node ID";
  Serial.printf("Received from %s (%x) (%x):\r\n", nodeName.c_str(), from, from & 0xffffff);

  sscanf(msg.c_str(), "SINF:%d,%d,%d", &invals[0], &invals[1], &invals[2]);
  if (id >= 0) {
    nodes[id].sensorVal[0] = (byte)invals[0];
    nodes[id].sensorVal[1] = (byte)invals[1];
    nodes[id].sensorVal[2] = (byte)invals[2];
    otherNodeTouchVal = (byte)invals[1];

    Serial.printf("Received from %s:%x %u msg=%s  %d %d %d \r\n", nodeName.c_str(), from, from, msg.c_str(), nodes[id].sensorVal[0], nodes[id].sensorVal[1], nodes[id].sensorVal[2]);
  }

  //  Serial.printf("vals%d %d %d\r\n", commandString, nodes[id].sensorVal[0], nodes[id].sensorVal[1], nodes[id].sensorVal[2]);
  // Serial.printf("Command=%s %s %s %s\r\n", commandString, &val_strs[0], &val_strs[1], val_strs[2]);
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Connection, nodeId = %u\r\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\r\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\r\n", mesh.getNodeTime(), offset);
}

int g = 0;
void initTest()
{
  for (int i = 0 ; i < numLeds ; i++) {
    leds[i] = CRGB(127, 0, 0);
  }
  FastLED.show();
  delay(500);
  for (int i = 0 ; i < numLeds ; i++) {
    leds[i] = CRGB(0, 127, 0);
  }
  FastLED.show();
  delay(500);
  for (int i = 0 ; i < numLeds ; i++) {
    leds[i] = CRGB(0, 0, 127);
  }
  FastLED.show();
  delay(500);
  for (int i = 0 ; i < numLeds ; i++) {
    leds[i] = CRGB(0, 0, 0);
  }
  FastLED.show();
}

void touchTask(void* params) {

}


void ledTest() {

  for (int i = 0; i < 255; i++) {

    for (int l = 0; l < numLeds; l++) {
      leds[l] = CRGB(i, 0, distanceMapped);
    }
    //    ledcWrite(hpRedChannel, distanceMapped);
    //    ledcWrite(hpGreenChannel, touchValue);
    //    ledcWrite(hpBlueChannel, otherNodeTouchVal);
    FastLED.show();
    // vTaskDelay(2 / portTICK_PERIOD_MS);
  }
}
void ledTask(void* params) {
  while (1) {
    ledTest();
  }
}

void ultrasonicTask(void* params) {
  while (1) {
    measureDistance();
    delayMicroseconds(10);
  }
}

void setup() {
  Serial.begin(115200);
  uint32_t thisID = ESP.getChipId() & 0xffffff;
  Serial.printf("\r\n Chip ID:%x %x \r\n", ESP.getChipId(), thisID);
  NODE_REF = -1;
  for (int i = 0; i < maxNumNodes; i++) {
    if (thisID == node_ids[i]) {
      NODE_REF = i;
      Serial.printf("\r\n Chip ID: %s (%x) \r\n", node_names[NODE_REF].c_str(), ESP.getChipId(), thisID);
    }
  }
  if (NODE_REF < 0) {
    Serial.printf("\r\n Unknown Chip ID:%x unable to proceed \r\n", thisID);
    while (1);
  }

  initFileSystem();
  String files[10];

  getFilesInDirectory(files, "/");
  //Serial.printf("\r\n Chip ID:%x %x \r\n", ESP.getChipId(), thisID);


  //Configure FastLED for Addressable LEDs
  // FastLED.addLeds<WS2812B, dataPin, RGB>(leds, numLeds);
  initTest();

  // Configure Ultrasonic pins
  // pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  // pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
  initNodes();
  configNode(NODE_REF);


  initOTA();

  //Set up tasks


  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
#ifdef ESP32
  Serial.println(localPort);
#else
  Serial.println(Udp.localPort());
#endif
}

float measureDistance() {
  //  // Clears the trigPin
  //  digitalWrite(trigPin, LOW);
  //  delayMicroseconds(2);
  //
  //  // Sets the trigPin on HIGH state for 10 micro seconds
  //  digitalWrite(trigPin, HIGH);
  //  delayMicroseconds(10);
  //  digitalWrite(trigPin, LOW);
  //
  //  // Reads the echoPin, returns the sound wave travel time in microseconds
  //  // float duration = millis()-startTime;
  //  float duration = pulseIn(echoPin, HIGH);
  //
  //  // Calculating the distance
  //  float distance = duration * 0.034 / 2;
  //
  //  // Prints the distance on the Serial Monitor
  //  //Serial.print("Distance: ");
  //  // Serial.println(distance);
  //  int dis_int = (int)distance;
  //  dis_int = constrain(dis_int, 3, 100);
  //  dis_int = map(dis_int, 3, 100, 0, 255);
  //  distanceMapped = 255 - dis_int;
  //  //Serial.printf("distancemapped=%d\r\n", distanceMapped);
  //  return distance;
}

void rec_routine(OSCMessage &msg) {
  Serial.printf("Link1: %f\r\n", msg.getFloat(0));
}


void loop() {
  // it will run the user scheduler as well
  mesh.update();
  // Serial.println(WiFi.localIP());
  //  // ledTest();
  //  if (hasUltrasonic) {
  //    measureDistance();
  //  }


  nodeLoop(NODE_REF);
  checkForSerialCommands();
  //checkForOSC();
  //sendOSC();
  delay(500);
}

void sendOSC() {
  OSCMessage msg("/composition/dashboard/link1");
  msg.add((float)255 / 255);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();

}
void checkForOSC() {
  OSCMessage inmsg;

  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      // Serial.print(Udp.read());
      inmsg.fill(Udp.read());
    }
    if (!inmsg.hasError()) {
      if (inmsg.dispatch("/composition/dashboard/link1", rec_routine)) {
        //    Serial.println("MATCHED");
      }
      else {
        //    Serial.println("NOT MATCHED");
      }
    } else {
      error = inmsg.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }
}

void initOTA() {
  ArduinoOTA.setPassword("777");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
}

void checkForSerialCommands() {
  if (Serial.available() > 0) {
    byte inbyte = Serial.read();

    switch (inbyte) {
      case 'o': {
          unsigned long  time_elapsed = millis();
          while (millis() - time_elapsed < 300000) {
            ArduinoOTA.handle();
            delay(10);
          }
          break;
        }
    }
  }
}
