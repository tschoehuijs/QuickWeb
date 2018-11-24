/*
QuickWeb.h - Library for easily creating ESP8266 home website
Created by Tom Schoehuijs.
Released into the public domain.
*/

#ifndef QuickWeb_h
#define QuickWeb_h

#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"

class QuickWeb {
public:
  QuickWeb(int type, String IP); // for nodes
  QuickWeb(int type); // for sending nodes
  QuickWeb( ); // initialize for homebase
  void update( int data ); // set data
  void update( ); // fetch new data
  void print( WiFiClient client ); // print table row
  void html( WiFiClient client, int H ); // print header and footer
  void send( WiFiClient client ); // send return value
  void send( String packet ); // send command to node

  bool isNumeric(String str); // check if incoming packet is number
private:
  int _type; // define type
  int _data; // data for html
  String _IP; // IP address
};

#endif
