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

// Ensure the library is only included once
#ifndef _WebAPI_
#define _WebAPI_

#include "Arduino.h"
#include "ESPAsyncWebServer.h"

#define useVerboseSerial true

// State types for the API manager
typedef enum {
	pBOOL,
	pUINT,
	pINT,
	pFLOAT,
	pSTRING
} valueTypes; 

// Char array size
#define charArraySize 4

// Callback function to use when variable changes
typedef void (*onSetCallback)();

// API keyword template, for defining avaliable calls to the API
struct apiKeyword {
	String requestKeyword; // Keyword to request, /api/'requestKeyword'
	String htmlPlaceholder; // HTML placeholder to search for
	uint8_t valueType; // The type of value that the keyword codes for 
	void *valuePointer;
	/*
	union {
		float_t floatValue;
		int32_t intValue;
		uint32_t uintValue;
		char charArrayValue[charArraySize];
		bool boolValue;
	} keyValue;
	*/
	onSetCallback callback; // Function pointer to call on 
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

		// Get responder
		apiResponse apiGet(String *keyword);
		// Set responder
		apiResponse apiSet(String *keyword, String *value);

		// Find keyword in keywords list
		int16_t findKeywordIndex(String *keywordName);
		// Find htmlPlaceholder in keywords list
		int16_t findPlaceholderIndex(const String &placeholder);

		// Get value, based on type
		apiResponse getValueByType(uint8_t index);
		// Set value, based on type
		apiResponse setValueByType(uint8_t index, String *value);

		// Process placeholder by type
		String processPlaceholderByType(uint8_t index);
};
#endif