/*
 * WebAPI is a subelement of ESPWebManager.
 * ESPWebManager provides an implementation of an asynchronous
 * web server (ESPAsyncWebServer) and a SPIFFS filesystem for
 * to easily deploy a website on a ESP32 with multiple web pages
 * javascript and css styles.
 * At the same time it features an interface for setting and
 * getting states of user defined variables on the ESP. This
 * "API", thus allows the web pages to change states of
 * varaibles on the ESP, using simple web request which can be
 * implemented in javascript, using the XMLHttpRequest()
 * function.
 * 
 * This library is build uppon the Arduino-ESP core library,
 * SPIFFS library and the ESPAsyncWebServer library, both 
 * of which are required to run this library.
 * 
 * SPIFFS should be included as part of the Arduino-ESP32 
 * library, which can be found at:
 * https://github.com/espressif/arduino-esp32
 * 
 * ESPAsyncWebServer can be found at:
 * https://github.com/me-no-dev/ESPAsyncWebServer
 * 
 * The library is distrubuted with the GNU Lesser General 
 * Public License v2.1, as per requirement of the Arduino-ESP32
 * and ESPAsyncWebServer library.
 * This library was created by ldaug99.
*/ 

#include "Arduino.h"
#include "WebAPI.h"

//*************************************************************
// Public functions
//*************************************************************

// Constructor
WebAPI::WebAPI(apiKeyword *apiKeywords, const uint8_t keywords) : _apiKeywords(apiKeywords), _keywords(keywords){}

// Default API request handler
apiResponse WebAPI::apiHandler(String *requestURL) {
	#ifdef useVerboseSerial
		Serial.print("WebAPI::apiHandler(), API request on URL: ");
		Serial.println(*requestURL);
	#endif





	if (requestURL->indexOf("coffeeState") >= 0) {
		if (requestURL->indexOf("on") >= 0) {
			Serial.println("Coffee now on");
			//teststate = true;
			_apiKeywords[0].valueType = true;

			return {200, "OK"};
		} else {
			Serial.println("Coffee now off");
			//setCoffeeState(false);
			_apiKeywords[0].valueType = false;

			return {200, "OK"};
		}
	}
	return {200, "OK"};



}

// Default API based HTML processor
String WebAPI::htmlProcessor(const String &var) {
	#ifdef useVerboseSerial
		Serial.println("WebAPI::htmlProcessor(), Starting webServer...");
		Serial.println("WebAPI::htmlProcessor(), Setting up webpath responses...");
	#endif


	Serial.print("webManager::begin(), Processor on var: ");
	Serial.println(var);
	if (var == "COFFEESTATE"){
		if (_apiKeywords[0].valueType){
			return "On";
		} else{
			return "Off";
		}
	}




	return String();
}