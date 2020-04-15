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
 * This library is build uppon the SPIFFS library and the 
 * ESPAsyncWebServer library, both of which are required to 
 * run this library.
 * SPIFFS should be included as part of the Arduino-ESP32 
 * library, which can be found at:
 * https://github.com/espressif/arduino-esp32
 * ESPAsyncWebServer can be found at:
 * https://github.com/me-no-dev/ESPAsyncWebServer
 * The library is distrubuted with the GNU Lesser General 
 * Public License v2.1, as per requirement of the Arduino-ESP32
 * and ESPAsyncWebServer library.
 * This library was created by ldaug99.
*/ 

// Ensure the library is only included once
#ifndef _WebAPI_
#define _WebAPI_

#include "Arduino.h"

#define useVerboseSerial true

// State types for the API manager
typedef enum {
  	DOUBLE, // Value is a double
  	LONG, // Value is a long
  	ULONG, // Value is a unsigned long
	CHARA, // Value is a char array with 8 array places
	BOOL // Value is a bool
} valueTypes; 

// API keyword template, for defining avaliable calls to the API
struct apiKeyword {
	String requestKeyword; // Keyword to request, /api/'requestKeyword'
	String htmlPlaceholder; // HTML placeholder to search for
	uint8_t valueType; // The type of value that the keyword codes for 
	union {
		double_t floatValue;
		int64_t longValue;
		uint64_t ulongValue;
		char charArrayValue[8];
		bool boolValue;
	} keyValue;
};

// API request response struct
struct apiResponse {
	uint16_t responseCode;
	String responseText;
};

class WebAPI {
	public:
		// Constructor
		WebAPI(apiKeyword *apiKeywords, const uint8_t keywords);

		// Empty constructor
		WebAPI();
		// Destructor
		~WebAPI();

		// Default API request handler
		apiResponse apiHandler(String *requestURL);

		// Default API based HTML processor
		String htmlProcessor(const String &var);

	private:
		// API keywords struct		
		apiKeyword *_apiKeywords;
		// Number of keywords
		const uint8_t _keywords;
};
#endif