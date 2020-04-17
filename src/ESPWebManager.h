/*
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
#ifndef _ESPWebManager_
#define _ESPWebManager_

#include "Arduino.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "WebAPI.h"

#define useVerboseSerial true
#define defaultWebPort 80

// Content types for web
typedef enum {
  	HTMLfile, // Content is a HTML file, and should be send as HTML string with processor enabled (to allow updating variable states)
  	RESfile, // Content is a resources file and should be passed as text formatted as according to the file extension
  	API // Content (or rather webpath) is an api interface, and should reply with a response code and a short text message
} contentTypes; 

// Struct template for managing webpages
struct webContentEntry {
	const char *webPath; // The path to type in a browser to access the content
	const char *fileName; // The (path and) filename to the file, which SPIFFS is to access
	const uint8_t contentType; // The content type
	const WebRequestMethodComposite methods; // Methods allowed for the web content, normally GET for HTML and resource files.
};

// Callback function typedef (will return an apiResponse struct, is named apiCallback (and is a pointer to a function), take a String pointer as input)
typedef apiResponse (*apiCallback)(String *);

// Callback function typedef for HTML processor
typedef String (*htmlProcessor)(const String &);

// Callback function typedef for HandleNotFound function.
typedef void (*NotFoundHandle)(AsyncWebServerRequest *);

class webManager {
	friend class WebAPI;

	public:
		// Constructor with API disabled
		webManager(const webContentEntry *webContent, const uint8_t contentEntries);
		// Constructor with API enabled
		webManager(const webContentEntry *webContent, const uint8_t contentEntries, apiKeyword *apiKeywords, const uint8_t keywords);

		// Set custom api callback function (override existing)
		void setAPICallback(apiCallback callback);
		// Check if using custom API handler
		bool usingCustomAPIHandler();

		// Set HTML placeholder processor callback
		void setHTMLprocessor(htmlProcessor callback);
		// Run HTML processor, using API keywords
		String APIbasedProcessor(const String &var);

		// Set Handle Not found callback
		void setNotFoundHandle(NotFoundHandle callback);

		// Start the web manager
		void begin(uint16_t webPort = defaultWebPort);

	private:
		// Webcontent entri struct
		const webContentEntry *_webContent;
		// Number of web entries
		const uint8_t _contentEntries;

		// Web server pointer
		AsyncWebServer *_server;

		// API class pointer
		WebAPI *api;
		// Custom callback pointer for API request handler
		apiCallback _apiCallback = nullptr;

		// Callback pointer for not found handler
		NotFoundHandle _NotFoundHandle = nullptr;

		// Callback pointer for HTML processor
		htmlProcessor _htmlProcessor = nullptr;

		// Process web content, and assign proper handler
		void processWebEntry(const webContentEntry *entry);

		// HTML request responder
		void onHTMLrequest(const webContentEntry *entry);
		// Resource request responder
		void onResourceRequest(const webContentEntry *entry);
		// API request responder
		void onAPIrequest(const webContentEntry *entry);
};
#endif