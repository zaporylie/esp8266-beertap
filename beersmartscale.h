

HX711 scale = HX711(14, 12);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, 13, NEO_GRB + NEO_KHZ800);
const int buttonPin = 0;
const int valvePin = 13;

bool autoMode = false;
int currentStep = 0;
float calibrationFactor = 2312.0e-6;
float glassWeight = 100;
float beerWeight = 200;

bool getMode() {
  return autoMode;
}

void switchMode() {
  if (autoMode == true) {
    autoMode = false;
  }
  else {
    autoMode = true;
  }
}

bool matchGlass(float weight) {
  float hysteresis = 10;
  if ((weight >= (glassWeight - hysteresis)) && (weight <= (glassWeight + hysteresis))) {
    return true;
  }
  return false;
}

bool matchBeer(float weight) {
  float hysteresis = 10;
  if (weight <= beerWeight) {
    return false;
  }
  return true;
}

bool emptyTray(float weight) {
  float hysteresis = 10;
  if ((weight >= (0 - hysteresis)) && (weight <= (hysteresis))) {
    return true;
  }
  return false;
}

void tare() {
    // Read two values.
  scale.read_average(2);

  // Tare.
  scale.tare();  //Reset the scale to 0

  // Set scale.
  scale.set_scale(calibrationFactor); //Adjust to this calibration factor
}

void calibrate(float weight) {
  // Do the magic.
  
}

float getWeight(float offset = 0) {
  float weight = scale.get_units(4);
  Serial.print(weight, 2);
  Serial.print(" g");
  Serial.println();
  // Get avarage weight from 4 reading and return value in grams.
  return weight - offset;
}

void showWeight(float weight, float maximum = 1000) {
  // @todo add colors!
  Serial.println("Show weight");
  uint32_t color;
  if (autoMode && currentStep == 0) {
    color = strip.Color(0, 0, 150);  
  }
  else if (autoMode && currentStep == 1) {
    color = strip.Color(0, 150, 0);
  }
  else if (autoMode && currentStep == 2) {
    color = strip.Color(150, 0, 0);
  }
  else {
    color = strip.Color(150, 150, 0);
  }
  
  for (uint8_t i = 0; i < strip.numPixels(); i++) {
    if (i == 0 && weight < 5) {
      int sec = millis() / 1000;
      if (sec % 2) {
        strip.setPixelColor(i, color);
      }
      else {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
    }
    else if (i <= (weight * strip.numPixels() / maximum)) {
      strip.setPixelColor(i, color);
    }
    else {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
  }
}

void openValve() {
  bool valve = digitalRead(valvePin);
  Serial.print("Open valve: ");
  Serial.println(valve);
  digitalWrite(valvePin, HIGH);
}

void closeValve() {
  bool valve = digitalRead(valvePin);
  Serial.print("Close valve: ");
  Serial.println(valve);
  digitalWrite(valvePin, LOW);
}

bool getValve() {
  bool valve = digitalRead(valvePin);
  Serial.print("Get valve: ");
  Serial.println(valve);
  return valve;
}

bool getButton() {
  bool button = digitalRead(buttonPin);
  Serial.print("Get button: ");
  Serial.println(button);
  return !button;
}
