#include <SPI.h>
#include "WiFi.h"
#include <WiFiUdp.h>

//Works!!!!


int status = WL_IDLE_STATUS;

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //ENTER YOUR WIFI NAME AND PASSWORD HERE
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 

char ssid[] = "learygopro"; //  your network SSID (name)
char pass[] = "vuvuzela";    // your network password (use for WPA, or use as key for WEP)

int localPort = 7;
byte broadCastIp[] = { 10,5,5,9 };

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //ENTER YOUR MAC ADDRESS HERE
  //!!!!!!!!!!!!!!!!!!!!!!!!!!! 

byte remote_MAC_ADD[] = { 0xd4, 0xd9, 0x19, 0x8e, 0x55, 0x67 };
int wolPort = 9;


WiFiUDP Udp;

WiFiClient client;

const char* host = "10.5.5.9";

const int httpPort = 80;


void setup(){  

  //Initialize serial and wait for port to open:
  Serial.begin(115200);


/*  // check for the presence of the wifi module:
  if (WiFi.status() == WL_NO_SHIELD) {
     Serial.println("WiFi not present");
     // don't continue:
     while (true);
  }
*/

// ADD THIS TO LOOP??????????????
// MAKE ITS OWN FUNCTION???????

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
     status = WiFi.begin(ssid, pass);

    // wait 8 seconds for connection:
    delay(8000);
  }

  Serial.println("Connected to wifi");
  printWifiStatus();


}


void loop(){

// Add code to check to see if wifi is still on. If not, stop and keep trying to connect

// WAKE UP CAMERA FROM SLEEP AND CONNECT
CameraInitiate();

// Add code to make sure that camera recognized (status 31)


// START RECORDING
StartRecording();

delay(5000);

// STOP RECORDING
StopRecording();

delay(5000);

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


// Function to create and send magic packet
// Taken and translated from here:
// https://www.logicaprogrammabile.it/wol-accendere-computer-arduino-wake-on-lan/

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
