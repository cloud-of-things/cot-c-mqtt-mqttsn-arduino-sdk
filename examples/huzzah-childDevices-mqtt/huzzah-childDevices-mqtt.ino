/**
 * @description Simple Demo for CoT Library using MQTT
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */
/** 
   This project demonstrates how simple it is to build a DTCoT application

   This example also shows that DTCoT master device can be extended by adding
   more child devices (sensors).

   Application description:
   In this demo simulated temperature measurement and alarm from master device is sent to the cloud,
   also simulated humidity measurement and alarm will be sent to child devices.


   This sketch has the following library dependencies:
        - <Adafruit MQTT Library> v0.20.1
		- <ESP8266WiFi> v1.0
        - <Time> v1.5
*/

#include "dt-arduino-iot-agent.h"
#include "cot_sdk/DTCotSdkMqtt.h"
#include "cot_sdk/DTCoTDevice.h"

#include "devices/DTCoTDeviceHUZZAH_WIFI.h"


/************************* WiFi Access Point *********************************/
#define SECRET_WIFI_SSID       "...your local Wifi network's SSID..."
#define SECRET_WIFI_PASSWORD   "...your local Wifi network's password..."

/*************************** Sketch Code ************************************/

using namespace DTCoT;

float tempMin = 20;
float tempMax = 30;
float tempInc = 0.5;
float tempVal = tempMin;

/* Setup the Cloud access and the device to communicate to cloud */

CoTDevice cotDevice;
CoTChildDevice childDevice1;
CoTChildDevice childDevice2;

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

	// Get/Create some ChildDevices once.
	childDevice1 = cotDevice.getOrCreateChildDevice( "childDevice_1");
	if( childDevice1.isValid()) {
		DT_DEBUG_PRINTLN("Setup,  ChildDevice 1 available. Name: %s, moId: %s",
				childDevice1.getName().c_str(), childDevice1.getMoId().c_str());
	}
	else
		DT_DEBUG_PRINTLN("Setup, ### ChildDevice 1 Not available!");

	childDevice2 = cotDevice.getOrCreateChildDevice( "childDevice_2");
	if( childDevice2.isValid()) {
		DT_DEBUG_PRINTLN("Setup,  ChildDevice 2 available. Name: %s, moId: %s",
				childDevice2.getName().c_str(), childDevice2.getMoId().c_str());
	}
	else
		DT_DEBUG_PRINTLN("Setup, ### ChildDevice 2 Not available!");

	delay(0);
	DT_DEBUG_PRINTLN_INFO("Setup... done.");
}

void loop() {
    float temp = simulateTemperature();

    DT_DEBUG_PRINTLN("Send measurement: %f", temp);
	cotDevice.sendMeasurement("ty", "Room1", "Temperature", "C", temp, now());

	// Get/create ChildDevices and send measurements to them.
	if( childDevice1.isValid()) {
		// Send
		childDevice1.sendAlarm("ChildDevice-1 alarm", "type", AlarmSeverity::WARNING, AlarmStatus::ACTIVE);
		childDevice1.sendMeasurement("ty", "Room1", "Humidity1", "%", 71.3, now());
	}

	if( childDevice2.isValid()) {
		// Send
		childDevice2.sendAlarm("ChildDevice-2 alarm", "type", AlarmSeverity::MAJOR, AlarmStatus::ACTIVE);
		childDevice2.sendMeasurement("ty", "Room1", "Humidity2", "%", 66.3, now());
	}

	// 3 or more available ChildDevices are not possible because of wrong AdaFruit Response parsing.

    DT_DEBUG_PRINTLN_INFO("Loop... done.");
	delay(10000); //[ms]
}

float simulateTemperature() {
	tempVal += tempInc;
    if (tempVal > tempMax) {
        tempVal = tempMin;
    }
	return tempVal;
}
