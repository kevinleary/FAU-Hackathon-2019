#include <Adafruit_NeoPixel.h>

int pulsePin = A0; // Pin Analog 0, or GPIO 36

#define ALARM 4000  //Alarm thresh hold
#define WARN 3000   //Warning thresh hold
#define NORMAL 2000 //Normal, below which is also Alarm thresh hold

#define NEOPIXEL_TYPE NEO_GBR + NEO_KHZ800
#define RGB_PIN 4   // Control pin for the Neo Pixel RGB

unsigned char r = 0;
unsigned char g = 0;
unsigned char b = 0;

float s = 0.0; //Initiate the analog signal

Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, RGB_PIN, NEOPIXEL_TYPE); // Initiate the neo pixel

// The SetUp Function:
void setup() {
  pixel.begin();
  Serial.begin(115200);         // Set's up Serial Communication. 
}// End setup

// The Main Loop Function
void loop() {
  s = analogRead(pulsePin);  
  
  // Set RGB LED Colour based on pulse
  
  r = (s >= ALARM || s <=NORMAL) ? 255 : ((s > WARN) ? 150 : 0);  // Turn the neo pixel RED if the pulse is above ALARM thresh hold or below the NORMAL thresh hold
  
  b = (s > WARN && s < ALARM) ? 255 : 0;        // Blue range
  
  g = (s > NORMAL && s < WARN) ? 255 : 0;       // Green range

  // This is just for testing purpose, comment out as needed

  Serial.println((String)"Signal value: "+s);
  
  Serial.println((String)"Red: " +  r);
  Serial.println((String)"Blue: " +  b);
  Serial.println((String)"Green: " +  g);
  
  
  pixel.setPixelColor(0, r, g,b );  // Set color for the neo pixel
  
  pixel.show();
  
  delay(500); // Act every 500ms
  
}//End loop
