/**
 * @description Simple Demo for CoT Library using MQTT
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved.
 * @licence MIT licence
 */
/**
   This project demonstrates how sto build a CoT application

   Application description:
   In this demo multiple alarms are sent to the cloud.

   This sketch has the following library dependencies:
        - <Adafruit_MQTT_Library> v0.20
        - <Time> v1.5
        - <FlashStorage> v0.7.1
        - <ArduinoSTL> v1.1.0
*/

#include "dt-arduino-iot-agent.h"
#include "cot_sdk/DTCotSdkMqtt.h"
#include "cot_sdk/DTCoTDevice.h"

#include "devices/DTCoTDeviceTuino096.h"

#include "DTCotLed.h"


using namespace DTCoT;

/* Application-specific settings */
float tempMin = 20;
float tempMax = 30;
float tempInc = 1;
float temp = tempMin;
bool restartFlag = false;
unsigned long startTS;
unsigned long loopCount = 0;
unsigned long loopInterval = 60000; //ms
unsigned long loopStart;
unsigned long lastLoopStart = 0;
float tempThreshold = 22; 	// Alarm threshold, °C

/* = Global Variables =
 * Set them here as constants and use those in the code below if needed.
 */
const char* DEVICE_NAME =	"<my_DEVICE_NAME>";
const char* DEVICE_Passwd =	"<my_device_passwd>";
const char* DEVICE_moID =	"<my_device_moID>";
//const char* APN = 			"<my_APN>";
//const char* APN_User =		"<my_APN_User>";
//const char* APN_Password =	"<my_APN_Passwd>";



//temperatue sensor simulation
float readTemp() {
    float t = temp;
    temp += tempInc;
    if (temp > tempMax) {
        temp = tempMin;
    }
    return t;
}

//info led
DTCotLed led(LED_BUILTIN, false);

//My CoT device
CoTDevice cotDevice;

void setup() {
    delay(100);
    Serial.begin(115200);
    delay(100);
    DT_DEBUG_PRINTLN_INFO("\nSetup...");

    DT_DEBUG_PRINTLN("DT_COT_ARDUINO_LIBRARY_VERSION <%s>", DT_COT_ARDUINO_LIBRARY_VERSION);


    /* Setup the Cloud access and the device to communicate to cloud */

    CoTConfigDeviceTuino096 *boardConfig = new CoTConfigDeviceTuino096();

    /* set non-default APN */
    //boardConfig->setAPN(APN);
	//boardConfig->setAPNUser(APN_User);
	//boardConfig->setAPNPassword(APN_Passwd>);

    /* set non-default device name in the CoT */
    boardConfig->setDeviceId(DEVICE_NAME);

    CoTDeviceTuino096 *board = new CoTDeviceTuino096(*boardConfig);

    CotSdkMqtt *cotSDK = new CotSdkMqtt(*board);

    /* set credentials if your device instance shall survive a re-programming of this sketch */
    cotSDK->setCotCredentials(DEVICE_NAME, DEVICE_Passwd, DEVICE_moID);

    //initialize the device
    while(!cotDevice.setup(cotSDK));

    //signal setup is finished
    led.toggle(4, 100);

    //timestamp of device start
    startTS = now();

    // tell the CoT Dashboard that I am online
    cotDevice.updateDevice("Tu96", "1", "1", "ime", "icc", "ims", 120);

    DT_DEBUG_PRINTLN_INFO("Setup... done.");
}


void loop() {
    loopStart = millis();

    //trigger led status, because no interrupt is in use for!
    led.trigger();

    // send the "temperature" measurement only in intervals of 'loopInterval'
    if (!lastLoopStart || loopStart > (lastLoopStart + loopInterval)) {
        lastLoopStart = loopStart;
    	loopCount++;
    	DT_DEBUG_PRINTLN_INFO("Loop[%lu]...", loopCount);

        cotDevice.sendMeasurement("c8y_Temperature", "room 1", "T", "Â°C", readTemp(), now());

        if (temp > tempThreshold) {
			DT_DEBUG_PRINTLN("### temp: %f greater than threshold: %f => send Alarm!",
					temp, tempThreshold);

			cotDevice.sendAlarm("Temperature threshold exceeded!", "temperature_alarm",
					AlarmSeverity::MAJOR, AlarmStatus::ACTIVE);
		}

		DT_DEBUG_PRINTLN_INFO("Loop[%ld]... done in <%lu>ms.", loopCount, millis() - loopStart);
        DT_DEBUG_PRINT("|");
    }

}
