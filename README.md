# FAU-Hackathon-2019
This is a repo for all FAU Hackathon 2019 related materials using the ESP32

## Links and other info
[1 - ESP32 Pin layout](http://forum.fritzing.org/t/esp32s-hiletgo-dev-boad-with-pinout-template/5357)

[2 - Getting Started](https://randomnerdtutorials.com/getting-started-with-esp32/)

[3 - IoT Watson Api for ESP32](https://developer.ibm.com/recipes/tutorials/connect-an-esp32-to-the-watson-iot-platform/)

[4 - Github Repo for ESP32](https://github.com/espressif/arduino-esp32)

[5 - Github Repo with BLE examples and others](https://github.com/ibm-watson-iot/device-arduino/tree/master/samples)

## Setup of ESP32
[Useful info](https://github.com/djaramil/esp32Workshop/blob/master/README.md)

## Code to get MAC address of ESP32 

```#include "esp_system.h"
String getMacAddress() {
uint8_t baseMac[6];
// Get MAC address for WiFi station
esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
char baseMacChr[18] = {0};
sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
return String(baseMacChr);
}
