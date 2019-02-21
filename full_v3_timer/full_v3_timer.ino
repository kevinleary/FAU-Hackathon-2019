/* This code is to read raw distance signal from a ultrasonic sensor and change the 
 *  color or a Neo Pixel RGB accordinly to a pre-defined distance range
 */
//libraries 
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>     //for MQTT connection
#include "WiFi.h"
#include <WiFiUdp.h>

//Watson IoT connection details 
#define MQTT_HOST "x5ri6x.messaging.internetofthings.ibmcloud.com"    //First 6 characters found at (IBM Watson IoT Platform->Settings->Identity
#define MQTT_PORT 1883
#define MQTT_DEVICEID "d:x5ri6x:ESP32S:dev01"   
//                       ^          ^     ^---------|  
//                  org identity  device type   device id
#define MQTT_USER "use-token-auth"
#define MQTT_TOKEN "WATSONINMYASS"      //auth token :)
#define MQTT_TOPIC "iot-2/evt/status/fmt/json"
#define MQTT_TOPIC_DISPLAY "iot-2/cmd/display/fmt/json"

//int status = WL_IDLE_STATUS;

//GO PRO
char ssid_GP[] = "learygopro"; //  your network SSID (name) FOR GOPRO
char pass_GP[] = "vuvuzela";    // your network password FOR GOPRO

//FAU NETWORK
char ssid_FAU[] = "fau"; //  your network SSID (name) FOR FAU NETWORK
char pass_FAU[] = "";    // your network password FOR FAU NETWORK


int localPort = 7;
byte broadCastIp[] = { 10,5,5,9 };

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //ENTER YOUR MAC ADDRESS HERE
  //!!!!!!!!!!!!!!!!!!!!!!!!!!! 

byte remote_MAC_ADD[] = { 0xd4, 0xd9, 0x19, 0x8e, 0x55, 0x67 };
int wolPort = 9;
//int switch_wifi = 0;

WiFiUDP Udp;

WiFiClient client;

const char* host = "10.5.5.9";

const int httpPort = 80;

bool WIFI = false;
char connection = 'N';
// END WIFI SETUP
//**********************************************************************

//Define variables for BPM
int x=0;
int lastx=0;
int lasty=0;
int LastTime=0;
int ThisTime;
bool BPMTiming=false;
bool BeatComplete=false;
int BPM=0;
#define UpperThreshold 2730
#define LowerThreshold 2710


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     22 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define NUMFLAKES     10 // Number of snowflakes in the animation example
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

//DHT
#define DHTTYPE DHT11
#define DHT_PIN 4
#define BUTTON_PIN 39

// RANGE SETTINGS - SONAR
#define CLOSE 0
#define FAR 200
#define CENTER 50

//PIR MOTION
int sonarInputPin = 5;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int sonarValue = digitalRead(sonarInputPin);  // read input value

// variables to hold data for JSON
StaticJsonBuffer<200> jsonBuffer;
JsonObject& payload = jsonBuffer.createObject();
JsonObject& status = payload.createNestedObject("d");
static char msg[200];
//JSON delimiter global variable declaration b: BPM m: Motion 
int b = 0;
int m = 0;
unsigned long startTime; //timer to determine when to send data

/*
static const unsigned char PROGMEM logo_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };
 */
// defines pins numbers for HC SR04 Sensor
const int trigPin = 23; // TRIGGER PIN
const int echoPin = 19; // ECHO PIN



int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button


int d = 0.0; // distance

// defines variables for sonar
long duration;
int distance;
//variables for dht

DHT dht(DHT_PIN, DHTTYPE);
float h = 0.0; // humidity
float t = 0.0; // temperature

// MQTT objects
void callback(char* topic, byte* payload, unsigned int length);
WiFiClient wifiClient;
PubSubClient mqtt(MQTT_HOST, MQTT_PORT, callback, wifiClient);

// callback for payload
void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] : ");
  
  payload[length] = 0; // ensure valid content is zero terminated so can treat as c-string
  Serial.println((char *)payload);
}

//**********************************************************************************
//SETUP
//**********************************************************************************
void setup() {
  
  //Timer code to post data to Watson
  startTime = millis();
  
  
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(sonarInputPin, INPUT);     // declare sensor as input
  Serial.begin(115200); // Starts the serial communication
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  dht.begin();
  pinMode(BUTTON_PIN, INPUT); //initialize pushbutton pin as an input

    ConnectToWifi(ssid_GP, pass_GP);
  CameraInitiate();
  StartRecording();
  //delay(10000);
  //StopRecording();
  delay(1000);

  WiFi.disconnect(true);      //hopefully?
  delay(5000);
  //WiFi.begin(ssid_FAU, pass_FAU);
  
  ConnectToWifi(ssid_FAU, pass_FAU);
  //delay(10000);
  //StartMQTT();

    /*mqtt.loop();
if(WiFi.SSID() != ssid_GP){
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) {
      Serial.println("MQTT Connected");
      mqtt.subscribe(MQTT_TOPIC_DISPLAY);
      mqtt.loop();
    } else {
      Serial.println("MQTT Failed to connect!");
      delay(5000);
    }
  }
}
*/
  
}


// BEGIN MAIN LOOP
//*********************************************************************************
//*********************************************************************************
void loop() {

sonarValue = digitalRead(sonarInputPin);  // read input value - Sonar

  // read the pushbutton input pin:
  buttonState = digitalRead(BUTTON_PIN);

    // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;
      Serial.println("on");
      Serial.print("number of button pushes: ");
      Serial.println(buttonPushCounter);
      
    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("off");
    }
    // Delay a little bit to avoid bouncing
    //delay(50); Comment out to speed up
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;



// Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

// Calculating the distance
distance= duration*0.034/2;

d = distance;

h = dht.readHumidity();
t = dht.readTemperature(true);


//Begin BPM calc
//****************************************************************
  b = BPM;
  
  if(x>127)  
  {
    //oled.clearDisplay();
    x=0;
    lastx=x;
  }
 
  ThisTime=millis();
  int value=analogRead(A0);
  //oled.setTextColor(WHITE);
  int y=(value/32)-60;
  //oled.writeLine(lastx,lasty,x,y,WHITE);
  lasty=y;
  lastx=x;
  // calc bpm
  //Serial.println((String)"BPM: " + BPM);
 
  if(value>UpperThreshold)
  {
    if(BeatComplete)
    {
      BPM=ThisTime-LastTime;
      BPM=int(60/(float(BPM)/1000));
      BPMTiming=false;
      BeatComplete=false;
      //tone(8,1000,250);
    }
    if(BPMTiming==false)
    {
      LastTime=millis();
      BPMTiming=true;
    }
  }
  if((value<LowerThreshold)&(BPMTiming))
    BeatComplete=true;
    
  Serial.println((String)"BPM: " + BPM);
  x++;
//****************************************************************
// End BPM calc


//Display to OLED
display.clearDisplay();
display.setTextColor(WHITE);
display.setTextSize(1.8);
display.setCursor(0,0);

if(buttonPushCounter == 0){
  display.setTextSize(7);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.print("P10");
  
  }

if(buttonPushCounter%2 == 1){
PrintDistance(d);
PrintTemp();
PrintBPM(BPM);
ShowWifiStatus(WIFI);
PrintMotion();
}

if(buttonPushCounter%2 == 0 && buttonPushCounter > 1){

    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.writeLine(lastx,lasty,x,y,WHITE);
    display.writeFillRect(0,50,128,16,BLACK);
    display.setCursor(0,50);
    display.print(BPM);
    display.print(" BPM");
    PrintMotion();
    delay(20);
  }

display.display();

// Prints the distance on the Serial Monitor
Serial.print("Distance: ");
Serial.println(distance);
delay(100);

  //SEND JSON
  if(millis() - startTime >= 5000){
    StartMQTTLoop();
    startTime = millis();
  }
  
  //delay(100); Comment out to speed up

  if (h > 80) {
    WiFi.disconnect(true);
    //delay(5000);
    delay(1000);
    ConnectToWifi(ssid_GP, pass_GP);
    //delay(1000);Comment out to speed up. Halved on next line
    delay(500);
    StopRecording();
    //delay(1000);Comment out to speed up. Halved on next line
    delay(500);
    WiFi.disconnect(true);
    //delay(1000); Comment out to speed up. Halved on next line
    delay(500);
    ConnectToWifi(ssid_FAU, pass_FAU);
  }

}

// END MAIN LOOP
//*********************************************************************************
//*********************************************************************************


//Function Definitions

void StartMQTTLoop() {

  mqtt.loop();

  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) {
      Serial.println("MQTT Connected");
      mqtt.subscribe(MQTT_TOPIC_DISPLAY);
      mqtt.loop();
    } else {
      Serial.println("MQTT Failed to connect!");
      delay(5000);
    }
  }
  
    mqtt.loop(); // moved from line 416
     // Print Message to console in JSON format
   status["temp"] = t;
   status["humidity"] = h;
   status["distance"] = d;
   status["pulse"] = b;
   status["motion"] = m;
   //status["test"] = "test";
   payload.printTo(msg, 200);
   Serial.println(msg);
   if (!mqtt.publish(MQTT_TOPIC, msg)) {
      Serial.println("MQTT Publish failed");
   }
  
  // Pause - but keep polling MQTT for incoming messages
  /*
  for(int i = 0; i < 1; i++) {
    //mqtt.loop();
    delay(250);
  }
  //mqtt.loop();
  */

}

void PrintTemp() {

  display.print((String)"\n\nTemp: " + t + " F" + "\n\nHum: " + h + " %");
}

void PrintMotion() {

  if (sonarValue == HIGH) {
    
    if (pirState == LOW) {
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(2);
        display.setCursor(0,0);
        display.print("\nMotion \ndetected!");
        m = 1;
        delay(1000);
    }
  }
  else {
    if (pirState == HIGH) {
      display.print("\nMotion ended!");
      pirState = LOW;
    }
    m = 0;
  }
}

void PrintDistance(int d) {

  display.print("Target Distance: ");
  display.print(d); 
}

void PrintBPM(int BPM){

  display.print("\n\nBPM: ");
  display.print(BPM); 
}

void ConnectToWifi(char ssid[], char pass[]) {

  // attempt to connect to Wifi network:
  while ( WiFi.status() != WL_CONNECTED) {
    WIFI = false;
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    WiFi.begin(ssid, pass);

    // wait 8 seconds for connection:
    delay(8000);
  }

  Serial.println("Connected to wifi");
  WIFI = true;
  printWifiStatus();

}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void SendMagicPacket(){

  //Create a 102 byte array
  byte magicPacket[102];

  // Variables for cycling through the array
  int Cycle = 0, CycleMacAdd = 0, IndexArray = 0;

  // This for loop cycles through the array
  for( Cycle = 0; Cycle < 6; Cycle++){

    // The first 6 bytes of the array are set to the value 0xFF
    magicPacket[IndexArray] = 0xFF;

    // Increment the array index
    IndexArray++;
  }

  // Now we cycle through the array to add the GoPro address
  for( Cycle = 0; Cycle < 16; Cycle++ ){
    //eseguo un Cycle per memorizzare i 6 byte del
    //mac address
    for( CycleMacAdd = 0; CycleMacAdd < 6; CycleMacAdd++){
      
      magicPacket[IndexArray] = remote_MAC_ADD[CycleMacAdd];
      
      // Increment the array index
      IndexArray++;
    }
  }

  //The magic packet is now broadcast to the GoPro IP address and port
  Udp.beginPacket(broadCastIp, wolPort);
  Udp.write(magicPacket, sizeof magicPacket);
  Udp.endPacket();

}

void StartRecording(){
  Serial.print("connecting to ");
  Serial.println(host);

  if (!client.connect("10.5.5.9", httpPort)) {
    Serial.println("connection failed");
    return;
  }

  //Command for starting recording
  String StartUrl = "/gp/gpControl/command/shutter?p=1";
  Serial.print("Requesting URL: ");
  Serial.println(StartUrl);
  client.print(String("GET ") + StartUrl + " HTTP/1.1\r\n" +
  "Host: " + host + "\r\n" +
  "Connection: close\r\n\r\n");
  Serial.println("Recording");
}



void StopRecording(){
  Serial.print("connecting to ");
  Serial.println(host);
  
  if (!client.connect("10.5.5.9", httpPort)) {
    Serial.println("connection failed");
    return;
  }

  //Command for stopping recording
  String StopUrl = "/gp/gpControl/command/shutter?p=0";
  Serial.print("Requesting URL: ");
  Serial.println(StopUrl);
  client.print(String("GET ") + StopUrl + " HTTP/1.1\r\n" +
  "Host: " + host + "\r\n" +
  "Connection: close\r\n\r\n");
  Serial.println("Stopped");
}

void BootPhotoMode(){
  //Starts up photo mode
  Serial.print("connecting to ");
  Serial.println(host);
  
  if (!client.connect("10.5.5.9", httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String BootUrl = "/gp/gpControl/command/sub_mode?mode=1&sub_mode=0";
  Serial.print("Requesting URL: ");
  Serial.println(BootUrl);
  client.print(String("GET ") + BootUrl + " HTTP/1.1\r\n" +
  "Host: " + host + "\r\n" +
  "Connection: close\r\n\r\n");
  Serial.println("Photo Mode!");
}

void TakeAPic() {

  Serial.print("connecting to ");
  Serial.println(host);

  if (!client.connect("10.5.5.9", httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  //Command to take a pic
  String StopUrl = "/gp/gpControl/command/shutter?p=1";
  Serial.print("Requesting URL: ");
  Serial.println(StopUrl);
  client.print(String("GET ") + StopUrl + " HTTP/1.1\r\n" +
  "Host: " + host + "\r\n" +
  "Connection: close\r\n\r\n");
  Serial.println("Pic Taken!");

}


// FUNCTION TO WAKE UP THE CAMERA

void CameraInitiate(){

  //Begin UDP communication
  Udp.begin(localPort);

  //Send the magic packet to wake up the GoPro out of sleep
  delay(2000);
  SendMagicPacket();
  delay(5000);  

  // Absolutely necessary to flush port of UDP junk for Wifi client communication
  Udp.flush();
  delay(1000);

  //Stop UDP communication
  Udp.stop();
  delay(1000);

}

void ShowWifiStatus(bool WIFI){
  if(WIFI == true)
    connection = 'Y';
  else
    connection = 'N';
  
  display.print("   WIFI: ");
  display.print(connection);
  }
