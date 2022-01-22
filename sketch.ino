/*
 Demo to use MAX30100 heart beat & SpO2 sensor and publish to Firebase Realtime Database.
 I use ESPectro Core development board, but basically you can use any ESP8266-based board.
 Created by Andri Yadi, Makestro.com on 11/6/16.
*/

//#include <Arduino.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <ESP8266WiFi.h>
//#include <ESP8266HTTPClient.h>
//#include "ESP8266WebServer.h"
#include <FirebaseArduino.h>

#define REPORTING_PERIOD_MS     1500

// Set Firebase Host and Auth key. Key is buried under "Project Settings" of a project.
// Here's how to get the key: https://raw.githubusercontent.com/andriyadi/Firebase_ESP8266_Demo/master/HowToGetTheKey.gif
#define FIREBASE_HOST "heart-sensor-5d89a-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "hkHke8WrY4hKMNn5fmTL1tCzx9xGDh8tbnrvbZzM"

// Set your WiFi credentials
#define WIFI_SSID       "Jay R "
#define WIFI_PASSWORD   "password??"

// PulseOximeter is the higher level interface to the sensor. Let's create the object here
PulseOximeter pox;

uint32_t tsLastReport = 0;
volatile boolean heartBeatDetected = false;

// Callback method will be fired when a pulse is detected
void onBeatDetected()
{
    heartBeatDetected = true;
    Serial.println("Beat!");
}

void setup()
{
    Serial.begin(115200);

    // Connect to wifi.
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());

    // Begin connection to Firebase
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.set("redlight", 0); 

    // Create the Firebase Realtime Database keys (if not yet) and set default values
//    Firebase.set("sensor/HeartBeat", 0);
//    Firebase.set("sensor/SpO2", 0);

    // Show the last error
    if (Firebase.failed()) {
        Serial.print("setting /message failed:");
        Serial.println(Firebase.error());
    }
else{
  Serial.print("Firebase success !:");
}
    // Initialize the PulseOximeter instance and register a beat-detected callback
      Serial.print("Initializing pulse oximeter..");

    // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }
    pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop()
{
    // Make sure to call update as soon as possible
    pox.update();

    // Asynchronously dump heart rate and oxidation levels to the serial, and publish to Firebase
    // For both, a value of 0 means "invalid"
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {

        float bpm = pox.getHeartRate();
        float spO2 = pox.getSpO2();

        Serial.print("Heart rate:");
        Serial.print(bpm);
        Serial.print("bpm / SpO2:");
        Serial.print(spO2);
        Serial.println("%");

        tsLastReport = millis();

        // If the values are valid and heart beat detected, publish to Firebase
        if (heartBeatDetected && bpm != 0) {
            Firebase.setFloat("sensor/HeartBeat", bpm);
            Firebase.setInt("sensor/SpO2", spO2);
        }
    }
}
/*
void loop()
{
//  pox.begin();
//    pox.setOnBeatDetectedCallback(onBeatDetected);
     
    // Make sure to call update as soon as possible
    pox.update();

    // Asynchronously dump heart rate and oxidation levels to the serial, and publish to Firebase
    // For both, a value of 0 means "invalid"
   
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
       float bpm = pox.getHeartRate();
        float spO2 = pox.getSpO2();
          Serial.print("Heart rate:");
        Serial.print(bpm);
        Serial.print("bpm / SpO2:");
        Serial.print(spO2);
        Serial.println("%");
        tsLastReport = millis();
        // If the values are valid and heart beat detected, publish to Firebase
            // If the values are valid and heart beat detected, publish to Firebase
        if (heartBeatDetected && bpm != 0) {
float x = pox.getHeartRate();
        float y = pox.getSpO2();
            Firebase.setFloat("sensor/HeartBeat",x);
            Firebase.setFloat("sensor/SpO2",  y);
        }
      
    }
//     if (heartBeatDetected ) {
//            Firebase.setFloat("sensor/HeartBeat", bpm);
//            Firebase.setFloat("sensor/SpO2", spO2);
//        }
} */
