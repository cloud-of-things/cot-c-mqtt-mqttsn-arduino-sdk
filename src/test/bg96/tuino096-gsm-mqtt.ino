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
   In this demo a simulated temperature measurement is sent to the cloud.
   In this demo multiple alarms are sent to the cloud.
   In this demo multiple operations are registered to receive from the cloud.

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


/*************************** Sketch Code ************************************/

using namespace DTCoT;

unsigned long startTS;
unsigned long loopCount = 0;
unsigned long loopInterval = 60000; //ms
unsigned long loopStart;
unsigned long lastLoopStart = 0;

float tempMin = 20;
float tempMax = 30;
float tempInc = 0.1;
float temp = tempMin;

//temperatue sensor simulation
float readTemp() {
    float t = temp;
    temp += tempInc;
    if (temp > tempMax) {
        temp = tempMin;
    }
    return t;
}


//My CoT device
CoTDevice cotDevice;

//info led
DTCotLed led(LED_BUILTIN, false);

//operation function to toggle led
OperationStatus operationLedToggle(std::vector<String>&) {
    led.toggle();
    return OperationStatus::SUCCESSFUL;
}

void setup() {
	// 'Serial' is USB, so... be carefull
  	delay(100);
  	Serial.begin(115200);
    delay(5000);
    DT_DEBUG_PRINTLN("\nSetup...");

    /* Setup the Cloud access and the device to communicate to cloud */
    CoTConfigDeviceTuino096 *boardConfig = new CoTConfigDeviceTuino096();
    CoTDeviceTuino096 *board = new CoTDeviceTuino096(*boardConfig);
    CotSdkMqtt *cotSDK = new CotSdkMqtt(*board);

    boardConfig->setDeviceId("HEHE.1.6.4086231995");
	cotSDK->setCotCredentials("HEHE.1.6.4086231995", "y2[3Y1cFp5", "161865");

	DT_DEBUG_PRINTLN("CotSdk version<%s>", cotSDK->getSdkVersion());

    //initialize the device
    while(!cotDevice.setup(cotSDK));

    cotDevice.registerOperation(operationLedToggle, DTCOTOPERATIONID_C8Y_COMMAND, "led toggle");

    //timestamp of device start
    startTS = now();

    DT_DEBUG_PRINTLN("Setup... done.");
}

void loop() {
    loopStart = millis();
    cotDevice.executeOperations();
    if (!lastLoopStart || loopStart > (lastLoopStart + loopInterval)) {
        lastLoopStart = loopStart;
        loopCount++;
    	cotDevice.sendAlarm("alarmCritical", "battery_problem", AlarmSeverity::CRITICAL, AlarmStatus::ACTIVE);
        cotDevice.sendMeasurement("c8y_Temperature", "room 1", "T", "°C", readTemp(), now());
    	cotDevice.updateDevice(	"", "", CotSmartRestProcessor::createCloudTimestring(startTS), String(loopCount).c_str(), "", "", 120);
    }
}
