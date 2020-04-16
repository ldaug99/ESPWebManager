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
	// Get index of equals sign (request to set value)
	int16_t equalsIndex = requestURL->indexOf("=");
	// Check for request method
	if (requestURL->indexOf("=") >= 0) {
		// Get value
		String value = requestURL->substring(equalsIndex + 1);
		// Get keyword
		*requestURL = requestURL->substring(0, equalsIndex);
		// Set request
		return apiSet(requestURL, &value);
	} else {
		// Get request
		return apiGet(requestURL);
	}
}

// Default API based HTML processor
String WebAPI::htmlProcessor(const String &var) {
	#ifdef useVerboseSerial
		Serial.print("WebAPI::htmlProcessor(), Processor on: ");
		Serial.println(var);
	#endif
	// Get index of placeholder in keywords struct array
	int16_t index = findPlaceholderIndex(var);
	// Check if keywords was found
	if (index >= 0) {
		// Return processed string
		return processPlaceholderByType(index);
	}
	// If not found, return empty string object
	return String();
}

//*************************************************************
// Private functions
//*************************************************************

// GET responder
apiResponse WebAPI::apiGet(String *keyword) {
	// Get index of keyword in keywords struct array
	int16_t index = findKeywordIndex(keyword);
	// Check if keywords was found
	if (index >= 0) {
		#ifdef useVerboseSerial
			apiResponse reply = getValueByType(index);
			Serial.print("WebAPI::apiGet(), Reply code: ");
			Serial.print(reply.responseCode);
			Serial.print(" , with reply: ");
			Serial.println(reply.responseText);
			return reply;
		#else
			// Get value by type
			return getValueByType(index);
		#endif
	}
	// If not found, return error code
	return {404, "Not found"};
}

// POST responde
apiResponse WebAPI::apiSet(String *keyword, String *value) {
	// Get index of keyword in keywords struct array
	int16_t index = findKeywordIndex(keyword);
	// Check if keywords was found
	if (index >= 0) {
		#ifdef useVerboseSerial
			apiResponse reply = setValueByType(index, value);
			Serial.print("WebAPI::apiSet(), Reply code: ");
			Serial.print(reply.responseCode);
			Serial.print(" , with reply: ");
			Serial.println(reply.responseText);
			return reply;
		#else
			// Set value by type
			return setValueByType(index, value);
		#endif
	}
	// If not found, return error code
	return {404, "Not found"};
}

// Find keyword in keywords list
int16_t WebAPI::findKeywordIndex(String *keyword) {
	#ifdef useVerboseSerial
		Serial.print("WebAPI::findKeywordIndex(), Looking for keyword: ");
		Serial.println(*keyword);
	#endif
	// Loop through all API keywords
	for (uint8_t i = 0; i < _keywords; i++) {
		// Check if keywords is in requestURL
		if (keyword->indexOf(_apiKeywords[i].requestKeyword) >= 0) {
			#ifdef useVerboseSerial
				Serial.print("WebAPI::findKeywordIndex(), Found keyword with index: ");
				Serial.println(i);
			#endif
			return i;
		}
	}
	#ifdef useVerboseSerial
		Serial.println("WebAPI::findKeywordIndex(), Keyword not found.");
	#endif
	// If not found return -1
	return -1;
}

// Find htmlPlaceholder in keywords list
int16_t WebAPI::findPlaceholderIndex(const String &placeholder) {
	#ifdef useVerboseSerial
		Serial.print("WebAPI::findPlaceholderIndex(), Looking for HTML placeholder: ");
		Serial.println(placeholder);
	#endif
	// Loop through all API keywords
	for (uint8_t i = 0; i < _keywords; i++) {
		// Check if placeholder is in requestURL
		if (placeholder.indexOf(_apiKeywords[i].htmlPlaceholder) >= 0) {
			#ifdef useVerboseSerial
				Serial.print("WebAPI::findPlaceholderIndex(), Found HTML placeholder with index: ");
				Serial.println(i);
			#endif
			return i;
		}
	}
	#ifdef useVerboseSerial
		Serial.println("WebAPI::findPlaceholderIndex(), HTML placeholder not found.");
	#endif
	// If not found return -1
	return -1;
}

// Get value, based on type
apiResponse WebAPI::getValueByType(uint8_t index) {
	switch (_apiKeywords[index].valueType) {
		case FLOAT: {
			return {200, String(_apiKeywords[index].keyValue.floatValue)};
		} break;
		case INT: {
			return {200, String(_apiKeywords[index].keyValue.intValue)};
		} break;
		case UINT: {
			return {200, String(_apiKeywords[index].keyValue.uintValue)};
		} break;
		case CHARA: {
			return {200, String(_apiKeywords[index].keyValue.charArrayValue)};
		} break;
		case BOOL: {
			return {200, String(_apiKeywords[index].keyValue.boolValue)};
		} break;
	}
	// If not found, return error code
	return {404, "Not found"};
}

// Set value, based on type
apiResponse WebAPI::setValueByType(uint8_t index, String *value) {
	switch (_apiKeywords[index].valueType) {
		case FLOAT: {
			// Convert value to float
			_apiKeywords[index].keyValue.floatValue = value->toFloat();
			return {200, "Ok"};
		} break;
		case INT: {
			// Convert value to int64, cast to int32
			_apiKeywords[index].keyValue.intValue = (int32_t)value->toInt();
			return {200, "Ok"};
		} break;
		case UINT: {
			// Convert value to int64, cast to uint32
			_apiKeywords[index].keyValue.uintValue = (uint32_t)value->toInt();
			return {200, "Ok"};
		} break;
		case CHARA: {
			// Write stirng to char buffer
			value->toCharArray(_apiKeywords[index].keyValue.charArrayValue, charArraySize);
			return {200, "Ok"};
		} break;
		case BOOL: {
			// Convert value to int64, cast to bool
			_apiKeywords[index].keyValue.boolValue = (bool)value->toInt();
			return {200, "Ok"};
		} break;
	}
	// If not found, return error code
	return {404, "Not found"};
}


// Process placeholder by type
String WebAPI::processPlaceholderByType(uint8_t index) {
	switch (_apiKeywords[index].valueType) {
		case FLOAT: {
			return String(_apiKeywords[index].keyValue.floatValue);
		} break;
		case INT: {
			return String(_apiKeywords[index].keyValue.intValue);
		} break;
		case UINT: {
			return String(_apiKeywords[index].keyValue.uintValue);
		} break;
		case CHARA: {
			return String(_apiKeywords[index].keyValue.charArrayValue);
		} break;
		case BOOL: {
			return String(_apiKeywords[index].keyValue.boolValue);
		} break;
	}
	// If not found, return empty string object
	return String();
}