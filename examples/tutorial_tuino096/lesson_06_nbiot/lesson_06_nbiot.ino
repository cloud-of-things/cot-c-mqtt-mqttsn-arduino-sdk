/**
 * @description Simple Demo for CoT Library using MQTT-SN
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved.
 * @licence MIT licence
 */
/**
   This project demonstrates how sto build a CoT application

   Application description:
   In this demo a simulated temperature measurement and alarm are sent to the
   cloud (using MQTT-SN via NBIoT).

   This sketch has the following library dependencies:
   	    - <Adafruit MQTT Library> v0.20.1
        - <FlashStorage> v0.7.1
        - <Time> v1.5
        - <ArduinoSTL> v1.1.0
 */

#include "dt-arduino-iot-agent.h"
#include "cot_sdk/DTCotSDKmqttSN.h"
#include "cot_sdk/DTCoTDevice.h"
#include "devices/DTCoTDeviceTuino096.h"

#include "DTCotLed.h"

#if !defined(ARDUINO_ARCH_SAMD)
#error This project should be compiled with Tuino096.
#endif /*ARDUINO_ARCH_SAMD*/

using namespace DTCoT;

float tempMin = 20;
float tempMax = 30;
float tempInc = 0.2;
float temp = tempMin;

unsigned long startTS;
unsigned long loopCount = 0;
unsigned long loopInterval = 60000; //ms
unsigned long loopStart;
unsigned long lastLoopStart = 0;

/* = Global Variables =
 * Set them here as constants and use those in the code below if needed.
 */
#define NB_IOT_MQTTSN_PWD		"<password for access to the MQTT-SN connector>"


/*** Definition of operations for device remote control. ***/
//info led
DTCotLed led(LED_BUILTIN, false);

//My CoT device
CoTDevice cotDevice;

void setup() {
	long timeoutMs = 120000; //ms

	delay(100);
	Serial.begin(115200);
	delay(100);

	DT_DEBUG_PRINTLN("\nSetup...");

	/* Setup device and protocol to communicate to cloud. */

	CoTConfigDeviceTuino096NbIoT *devConfig = new CoTConfigDeviceTuino096NbIoT();
	CoTDeviceTuino096 *board = new CoTDeviceTuino096(*devConfig);
	/* board must be initialized to get the IMSI for the SDK */
	board->init(timeoutMs);

	/* MQTT-SN SDK needs the board's IMSI and the device's Password */
	CotSdkMqttSN* cotSDK = new CotSdkMqttSN( *board, board->getImsi(), NB_IOT_MQTTSN_PWD );

	//initialize the device
	while(! cotDevice.setup(cotSDK, timeoutMs) );

	delay(0);
	DT_DEBUG_PRINTLN("Setup done.");
}


void loop() {
	loopStart = millis();

	// send the "temperature" measurement only in intervals of 'loopInterval'
	if (!lastLoopStart || loopStart > (lastLoopStart + loopInterval)) {
		lastLoopStart = loopStart;
		loopCount++;
		DT_DEBUG_PRINTLN_INFO("Loop[%lu]...", loopCount);

		/*
		 * Note: in MQTT-SN the 'measurement' type is restricted!
		 * Possible values are:
		 * - "Temperature"
		 * - "Voltage"
		 * - "Acceleration"
		 * - "Light"
		 * - "Humidity"
		 * - "Moisture"
		 * - "Distance"
		 * - "Current"
		 * - "SignalStrength"
		 * - "Pressure"
		 * - "Volume"
		 * - "Weight"
		 * - "Frequency"
		 *
		 * NOTE: in MQTT-SN the parameters 'type' and 'unit' are ignored!
		 */
		while( ! cotDevice.sendMeasurement("ty", "Room1", "Temperature", "C", temp, now()));
		temp += tempInc;
		if (temp > tempMax) {
			temp = tempMin;
		}
	}
}

