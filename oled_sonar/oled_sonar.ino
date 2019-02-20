/* This code is to read raw distance signal from a ultrasonic sensor and change the 
 *  color or a Neo Pixel RGB accordinly to a pre-defined distance range
 */
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     22 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define NUMFLAKES     10 // Number of snowflakes in the animation example
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

// RANGE SETTINGS
#define CLOSE 0
#define FAR 200
#define CENTER 50

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


int t = 0.0; // distance

// defines variables
long duration;
int distance;

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(115200); // Starts the serial communication
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
}

void loop() {
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

t = distance;

if (t > CLOSE && t < CENTER) {

      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(2);
      display.setCursor(0,0);
      display.print("CLOSE! \n@ ");
      display.print(t);
}

else if (t > CENTER && t < FAR) {
      
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(2);
      display.setCursor(0,0);
      display.print("MID!\n@ ");
      display.print(t);
}

else if (t > FAR) {
      
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(2);
      display.setCursor(0,0);
      display.print("FAR!\n@ ");
      display.print(t);
}

display.display();
// Prints the distance on the Serial Monitor
Serial.print("Distance: ");
Serial.println(distance);
delay(100);
}
