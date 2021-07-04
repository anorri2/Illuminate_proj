

#include <FastLED.h>
#include "nodeConfigs.h"
#include "painlessMesh.h"
#include <ArduinoOTA.h>


void initOTA();


//DELETE ME
int otherNodeTouchVal = 0;
//

#define   MESH_PREFIX     "Eywa"
#define   MESH_PASSWORD   "avitarrr"
#define   MESH_PORT       5555

int NODE_REF = 0;
bool hasUltrasonic = false;
bool hasTouch = false;


const byte maxNumNodes = 8;
uint32_t node_ids[maxNumNodes] = {0xe24229, 0xf149f1, 0xe4e8bc, 0xe28477, 0x2d8f8f };
enum node_refs {ULTRASONIC_NODE, HIGH_POWER_NODE, EZP8266_NODE_0, ESP8266_NODE_1, NODE_MCU};
String node_names[maxNumNodes] = {"ultrasonic node", "high power node", "8266 node0", "8266 node1", "nodemcu node"};

typedef struct {
  uint32_t nodeId;
  String nodeName;
  uint8_t sensorVal[3];
} NODE_INFO;

NODE_INFO nodes[maxNumNodes];


Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;


//Addressable LED variables
const int numLeds = 20; // CHANGE FOR YOUR SETUP
const byte dataPin = 27;
CRGB leds[numLeds];

float measureDistance();

//High power LED variables
const int freq = 5000;
const int resolution = 8;

const int hpRedChannel = 0;
const int hpGreenChannel = 1;
const int hpBlueChannel = 2;

const int hpRedPin = 21;
const int hpGreenPin = 22;
const int hpBluePin = 23;

//Button input
const int buttonPin = 34;

//Touch Input
const int touchPin = 15;
int touchReadings[10];
int touchCheckCounter = 0;
int touchLowerBound = 1000;
int touchUpperBound = 0;
byte touchValue;
int ave = 0;
// Ultrasonic input
const int trigPin = 32;
const int echoPin = 35;
int distanceMapped = 0;



//Task Handles
TaskHandle_t h_messageTask = NULL;
TaskHandle_t h_touchTask = NULL;
TaskHandle_t h_ultrasonicTask = NULL;
TaskHandle_t h_ledTask = NULL;
// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void ultrasonicTask(void* params);


Task taskSendMessage( TASK_SECOND * 30 , TASK_FOREVER, &sendMessage );
void sendMessage() {
  //Do nothing we'll handle this in our own FreeRTOS task
}
int count = 0;
void sendData(void* params) {
  while (1) {
    String msg = "SINF:";
    msg += (String)distanceMapped;
    msg += ',';
    msg += (String)touchValue;
    msg += ',';
    msg += (String)0;
    mesh.sendBroadcast( msg );
    Serial.println("Sent message" + msg);
    count++;
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
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
  Serial.printf("Received from %s (%x) (%x)\r\n", nodeName.c_str(), from, from & 0xffffff);

  sscanf(msg.c_str(), "SINF:%d,%d,%d", &invals[0], &invals[1], &invals[2]);
  if (id >= 0) {
    nodes[id].sensorVal[0] = (byte)invals[0];
    nodes[id].sensorVal[1] = (byte)invals[1];
    nodes[id].sensorVal[2] = (byte)invals[2];
    otherNodeTouchVal = (byte)invals[1];
    Serial.printf("Received from %s:%x %u msg=%s  %d %d %d \r\n", nodeName.c_str(), from, from, msg.c_str(), nodes[id].sensorVal[0], nodes[id].sensorVal[1], nodes[id].sensorVal[2]);
  }
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

  int oldestPosCounter = 9;
  touchCheckCounter = 0;
  int runningTotal = 0;
  while (1) {
    touchReadings[touchCheckCounter] = touchRead(touchPin);
    runningTotal += touchReadings[touchCheckCounter];
    runningTotal -= touchReadings[oldestPosCounter];
    ave = touchRead(touchPin);//runningTotal / 10;
    ave = constrain(ave, touchLowerBound, touchUpperBound);
    ave = map(ave, touchLowerBound, touchUpperBound, 0, 255);
    ave = 255 - ave;
    touchValue = ave;
    touchCheckCounter = (touchCheckCounter == 9) ? 0 : touchCheckCounter + 1;
    oldestPosCounter = (oldestPosCounter == 9) ? 0 : oldestPosCounter + 1;
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void ledTest() {

  for (int i = 0; i < 255; i++) {

    for (int l = 0; l < numLeds; l++) {
      leds[l] = CRGB(i, 0, distanceMapped);
    }
    ledcWrite(hpRedChannel, distanceMapped);
    ledcWrite(hpGreenChannel, touchValue);
    ledcWrite(hpBlueChannel, otherNodeTouchVal);
    FastLED.show();
    vTaskDelay(2 / portTICK_PERIOD_MS);
  }
}
void ledTask(void* params) {
  while (1) {
    //ledTest();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void ultrasonicTask(void* params) {
  while (1) {
    measureDistance();
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup() {

  Serial.begin(115200);
  uint32_t thisID;

  uint8_t MAC[] = {0, 0, 0, 0, 0, 0};
  uint64_t mc = ESP.getEfuseMac();
  Serial.print("MAC Address=");
  Serial.println(WiFi.macAddress());
  for (int i = 0; i < 6; i++) {
    MAC[i] = (mc >> (8 * i)) & 0xff ;
  }

  uint32_t nodeId = painlessmesh::tcp::encodeNodeId(MAC) + 1;
  if (nodeId == 0) {
    Serial.println("NodeId set to 0\n");
  }
  else {
    Serial.print("NodeId=");
    Serial.println(nodeId, HEX);
  }

  thisID = nodeId & 0xffffff;

  NODE_REF = -1;
  for (int i = 0; i < maxNumNodes; i++) {
    if (thisID == node_ids[i]) {
      NODE_REF = i;
      Serial.printf("\r\n Chip ID: %s (%x) \r\n", node_names[NODE_REF].c_str(), thisID);
    }
  }
  if (NODE_REF < 0) {
    Serial.printf("\r\n Unknown Chip ID:%x unable to proceed \r\n", thisID);
    //while (1);
  }

  //Set up PWM on High Power LED pins
//  ledcSetup(hpRedChannel, freq, resolution);
//  ledcSetup(hpGreenChannel, freq, resolution);
//  ledcSetup(hpBlueChannel, freq, resolution);
//  ledcAttachPin(hpRedPin, hpRedChannel);
//  ledcAttachPin(hpGreenPin, hpGreenChannel);
//  ledcAttachPin(hpBluePin, hpBlueChannel);

  //Configure FastLED for Addressable LEDs
  //  int numl=random(4,10);
  //  int dpin=random(0,11);
  //FastLED.addLeds<WS2812B, dataPin, RGB>(leds, numl);
  initTest();

  // Configure Ultrasonic pins
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

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

  //Set up tasks

  xTaskCreatePinnedToCore(
    sendData,        // Function that should be called
    "Mesh Send Task",   // Name of the task (for debugging)
    5000,              // Stack size (bytes)
    NULL,             // Parameter to pass
    0,               // Task priority
    &h_messageTask , // Task handle
    1              //Run on core 1
  );

  xTaskCreatePinnedToCore(
    ultrasonicTask,        // Function that should be called
    "Ultrasonic Task",   // Name of the task (for debugging)
    5000,              // Stack size (bytes)
    NULL,             // Parameter to pass
    0,               // Task priority
    &h_ultrasonicTask , // Task handle
    0              //Run on core 1
  );

  xTaskCreatePinnedToCore(
    touchTask,        // Function that should be called
    "Touch Task",   // Name of the task (for debugging)
    5000,              // Stack size (bytes)
    NULL,             // Parameter to pass
    0,               // Task priority
    &h_touchTask , // Task handle
    0              //Run on core 1
  );

  xTaskCreatePinnedToCore(
    ledTask,        // Function that should be called
    "LED Task",   // Name of the task (for debugging)
    5000,              // Stack size (bytes)
    NULL,             // Parameter to pass
    0,               // Task priority
    &h_ledTask , // Task handle
    0              //Run on core 1
  );

  touchUpperBound = touchRead(touchPin);
  touchLowerBound = touchUpperBound * 0.3;

  initOTA();
}

float measureDistance() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  // float duration = millis()-startTime;
  float duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  float distance = duration * 0.034 / 2;

  // Prints the distance on the Serial Monitor
  //Serial.print("Distance: ");
  // Serial.println(distance);
  int dis_int = (int)distance;
  dis_int = constrain(dis_int, 3, 100);
  dis_int = map(dis_int, 3, 100, 0, 255);
  distanceMapped = 255 - dis_int;
  //Serial.printf("distancemapped=%d\r\n", distanceMapped);
  return distance;
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
  Serial.print ("Station IP=");
  IPAddress ipadd = mesh.getStationIP();
  Serial.println(ipadd);
  Serial.print ("AP IP=");
  IPAddress apadd = mesh.getAPIP();
  Serial.println(apadd);
  //  // ledTest();
  //  if (hasUltrasonic) {
  //    measureDistance();
  //  }

  nodeLoop(NODE_REF);
  vTaskDelay(50 / portTICK_PERIOD_MS);
  //Serial.printf("Touch value = %d", touchRead(touchPin));

  if (Serial.available() > 0) {
    byte inbyte = Serial.read();

    switch (inbyte) {
      case 'u' : {
          Serial.print("Setting Upperbound.");
          int upp = 0;
          int reading = touchRead(touchPin);
          int i = 0;
          while (i < 1000) {
            if (reading > upp) {
              upp = reading;
            }
            if (i % 100 == 0) {
              Serial.print(".");
            }

            i++;
            delay(1);
          }
          touchUpperBound = upp;
          Serial.print("Upperbound Set to ");
          Serial.println(touchUpperBound);
          break;
        }
      case 'l' : {
          int low = 5000;
          int reading = touchRead(touchPin);
          int i = 0;
          Serial.print("Setting Lowerbound.");
          while (i < 1000) {
            if (reading < low) {
              low = reading;
            }
            if (i % 100 == 0) {
              Serial.print(".");
            }
            i++;
            delay(1);
          }
          touchLowerBound = low;
          Serial.print("LowerBound Set to ");
          Serial.println(touchLowerBound);
          break;
        }
      case 'o': {
          unsigned long  time_elapsed = millis();
          //          while (millis() - time_elapsed < 30000) {
          while (1) {
            ArduinoOTA.handle();
            delay(10);
          }
          break;
        }

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
