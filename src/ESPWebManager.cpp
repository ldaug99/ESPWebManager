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

#include "Arduino.h"
#include "ESPWebManager.h"

//*************************************************************
// Public functions
//*************************************************************

// Constructor, API disabled
webManager::webManager(const webContentEntry *webContent, const uint8_t contentEntries) : _webContent(webContent), _contentEntries(contentEntries){
	delete &api;
}

// Constructor, API enabled
webManager::webManager(const webContentEntry *webContent, const uint8_t contentEntries, apiKeyword *apiKeywords, const uint8_t keywords) : _webContent(webContent), _contentEntries(contentEntries){
	// Start instance of WebAPI
	api = new WebAPI(apiKeywords, keywords);
}

// Set custom API callback (override existing)
void webManager::setAPICallback(apiCallback callback) {
	// Set API callback pointer
	_apiCallback = callback;
}

// Check if using custom API handler
bool webManager::usingCustomAPIHandler() {
	return _apiCallback != nullptr;
}

// Set HTML placeholder processor callback
void webManager::setHTMLprocessor(htmlProcessor callback) {
	_htmlProcessor = callback;
}

// Run HTML processor, using API keywords
String webManager::APIbasedProcessor(const String &var) {
	return api->htmlProcessor(var);
}

// Set Handle Not found callback
void webManager::setNotFoundHandle(NotFoundHandle callback) {
	_NotFoundHandle = callback;
}

// Start web manager
void webManager::begin(uint16_t webPort) {
	#ifdef useVerboseSerial
		Serial.println("webManager::begin(), Starting webServer...");
		Serial.println("webManager::begin(), Setting up webpath responses...");
	#endif
	// Create AsyncWebServer object on webPort0
	_server = new AsyncWebServer(webPort);
	// Process all web content and assign on request handler 
	for (uint8_t i = 0; i < _contentEntries; i++) {
		#ifdef useVerboseSerial
			Serial.print("webManager::begin(), Setting reponse for webpath: ");
			Serial.println(_webContent[i].webPath);
		#endif
		// Setting webserver responses
		processWebEntry(&_webContent[i]);
	}
	// Check if a callback for the not found handler is given
	if (_NotFoundHandle != nullptr) {
		// On invalid path
		_server->onNotFound(this->_NotFoundHandle);
	} else {
		_server->onNotFound([](AsyncWebServerRequest *request){
			request->send(404, "text/plain", "Not found");
		});
	}
	// Start the webserver
	_server->begin();
	#ifdef useVerboseSerial
		Serial.println("webManager::begin(), WebServer setup finished!");
	#endif
}

//*************************************************************
// Private functions
//*************************************************************

// Process web content, and assign proper handler
void webManager::processWebEntry(const webContentEntry *entry) {
	// Setup the server response on request
	switch (entry->contentType) {
		case HTMLfile: onHTMLrequest(entry); break; // HTML content response
		case RESfile: onResourceRequest(entry); break; // Resource file response
		case API: onAPIrequest(entry); break;

		// TODO: add more response types.
	}
}

// Send HTML page on request
void webManager::onHTMLrequest(const webContentEntry *entry) {
	// Get the fileName to pass on request
	const char *fileName = entry->fileName;
	// Send HTML response, with processor enabled
	_server->on(entry->webPath, entry->methods, [this,fileName](AsyncWebServerRequest *request){
		request->send(SPIFFS, fileName, String(), false, this->_htmlProcessor);
	});
}

// Resource request responder
void webManager::onResourceRequest(const webContentEntry *entry) {
	// Get the fileName to pass on request
	const char *fileName = entry->fileName;
	// Get filename from pageLookup
	String contentType = fileName;
	// Get extension of filename
	contentType = contentType.substring(contentType.lastIndexOf(".") + 1, contentType.length());
	// Send simple text response
	_server->on(entry->webPath, entry->methods, [fileName, contentType](AsyncWebServerRequest *request){
		//request->send(SPIFFS, fileName, "text/" + contentType);
		request->send(SPIFFS, fileName);
	});
}

// API request responder
void webManager::onAPIrequest(const webContentEntry *entry) {
	// HTML API request route
	_server->on(entry->webPath, entry->methods, [this](AsyncWebServerRequest *request){
		// Get relative URL form the request
		String requestURL = request->url().c_str();
		// Prepare API reply
		apiResponse reply;
		// Handle the API request, check if using custom handler
		if (this->usingCustomAPIHandler()) {
			// Execute custom API handler
			reply = this->_apiCallback(&requestURL);
		} else {
			// Execute default API handler
			reply = this->api->apiHandler(&requestURL);
		}
		// Send back a replay, 200 if ok
		request->send(reply.responseCode, "text/plain",reply.responseText);
	});
}