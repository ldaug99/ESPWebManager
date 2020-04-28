HTTP_POST = "POST";
HTTP_GET = "GET";
// Update HTML on page load
function updateHTML() {
	// Coffee maker state
	if (document.getElementById("LED1Button").value == "LED1State=0") {
		document.getElementById("LED1Button").innerHTML = "Turn on LED1";
		document.getElementById("LED1Button").value = "LED1State=1"
		document.getElementById("LED1Txt").innerHTML = "Off"
	} else {
		document.getElementById("LED1Button").innerHTML = "Turn off LED1";
		document.getElementById("LED1Button").value = "LED1State=0"
		document.getElementById("LED1Txt").innerHTML = "On"
	}

	// Coffee heater state
	if (document.getElementById("LED2Button").value == "LED2State=0") {
		document.getElementById("LED2Button").innerHTML = "Turn on LED2";
		document.getElementById("LED2Button").value = "LED2State=1"
		document.getElementById("LED2Txt").innerHTML = "Off"
	} else {
		document.getElementById("LED2Button").innerHTML = "Turn off LED2";
		document.getElementById("LED2Button").value = "LED2State=0"
		document.getElementById("LED2Txt").innerHTML = "On"
	}
}
// Reloading HTML page, after request was processed
function apiReplyListener() {
	setTimeout(() => { location.reload(); }, 500);
}
// Send API request
function apiRequest(type, parameter) {
	var requestUrl = "/api/" + parameter;
	var xmlHTTP = new XMLHttpRequest();
	xmlHTTP.open(type, requestUrl, true);
	xmlHTTP.onload = apiReplyListener();
	xmlHTTP.send(null);
}