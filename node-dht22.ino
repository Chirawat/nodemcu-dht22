// JSON http://www.akexorcist.com/2016/11/firebase-and-esp8266-with-arduino.html


//
// Copyright 2015 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// FirebaseDemo_ESP8266 is a sample that demo the different functions
// of the FirebaseArduino API.

#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#include "DHT.h"
#define DHTPIN D1     
#define DHTTYPE DHT22   // DHT 22  (AM2302)

// Set these to run example.
#define FIREBASE_HOST "amber-inferno-700.firebaseio.com"
#define FIREBASE_AUTH "NnQPZr2JkzTNckvt4Iycin4uxDylADJECcZlYf4g"
#define WIFI_SSID "CHATSUDA"
#define WIFI_PASSWORD "@pass196"


DHT dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); 
  
  Serial.begin(9600);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  dht.begin();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

int n = 0;

void loop() {
  // Indicator 
  digitalWrite(LED_BUILTIN, LOW); 
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);

  delay(9900);
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t) ) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index
  // Must send in temp in Fahrenheit!
  float hi = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.print(" *C\t ");
  Serial.print("Heat index: ");
  Serial.print(hi);
  Serial.println(" *C");

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& valueObject = jsonBuffer.createObject();
  StaticJsonBuffer<50> jsonBuffer_t;
  JsonObject& timeStampObject = jsonBuffer_t.createObject();
  
  timeStampObject[".sv"] = "timestamp";  
  valueObject["timestamp"] = timeStampObject;
  valueObject["temperature"] = t;
  valueObject["Humidity"] = h;
  valueObject["HeatIndex"] = hi;
  
  // append a new value to /logs
  String name = Firebase.push("logs", valueObject);
  // handle error
  if (Firebase.failed()) {
      Serial.print("pushing /logs failed:");
      Serial.println(Firebase.error());  
      return;
  }
  Serial.print("pushed: /logs/");
  Serial.println(name);
  Serial.println();
}
