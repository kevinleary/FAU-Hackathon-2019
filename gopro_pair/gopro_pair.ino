#include <SPI.h>
#include "WiFi.h"
#include <WiFiUdp.h>

//works

int status = WL_IDLE_STATUS;

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //ENTER YOUR WIFI NAME AND PASSWORD HERE
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 

char ssid[] = "learygopro"; //  your network SSID (name)
char pass[] = "vuvuzela";    // your network password (use for WPA, or use as key for WEP)

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
  }*/

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


  // START PIN
  StartPin();

  delay(10000);

  // FINISH PIN
  FinishPin();


}


void loop(){

  //Nothing to do here!
  delay(1000);

}


void StartPin(){
  Serial.print("connecting to ");
  Serial.println(host);

  if (!client.connect("10.5.5.9", httpPort)) {
    Serial.println("connection failed");
    return;
  }

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //ENTER YOUR PIN HERE WHERE IT SAYS XXXX
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  
  
  String StartUrl = "/gpPair?c=start&pin=XXXX&mode=0";
  Serial.print("Requesting URL: ");
  Serial.println(StartUrl);
  client.print(String("GET ") + StartUrl + " HTTP/1.1\r\n" +
  "Host: " + host + "\r\n" +
  "Connection: close\r\n\r\n");
  Serial.println("Started");
}



void FinishPin(){
  Serial.print("connecting to ");
  Serial.println(host);
  
  if (!client.connect("10.5.5.9", httpPort)) {
    Serial.println("connection failed");
    return;
  }

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //ENTER YOUR PIN HERE WHERE IT SAYS XXXX
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  
  String StopUrl = "10.5.5.9/gpPair?c=finish&pin=XXXX&mode=0";
  Serial.print("Requesting URL: ");
  Serial.println(StopUrl);
  client.print(String("GET ") + StopUrl + " HTTP/1.1\r\n" +
  "Host: " + host + "\r\n" +
  "Connection: close\r\n\r\n");
  Serial.println("Finished");
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
