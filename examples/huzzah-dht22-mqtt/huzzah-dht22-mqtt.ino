/**
 * @description Simple Demo for CoT Library using MQTT
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */
/** 
   This project demonstrates how simple it is to build a DTCoT application

   This example also shows that DTCoT does not depend on device type,
   network and protocol libraies, and can be easily extended by adding more
   devices and underlaying protocols (as soon as the cloud starts supporting them).

   Application description:
   In this demo a real temperature measurement from DHT22 sensor is sent to the cloud
   via the ESP8266 WiFi module.

   This sketch has the following library dependencies:
        - <Adafruit MQTT Library> v0.20.1
        - <ESP8266WiFi> v1.0
        - <Time> v1.5
        - <DHT sensor library (Default)> v1.3.0
 */

#include "dt-arduino-iot-agent.h"
#include "cot_sdk/DTCotSdkMqtt.h"
#include "cot_sdk/DTCoTDevice.h"

#include "devices/DTCoTDeviceHUZZAH_WIFI.h"
#include "DHT.h"


/************************* WiFi Access Point *********************************/
#define SECRET_WIFI_SSID       "...your local Wifi network's SSID..."
#define SECRET_WIFI_PASSWORD   "...your local Wifi network's password..."

/*************************** Sketch Code ************************************/

using namespace DTCoT;

#define DHTPIN D2       // what digital pin the DHT22 is conected to (D2 = 4)
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors

float tempThreshold = 27; 	// Alarm threshold, °C


/* Setup the Cloud access and the device to communicate to cloud */

CoTDevice cotDevice;

DHT dht(DHTPIN, DHTTYPE);


void setup() {
	delay(100);
	Serial.begin(115200);
	delay(100);
    DT_DEBUG_PRINTLN_INFO("\nSetup...");

    /* Setup the Cloud access and the device to communicate to cloud. */
    CoTConfigDeviceHuzzahWifi* boardConfig = new CoTConfigDeviceHuzzahWifi(SECRET_WIFI_SSID, SECRET_WIFI_PASSWORD);
    CoTDeviceHuzzahWifi* board = new CoTDeviceHuzzahWifi(*boardConfig);
    CotSdkMqtt* cotSDK = new CotSdkMqtt(*board);

    //initialize the device
    while(!cotDevice.setup(cotSDK));

	delay(0);
	DT_DEBUG_PRINTLN_INFO("Setup... done.");
}

void loop() {
    float temp = dht.readTemperature(); // Read temperature as Celsius (the default)

	// Check if any reads failed and exit early (to try again).
	if (isnan(temp)) {
		DT_INFO_PRINTLN("### Failed to read temperature from DHT22 sensor!");
		return;
	}

	DT_DEBUG_PRINTLN("Send measurement: %f", tempC);
	cotDevice.sendMeasurement("ty", "Room1", "Temperature", "C", temp, now());

	if (temp > tempThreshold) {
		DT_DEBUG_PRINTLN("### temp: %f greater than threshold: %f => send Alarm!",
				temp, tempThreshold);

		cotDevice.sendAlarm("Temperature threshold exceeded!", "temperature_alarm",
				AlarmSeverity::MAJOR, AlarmStatus::ACTIVE);
	}

    DT_DEBUG_PRINTLN_INFO("Loop... done.");
	delay(10000); //[ms]
}
