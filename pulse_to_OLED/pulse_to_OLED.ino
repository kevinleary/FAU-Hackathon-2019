#include <Adafruit_SSD1306.h>
 
#define OLED_Address 0x3C        // Try 0x3D if not working
Adafruit_SSD1306 oled(128, 64);  // create our screen object setting resolution to 128x64
 
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
 
void setup() {
  Serial.begin(115200);
  oled.begin(SSD1306_SWITCHCAPVCC, OLED_Address);
  oled.clearDisplay();
  oled.setTextSize(2);
}
 
 
void loop() 
{
  if(x>127)  
  {
    oled.clearDisplay();
    x=0;
    lastx=x;
  }
 
  ThisTime=millis();
  int value=analogRead(A0);
  oled.setTextColor(WHITE);
  int y=(value/32)-60;
  oled.writeLine(lastx,lasty,x,y,WHITE);
  lasty=y;
  lastx=x;
  // calc bpm
  Serial.println((String)"Signal value: " + value);
 
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
    
    // display bpm
  oled.writeFillRect(0,50,128,16,BLACK);
  oled.setCursor(0,50);
  oled.print(BPM);
  oled.print(" BPM");
  oled.display();
  x++;
  delay(20);
}
