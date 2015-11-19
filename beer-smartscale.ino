#include <ArduinoJson.h>
#include "HX711.h"
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h> 
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include "beersmartscale.h"
#include "web.h"

void setup() {
  Serial.begin(115200);
  getButton();
  getValve();

  Serial.println("Initiating valve");
  Serial.println("initiating...");
  pinMode(valvePin, OUTPUT);
  closeValve();

  Serial.println("Initiating button");
  Serial.println("initiating...");

  pinMode(buttonPin, INPUT);

  Serial.println("Preparing LED display");
  Serial.println("initiating...");

  strip.begin();
  strip.show();
  
  Serial.println("HX711 calibration");
  Serial.println("taring...");

  tare();

  WiFiManager wifi;
  wifi.autoConnect("AutoConnectAP");

  
  if ( MDNS.begin ( "beertap" ) ) {
    Serial.println ( "MDNS responder started" );
  }

  server.on ( "/", handleRoot );
  server.on ( "/switchMode", handleAutoMode );
  server.on ( "/status", handleStatus );
  server.on ( "/weight", handleWeight );
  server.on ( "/tare", handleTare );
  server.on ( "/settings", handleSettings );
  server.onNotFound ( handleNotFound );
  
  httpUpdater.setup(&server);
  server.begin();
  Serial.println ( "HTTP server started" );
}

void loop() {

  // Get weight.
  float weight = getWeight();
  
  if (getMode()) {
    switch (currentStep) {
      case 0:
        closeValve();
        showWeight(weight, glassWeight);
        if (matchGlass(weight)) {
          delay(800);
          if (matchGlass(weight)) {
            currentStep = 1;
            pingServer();
          }
        }
        break;
  
      case 1:
        showWeight(weight - glassWeight, beerWeight);
        if (matchBeer(weight - glassWeight)) {
          closeValve();
          currentStep = 2;
          pingServer();
        }
        else {
          openValve();
        }
        break;
  
      case 2:
        closeValve();
        showWeight(weight, beerWeight + glassWeight);
        if (emptyTray(weight)) {
          delay(800);
          if (emptyTray(weight)) {
            currentStep = 0;
            pingServer();
            tare();
          }
        }
        break;
    }
  }
  else {
    // Old way - read button!
    if (getButton()) {
      openValve();
    }
    else {
      closeValve();
    }
    showWeight(weight, beerWeight + glassWeight);
  }
  server.handleClient(); 
}



