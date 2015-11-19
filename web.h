
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

void handleStatus() {
  StaticJsonBuffer<200> jsonBuffer;
  String temp;

  JsonObject& root = jsonBuffer.createObject();
  root["currentStep"] = currentStep;
  root["autoMode"] = autoMode;
  root["calibrationFactor"] = calibrationFactor;
  root["glassWeight"] = glassWeight;
  root["beerWeight"] = beerWeight;
  root["buttonState"] = getButton();
  root["valveState"] = getValve();
  
  root.printTo(temp);
  server.send(200, "application/javascript", temp);
}

void handleSettings() {
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    
    if (server.argName(i) == "glassWeight") {
      glassWeight = server.arg(i).toFloat();
    }
    else if (server.argName(i) == "beerWeight") {
      beerWeight = server.arg(i).toFloat();
    }
  }
  server.send(200, "application/javascript", "OK");
}

void handleAutoMode() {
  switchMode();
  String temp;
  if (autoMode) {
    temp = "TRUE";
  }
  else {
    temp = "FALSE";
  }
  server.send(200, "application/javascript", temp);
}

void handleWeight() {
  char temp[50];
//  snprintf(temp, 50, "%05f", getWeight());
  dtostrf(getWeight(), 4, 2, temp);
  server.send(200, "application/javascript", temp);
}

void handleRoot() {
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  snprintf(temp, 400,

"<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP8266 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP8266!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>",

    hr, min % 60, sec % 60
  );
  server.send ( 200, "text/html", temp );
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}

void pingServer() {
  WiFiClient client;
  
  const char* host = "hooks.nymedia.no";
  const int httpPort = 80;
  
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
//
//  if (client.verify(fingerprint, host)) {
//    Serial.println("certificate matches");
//  } else {
//    Serial.println("certificate doesn't match");
//  }
  
  // We now create a URI for the request
  String url = "/BeerTap";
  url += "?currentStep=";
  url += currentStep;
  url += "&autoMode=";
  url += autoMode;
  url += "&buttonState=";
  url += getButton();
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
}
