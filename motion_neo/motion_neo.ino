/* This code is to read raw distance signal from a Pyroelectric Infrared PIR sensor and change the 
 *  color or a Neo Pixel RGB accordinly to a pre-defined conditions.
 */
 
#include <Adafruit_NeoPixel.h>
#define NEOPIXEL_TYPE NEO_RGB + NEO_KHZ800
#define RGB_PIN 5 // GPIO pin the data line of RGB LED is connected to

Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, RGB_PIN, NEOPIXEL_TYPE);

int inputPin = 18;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status

unsigned char r = 0;
unsigned char b = 0;
unsigned char g = 0;

void setup() {
  pinMode(inputPin, INPUT);     // declare sensor as input
  Serial.begin(115200);
  pixel.begin();
}
 
void loop(){
  val = digitalRead(inputPin);  // read input value
  if (val == HIGH) {
    
    // check if the input is HIGH
    if (pirState == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");
      // We only want to print on the output change, not state
      pirState = HIGH;
      b = 0;
      g = 0;
      r = 250;
      pixel.setPixelColor(0, r, g, b);
      pixel.show();
      delay(1000);
    }
  } else {
    if (pirState == HIGH){
      // we have just turned of
      Serial.println("Motion ended!");
      b = 0;
      g = 250;
      r = 0;
      pixel.setPixelColor(0, r, g, b);
      pixel.show();
      // We only want to print on the output change, not state
      pirState = LOW;
    }
  }
}
