/*
QuickWeb.cpp - Library for easily making websites.
Created by Tom Schoehuijs.
Released into the public domain.
*/

#include "Arduino.h"
#include "QuickWeb.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"

QuickWeb::QuickWeb(int type, String IP){
	_type = type;
	_data = 0;
	_IP = IP;

	if (~IP.endsWith("/")){
		IP+="/";
	}
}

QuickWeb::QuickWeb(int type){ // for sending nodes
	_type = type;
	_data = 0;
	_IP = "";
}

QuickWeb::QuickWeb( ){ // homebase
	_type = 0;
	_data = 0;
	_IP = "";
}

void QuickWeb::update(int data){
	_data = data;
	if (_data < 0){
		_data=0;
	}

	Serial.println("Data updated.");
}

void QuickWeb::update( ){
	WiFiClient clientGet;
	clientGet.setTimeout(200);

	const int httpGetPort = 80;
	//the path and file to send the data to:
	String urlGet = "status";

	Serial.println(">>> Connecting to host: " + _IP);
	Serial.println("Command: " + urlGet);

	if (!clientGet.connect(_IP, httpGetPort)) {
		Serial.println("Connection failed.");
	} else {
		Serial.println("Connection successful.");

		clientGet.println("GET " + urlGet + " HTTP/1.1");
		clientGet.print("Host: ");
		clientGet.println(_IP);
		clientGet.println("User-Agent: ESP8266/1.0");
		clientGet.println("Connection: close\r\n\r\n");

		unsigned long timeoutP = millis();
		while (clientGet.available() == 0) {
			if (millis() - timeoutP > 2000) {
				Serial.print(">>> Client Timeout: ");
				Serial.println(_IP);
				clientGet.stop();
				return;
			}
		}

		while (clientGet.available()) {
			String payload = clientGet.readString();
			String retLine = payload.substring(61, payload.length() - 2);

			if (isNumeric(retLine)) {
				Serial.println("Response is a number: " + retLine);
				_data=retLine.toInt();
			} else {
				Serial.println("Response is not a number:" + retLine);
			}
		}

	} //end client connection if else
	Serial.println("");
	Serial.print(">>> Closing host: ");
	Serial.println(_IP);

	clientGet.stop();
}

void QuickWeb::print( WiFiClient client ){
	String html;

	if (_type == 0){ // homebase
		html = "\r\n<col width=\"200\">\r\n<tr>\r\n<td><img src=\"https://rawgit.com/tschoehuijs/tschoehuijs.github.io/master/Images/homebase.png\" alt=\"Home Base\" height=\"150\"></td>\r\n<td></td>\r\n<td><div class=\"GaugeMeter\" data-used=\"@1\" data-type=\"temperature\"></div></td>\r\n<td></td>\r\n</tr>\r\n";
		html.replace("@1", String(_data));

	} else if (_type == 1){ // Node lights
		String lightstatus = "off";

		html = "\r\n<col width=\"200\">\r\n<tr>\r\n<td><img src=\"https://rawgit.com/tschoehuijs/tschoehuijs.github.io/master/Images/node.png\" alt=\"Node\" height=\"120\"></td>\r\n<td><a href=\"light=toggle\"><button class=\"lights @1\">@1</button></a></td>\r\n<td></td>\r\n<td></td>\r\n</tr>\r\n";

		if (_data==1){
			lightstatus = "on";
		}

		html.replace("@1", lightstatus);

	} else if (_type == 2){ // Node temperature
		html = "\r\n<col width=\"200\">\r\n<tr>\r\n<td><img src=\"https://rawgit.com/tschoehuijs/tschoehuijs.github.io/master/Images/node.png\" alt=\"Node\" height=\"120\"></td>\r\n<td></td>\r\n<td><div class=\"GaugeMeter\" data-used=\"@1\" data-type=\"temperature\"></div></td>\r\n<td></td>\r\n</tr>\r\n";
		html.replace("@1", String(_data));

	} else if (_type == 3){ // Node soil
		html = "\r\n<col width=\"200\">\r\n<tr>\r\n<td><img src=\"https://rawgit.com/tschoehuijs/tschoehuijs.github.io/master/Images/node.png\" alt=\"Node\" height=\"120\"></td>\r\n<td></td>\r\n<td></td>\r\n<td><div class=\"GaugeMeter\" data-percent=\"@1\" data-type=\"soil\"></div></td>\r\n</tr>\r\n";
		html.replace("@1", String(_data));

	} else if (_type == 4){ // Node christmas
		String lightstatus = "off";

		html = "\r\n<col width=\"200\">\r\n<tr>\r\n<td><img src=\"https://orig00.deviantart.net/87c2/f/2017/255/b/9/b987e595870d3adfd058286c027102fe-dbn70th.png\" alt=\"Node\" height=\"120\"></td>\r\n<td><a href=\"christmas=toggle\"><button class=\"lights @1\">@1</button></a></td>\r\n<td></td>\r\n<td></td>\r\n</tr>\r\n";

		if (_data==1){
			lightstatus = "on";
		}

		html.replace("@1", lightstatus);

	}

	client.print(html);
}

void QuickWeb::html( WiFiClient client, int H ){
	String html;


	if ( H == 0 ){ // html start
		client.println("HTTP/1.1 200 OK");
		client.println("Content-Type: text/html");
		client.println(""); //  do not forget this one

		html ="<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>ESP8266 Home Automation Server</title>\r\n<link rel=\"stylesheet\" type=\"text/css\" href=\"https://rawgit.com/tschoehuijs/tschoehuijs.github.io/master/Scripts/main.css\">\r\n<link rel=\"stylesheet\" type=\"text/css\" href=\"http://netdna.bootstrapcdn.com/font-awesome/4.3.0/css/font-awesome.min.css\">\r\n<link rel=\"icon\" type=\"image/x-icon\" href=\"https://rawgit.com/tschoehuijs/tschoehuijs.github.io/master/favicon.ico\">\r\n<script src=\"https://code.jquery.com/jquery-2.1.4.min.js\"></script>\r\n<script src=\"https://rawgit.com/tschoehuijs/tschoehuijs.github.io/master/Scripts/GaugeMeter.js\"></script>\r\n</head>\r\n<body>\r\n<center>\r\n<table style=\"width:auto\">\r\n<col width=\"300\">\r\n<tr>\r\n<th></th>\r\n<th><img src=\"https://rawgit.com/tschoehuijs/tschoehuijs.github.io/master/Images/light.png\" alt=\"Lights\" height=\"150\"></th>\r\n<th><img src=\"https://rawgit.com/tschoehuijs/tschoehuijs.github.io/master/Images/temperature.png\" alt=\"Temperature sensors\" height=\"150\"></th>\r\n<th><img src=\"https://rawgit.com/tschoehuijs/tschoehuijs.github.io/master/Images/soil.png\" alt=\"Soil\" height=\"150\"></th>\r\n</tr>\r\n";
	} else if ( H == 1 ){ // html end
		html ="\r\n</table>\r\n</center>\r\n<div class=\"footer\">\r\n<div id=\"left\">\r\n<div id='ss_menu'>\r\n<div>\r\n<a class=\"fa fa-linkedin\" href=\"http://linkedin.com/in/tschoehuijs\" target=\"_blank\" style=\"text-decoration:none;color:#fff\"></a>\r\n</div>\r\n<div>\r\n<a class=\"fa fa-twitter\" href=\"http://twitter.com/tschoehuijs\" target=\"_blank\" style=\"text-decoration:none;color:#fff\"></a>\r\n</div>\r\n<div>\r\n<a class=\"fa fa-facebook-official\" href=\"http://facebook.com/tschoehuijs\" target=\"_blank\" style=\"text-decoration:none;color:#fff\"></a>\r\n</div>\r\n<div class='menu'>\r\n<div class='share' id='ss_toggle' data-rot='180'>\r\n<div class='circle'></div>\r\n<div class='bar'></div>\r\n</div>\r\n</div>\r\n</div>\r\n</div>\r\n<div id=\"right\">\r\n<a href=\"restart\"><button class=\"version\" href=\"restart\"><span class=\"restart\"></span></button></a>\r\n</div>\r\n<div id=\"center\"><a href=\"\"><button class=\"refresh\" href=\"#\">Refresh</button></a></div>\r\n</div>\r\n<script>\r\n$(\".GaugeMeter\").gaugeMeter();\r\n$(document).ready(function(ev) { var toggle = $('#ss_toggle');\r\nvar menu = $('#ss_menu');\r\nvar rot;\r\n$('#ss_toggle').on('click', function(ev) { rot = parseInt($(this).data('rot')) + 180;\r\nmenu.css('transform', 'rotate(' + rot + 'deg)');\r\nmenu.css('webkitTransform', 'rotate(' + rot + 'deg)');\r\nif ((rot / 180) % 2 == 0) { toggle.parent().addClass('ss_active');\r\ntoggle.addClass('close');\r\n} else { toggle.parent().removeClass('ss_active');\r\ntoggle.removeClass('close');\r\n} $(this).data('rot', rot);\r\n});\r\nmenu.on('transitionend webkitTransitionEnd oTransitionEnd', function() { if ((rot / 180) % 2 == 0) { $('#ss_menu div i').addClass('ss_animate');\r\n} else { $('#ss_menu div i').removeClass('ss_animate');\r\n} });\r\n});\r\n</script>\r\n</body>\r\n</html>";

	}

	client.print(html);
}

void QuickWeb::send( WiFiClient client ){
	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	client.println("");
	client.println("<!DOCTYPE HTML>");
	client.println(String(_data));

	Serial.println("Data sent.");
}

void QuickWeb::send( String packet ){ // send a command to a node
	WiFiClient clientGet;
	const int httpGetPort = 80;

	//the path and file to send the data to:
	// String urlGet = "/command/" + _IP + "/status";
	String urlGet = "christmas=" + packet;

	Serial.print(">>> Connecting to host: ");
	Serial.println(_IP);
	Serial.println("Command: " + urlGet);

	if (!clientGet.connect(_IP, httpGetPort)) {
		Serial.print("Connection failed: ");
		Serial.print(_IP);
	} else {
		Serial.println("Connection successful.");

		clientGet.println("GET " + urlGet + " HTTP/1.1");
		clientGet.print("Host: ");
		clientGet.println(_IP);
		clientGet.println("User-Agent: ESP8266/1.0");
		clientGet.println("Connection: close\r\n\r\n");

		unsigned long timeoutP = millis();
		while (clientGet.available() == 0) {
			if (millis() - timeoutP > 10000) {
				Serial.print(">>> Client Timeout: ");
				Serial.println(_IP);
				clientGet.stop();
				return;
			}
		}
	} //end client connection if else
	Serial.println("");
	Serial.print(">>> Closing host: ");
	Serial.println(_IP);

	clientGet.stop();
}

bool QuickWeb::isNumeric(String str) {
	unsigned int stringLength = str.length();

	if (stringLength == 0) {
		return false;
	}

	boolean seenDecimal = false;

	for(unsigned int i = 0; i < stringLength; ++i) {
		if (isDigit(str.charAt(i))) {
			continue;
		}

		if (str.charAt(i) == '.') {
			if (seenDecimal) {
				return false;
			}
			seenDecimal = true;
			continue;
		}
		return false;
	}
	return true;
}
