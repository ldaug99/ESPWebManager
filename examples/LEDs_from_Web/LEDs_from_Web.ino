/*
   This project creates a simple website inferface for a
   coffee maker. Thie website features an API through which the
   coffee maker state (on/off) and auto turn-on/off alarms
   can be set. The project is build uppon the webManager library
   created by myself, which can be found at:
   https://github.com/ldaug99/ESPWebManager

   The ESPWebManager is build on the ESPAsyncWebServer library by
   me-no-dev and the SPIFFS library by espressif. The webManager
   library is an attempt to simplify setting up a website
   on an ESP32.

   The ESPAsyncWebServer library can be found at:
   https://github.com/me-no-dev/ESPAsyncWebServer

   The SPIFFS and Arduino-ESP32 core library can be found at:
   https://github.com/espressif/arduino-esp32

   To upload the HTML and resource files, use the esptool_py,
   by me-no-dev, which can be found at:
   https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/
   or simply open the project in PlatformIO and execute the
   command: platformio run --target uploadfs

   The library is distrubuted with the GNU Lesser General
   Public License v2.1, as per requirement of the Arduino-ESP32
   and ESPAsyncWebServer library.
   This library was created by ldaug99.
*/

// To upload website files located in the data folder, use:
// platformio run --target uploadfs

// Include needed libraries
#include <Arduino.h>
#include "ESPWebManager.h"

// Define WiFi SSID and password
const char* ssid = "ssid";
const char* password = "password";

// Coffee maker pin
const uint8_t LED1Pin = 21;
// Coffee heater pin
const uint8_t LED2Pin = 19;

// Number of URLs that the user can access
#define contentEntries 4
// Define all avaliable web pages and resouce elements avaliable to the webserver
const webContentEntry webContent[contentEntries] = {
  {"/",         "/index.html",    HTMLfile,   HTTP_GET},
  {"/myScript.js",  "/myScript.js",   RESfile,  HTTP_GET},
  {"/styles.css",   "/styles.css",    RESfile,  HTTP_GET},
  {"/api",      "",         API,    HTTP_GET | HTTP_POST | HTTP_PUT}
};

// Callback function decleration
void updateLED1State();
void updateLED2State();

// Number of API keywords
#define keywords 2
// Variables accessable from the API
bool bLED1State = false;
bool bLED2State = false;
// API keywords
apiKeyword apiKeywords[keywords] = {
  {"LED1State", "LED1STATE", pBOOL, &bLED1State, updateLED1State},
  {"LED2State", "LED2STATE", pBOOL, &bLED2State, updateLED2State},
};

// Initialize the webManager class using the given webcontent, but without API functionality
//webManager webCoffee(webContent, contentEntries);
// Initialize the webManager class, with the given webcontent and the given api keywords
webManager webCoffee(webContent, contentEntries, apiKeywords, keywords);

// Static HTML processor for the webManager instance. If more instances are needed, simply create more of theres functions, and add them to the respective webManager instancen in setup.
String processor(const String &var)  {
  return webCoffee.APIbasedProcessor(var);
}

// Callback for LED1State
void updateLED1State() {
  Serial.print("LED1 state now is: ");
  Serial.println(bLED1State);
  if (bLED1State) {
    Serial.println("Turning on LED1.");
    digitalWrite(LED1Pin, HIGH);
  } else {
    Serial.println("Turning off LED1.");
    digitalWrite(LED1Pin, LOW);
  }
}

// Callback for LED2State
void updateLED2State() {
  Serial.print("LED2 state now is: ");
  Serial.println(bLED2State);
  if (bLED2State) {
    Serial.println("Turning on LED2.");
    digitalWrite(LED2Pin, HIGH);
  } else {
    Serial.println("Turning off LED2.");
    digitalWrite(LED2Pin, LOW);
  }
}

// Setup
void setup(void) {
  // Start serial communication
  Serial.begin(115200);
  // Start SPIFFS (for managaing website files)
  webCoffee.startSPIFFS();
  // Start WiFi and connect to SSID
  webCoffee.startWIFIclient(ssid, password);
  // Start MDNS responder
  webCoffee.startMDNS("esp32");
  // Add API based HTML processer to webManager
  webCoffee.setHTMLprocessor(processor);
  // Start webManager
  webCoffee.begin();
  Serial.println("HTTP server started");
  // Set pinMode for LED pins
  pinMode(LED1Pin, OUTPUT);
  pinMode(LED2Pin, OUTPUT);
}

// Loop
void loop(void) {

}
