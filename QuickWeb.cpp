/*
QuickWeb.cpp - Library for easily making websites.
Created by Tom Schoehuijs.
Released into the public domain.

Node explaination:
0: home
1: lights (living room)
2: temperature (kitchen)
3: temperature (outside)
4: Plant
5: Cactus
6: christmas lights
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

	switch(_type){
		case 1:
		key = "light";
		break;
		case 6:
		key = "christmas";
		break;
		default:
		key = "null";
		break;
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
	Serial.println("");
}

void QuickWeb::update( ){
	WiFiClient clientGet;
	clientGet.setTimeout(200);

	const int httpGetPort = 80;
	//the path and file to send the data to:
	String urlGet = "status";

	Serial.println("");
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

	}
	Serial.print(">>> Closing host: ");
	Serial.println(_IP);

	clientGet.stop();
}

void QuickWeb::print( WiFiClient client ){
	String html;

	switch(_type) {
		case 0:{
			html = "<div class=\"card\">\r\n<div class=\"container\">\r\n<img src=\"https://cdn.jsdelivr.net/gh/tschoehuijs/tschoehuijs.github.io@UI2/Images/Node0.png\" alt=\"Home Base\" height=\"80\" style=\"padding-top: 15px;\">\r\n<p>Homebase</p>\r\n</div>\r\n<div class=\"GaugeMeter gaugeMeter\" data-used=\"@1\" data-type=\"temperature\" style=\"width: 200px;\"><span style=\"line-height: 200px; font-size: 44px;\">@1<u>degC</u></span><b style=\"line-height: 276.923px; color: rgb(255, 255, 255);\">Temperature</b><canvas width=\"200\" height=\"200\"></canvas></div>\r\n</div>\r\n";
			html.replace("@1", String(_data));
		}
		break;

		case 1:{ // lights (living room)
			String lightstatus = "off";

			html = "<div class=\"card\">\r\n<div class=\"container\">\r\n<img src=\"https://cdn.jsdelivr.net/gh/tschoehuijs/tschoehuijs.github.io@UI2/Images/Node3.png\" alt=\"Lights\" height=\"90\" style=\"padding-top: 15px; margin-bottom: -10px;\">\r\n<p>Living Room</p>\r\n</div><br><br><br>\r\n<a href=\"light=toggle\"><button class=\"lights @1\" style=\"padding-top: 15px;\">@1</button>\r\n</a>\r\n</div>\r\n";

			if (_data==1){
				lightstatus = "on";
			}

			html.replace("@1", lightstatus);
		}
		break;

		case 2:{ // temperature (kitchen)
			html = "<div class=\"card\">\r\n<div class=\"container\">\r\n<img src=\"https://cdn.jsdelivr.net/gh/tschoehuijs/tschoehuijs.github.io@UI2/Images/Node2.png\" alt=\"Kitchen\" height=\"80\" style=\"padding-top: 15px;\">\r\n<p>Kitchen</p>\r\n</div>\r\n<div class=\"GaugeMeter gaugeMeter\" data-used=\"@1\" data-type=\"temperature\" style=\"width: 200px;\"><span style=\"line-height: 200px; font-size: 44px;\">@1<u>degC</u></span><b style=\"line-height: 276.923px; color: rgb(255, 255, 255);\">Temperature</b><canvas width=\"200\" height=\"200\"></canvas></div>\r\n</div>\r\n";
			html.replace("@1", String(_data));
		}
		break;

		case 3:{ // temperature (outside)
			html = "<div class=\"card\">\r\n<div class=\"container\">\r\n<img src=\"https://cdn.jsdelivr.net/gh/tschoehuijs/tschoehuijs.github.io@UI2/Images/Node1.png\" alt=\"Outside\" height=\"80\" style=\"padding-top: 15px;\">\r\n<p>Outside</p>\r\n</div>\r\n<div class=\"GaugeMeter gaugeMeter\" data-used=\"@1\" data-type=\"temperature\" style=\"width: 200px;\"><span style=\"line-height: 200px; font-size: 44px;\">@1<u>degC</u></span><b style=\"line-height: 276.923px; color: rgb(255, 255, 255);\">Temperature</b><canvas width=\"200\" height=\"200\"></canvas></div>\r\n</div>\r\n";
			html.replace("@1", String(_data));
		}
		break;

		case 4:{ // plant (general)
			html = "<div class=\"card\">\r\n<div class=\"container\">\r\n<img src=\"https://cdn.jsdelivr.net/gh/tschoehuijs/tschoehuijs.github.io@UI2/Images/Node4.png\" alt=\"Plant\" height=\"80\" style=\"padding-top: 15px;\">\r\n<p>Plant</p>\r\n</div>\r\n<div class=\"GaugeMeter\" data-percent=\"@1\" data-type=\"soil\" style=\"width: 200px;\"><span style=\"line-height: 200px; font-size: 44px;\">@1<u>degC</u></span><b style=\"line-height: 276.923px; color: rgb(255, 255, 255);\">Moisture</b><canvas width=\"200\" height=\"200\"></canvas></div>\r\n</div>\r\n";
			html.replace("@1", String(_data));
		}
		break;

		case 5:{ // cactus
			html = "<div class=\"card\">\r\n<div class=\"container\">\r\n<img src=\"https://cdn.jsdelivr.net/gh/tschoehuijs/tschoehuijs.github.io@UI2/Images/Node4.png\" alt=\"Cactus\" height=\"80\" style=\"padding-top: 15px;\">\r\n<p>Cactus</p>\r\n</div>\r\n<div class=\"GaugeMeter\" data-percent=\"@1\" data-type=\"soil\" style=\"width: 200px;\"><span style=\"line-height: 200px; font-size: 44px;\">@1<u>degC</u></span><b style=\"line-height: 276.923px; color: rgb(255, 255, 255);\">Moisture</b><canvas width=\"200\" height=\"200\"></canvas></div>\r\n</div>\r\n";
			html.replace("@1", String(_data));
		}
		break;

		case 6:{ // christmas lights
			String lightstatus = "off";

			html = "<div class=\"card\">\r\n<div class=\"container\">\r\n<img src=\"https://gallery.yopriceville.com/var/resizes/Free-Clipart-Pictures/Christmas-PNG/Christmas_Winter_House_Transparent_PNG_Clip_Art_Image.png?m=1507172112\" alt=\"Chrstmas Houses\" height=\"90\" style=\"padding-top: 15px; margin-bottom: -10px;\">\r\n<p>Chrstmas Houses</p>\r\n</div><br><br><br>\r\n<a href=\"christmas=toggle\"><button class=\"lights @1\" style=\"padding-top: 15px;\">@1</button>\r\n</a>\r\n</div>\r\n";

			if (_data==1){
				lightstatus = "on";
			}

			html.replace("@1", lightstatus);
		}
		break;
	}
	client.print(html);
}

void QuickWeb::html( WiFiClient client, int H ){
	String html;


	if ( H == 0 ){ // html start
		client.println("HTTP/1.1 200 OK");
		client.println("Content-Type: text/html");
		client.println(""); //  do not forget this one

		html = "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1252\">\r\n<title>CHAP-E</title>\r\n<link rel=\"stylesheet\" type=\"text/css\" href=\"https://cdn.jsdelivr.net/gh/tschoehuijs/tschoehuijs.github.io@UI2/Scripts/main.css\">\r\n<link rel=\"stylesheet\" type=\"text/css\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css\">\r\n<link rel=\"icon\" type=\"image/x-icon\" href=\"https://cdn.jsdelivr.net/gh/tschoehuijs/tschoehuijs.github.io@UI2/favicon.ico\">\r\n<script src=\"https://code.jquery.com/jquery-2.1.4.js\"></script>\r\n<script src=\"https://cdn.jsdelivr.net/gh/tschoehuijs/tschoehuijs.github.io@UI2/Scripts/GaugeMeter.js\"></script>\r\n</head>\r\n\r\n<body>\r\n<center>\r\n\r\n<div class=\"flex-container\">\r\n";

	} else if ( H == 1 ){ // html end
		html ="\r\n</div><br><br><br><br></center>\r\n<div class=\"footer\">\r\n<div id=\"left\">\r\n<div id='ss_menu'>\r\n<div>\r\n<a class=\"fa fa-linkedin\" href=\"http://linkedin.com/in/tschoehuijs\" target=\"_blank\" style=\"text-decoration:none;color:#fff\"></a>\r\n</div>\r\n<div>\r\n<a class=\"fa fa-twitter\" href=\"http://twitter.com/tschoehuijs\" target=\"_blank\" style=\"text-decoration:none;color:#fff\"></a>\r\n</div>\r\n<div>\r\n<a class=\"fa fa-facebook-official\" href=\"http://facebook.com/tschoehuijs\" target=\"_blank\" style=\"text-decoration:none;color:#fff\"></a>\r\n</div>\r\n<div class='menu'>\r\n<div class='share' id='ss_toggle' data-rot='180'>\r\n<div class='circle'></div>\r\n<div class='bar'></div>\r\n</div>\r\n</div>\r\n</div>\r\n</div>\r\n<div id=\"right\">\r\n<a href=\"restart\"><button class=\"version\" href=\"restart\"><span class=\"restart\"></span></button></a>\r\n</div>\r\n<div id=\"center\"><a href=\"\"><button class=\"refresh\" href=\"#\">Refresh</button></a></div>\r\n</div>\r\n<script>\r\n$(\".GaugeMeter\").gaugeMeter();\r\n$(document).ready(function(ev) { var toggle = $('#ss_toggle');\r\nvar menu = $('#ss_menu');\r\nvar rot;\r\n$('#ss_toggle').on('click', function(ev) { rot = parseInt($(this).data('rot')) + 180;\r\nmenu.css('transform', 'rotate(' + rot + 'deg)');\r\nmenu.css('webkitTransform', 'rotate(' + rot + 'deg)');\r\nif ((rot / 180) % 2 == 0) { toggle.parent().addClass('ss_active');\r\ntoggle.addClass('close');\r\n} else { toggle.parent().removeClass('ss_active');\r\ntoggle.removeClass('close');\r\n} $(this).data('rot', rot);\r\n});\r\nmenu.on('transitionend webkitTransitionEnd oTransitionEnd', function() { if ((rot / 180) % 2 == 0) { $('#ss_menu div i').addClass('ss_animate');\r\n} else { $('#ss_menu div i').removeClass('ss_animate');\r\n} });\r\n});\r\n</script>\r\n</body>\r\n</html>";

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
	String urlGet;

	if (_type == 6){
		urlGet = "christmas=" + packet;
	} else {
		urlGet = "light=" + packet;
	}

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
