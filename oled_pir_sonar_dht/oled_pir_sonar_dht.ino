/* This code is to read raw distance signal from a ultrasonic sensor and change the 
 *  color or a Neo Pixel RGB accordinly to a pre-defined distance range
 */
//libraries 
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

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

// RANGE SETTINGS - SONAR
#define CLOSE 0
#define FAR 200
#define CENTER 50

//PIR MOTION
int inputPin = 5;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = digitalRead(inputPin);  // read input value

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
  
// defines pins numbers for HC SR04 Sensor
const int trigPin = 23; // TRIGGER PIN
const int echoPin = 19; // ECHO PIN


int d = 0.0; // distance

// defines variables for sonar
long duration;
int distance;
//variables for dht
DHT dht(DHT_PIN, DHTTYPE);
float h = 0.0; // humidity
float t = 0.0; // temperature

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(inputPin, INPUT);     // declare sensor as input
  Serial.begin(115200); // Starts the serial communication
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  dht.begin();
}

void loop() {

val = digitalRead(inputPin);  // read input value - Sonar
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

display.clearDisplay();
display.setTextColor(WHITE);
display.setTextSize(1.8);
display.setCursor(0,0);
PrintDistance(d);
PrintTemp();
PrintMotion();
/*
if (d > CLOSE && d < CENTER) {

      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(1.8);
      display.setCursor(0,0);
      display.print("CLOSE! @ ");
      display.print(d);
      PrintTemp();
      PrintMotion();
}

else if (d > CENTER && d < FAR) {
      
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(1.8);
      display.setCursor(0,0);
      display.print("MID! @ ");
      display.print(d);
      PrintTemp();
      PrintMotion();
}

else if (d > FAR) {
      
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(1.8);
      display.setCursor(0,0);
      display.print("FAR! @ ");
      display.print(d);
      PrintTemp();
      PrintMotion();
}
*/
display.display();
// Prints the distance on the Serial Monitor
Serial.print("Distance: ");
Serial.println(distance);
delay(100);
}

void PrintTemp() {

  display.print((String)"\nTemp: " + t + "\nHum: " + h);
}

void PrintMotion() {

  if (val == HIGH) {
    
    if (pirState == LOW) {
      display.print("\nMotion detected!");
    }
  }
  else {
    if (pirState == HIGH) {
      display.print("\nMotion ended!");
      pirState = LOW;
    }
  }
}

void PrintDistance(int d) {

  display.print("Target Distance: ");
  display.print(d);
  /*if (d > CLOSE && d < CENTER) {


      display.print("Target Distance: ");
      display.print(d);
      delay(100);

  }

  else if (d > CENTER && d < FAR) {
      

      display.print("Target Distance: ");
      display.print(d);
 

  }

  else if (d > FAR) {
      
      display.print("Target Distance:");
      display.print(d);
      delay(100);
  }*/

  
}
