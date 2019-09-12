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


using namespace DTCoT;

/* Application-specific settings */
float tempMin = 20;
float tempMax = 30;
float tempInc = 0.1;
float temp = tempMin;
bool restartFlag = false;
unsigned long startTS;
unsigned long loopCount = 0;
unsigned long loopInterval = 60000; //ms
unsigned long loopStart;
unsigned long lastLoopStart = 0;

/* = Global Variables =
 * Set them here as constants and use those in the code below if needed.
 */
//const char* DEVICE_NAME =		"<my_DEVICE_NAME>";
//const char* DEVICE_Passwd =	"<my_device_passwd>";
//const char* DEVICE_moID =		"<my_device_moID>";

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

//operation function to switch led on
OperationStatus operationLedOn(std::vector<String>&) {
    led.turnOn(true);
    return OperationStatus::SUCCESSFUL;
}

//operation function to switch led off
OperationStatus operationLedOff(std::vector<String>&) {
    led.turnOn(false);
    return OperationStatus::SUCCESSFUL;
}

//operation function to toggle led
OperationStatus operationLedToggle(std::vector<String>&) {
    led.toggle();
    return OperationStatus::SUCCESSFUL;
}

//operation function to let led blink
OperationStatus operationLedBlinkOn(std::vector<String>&) {
    led.blink(true);
    DT_DEBUG_PRINTLN_INFO("##  Operation 'Led Blink'");
    return OperationStatus::SUCCESSFUL;
}

//operation function to switch led blink off
OperationStatus operationLedBlinkOff(std::vector<String>&) {
    led.blink(false);
    return OperationStatus::SUCCESSFUL;
}

//operation function to set a flag to restart the device
OperationStatus operationRestart(std::vector<String>& messageParts) {
    DT_DEBUG_PRINTLN_INFO("#####################################");
    DT_DEBUG_PRINTLN_INFO("##  Operation RESTART<%s>", messageParts[2].c_str());
    DT_DEBUG_PRINTLN_INFO("#####################################");
    restartFlag = true;
    return OperationStatus::SUCCESSFUL;
}

//function to set check the restart flag and execute restart if set.
void restartDeviceIfFlaged(void) {
    if (restartFlag) {
    	DT_DEBUG_PRINTLN_INFO("#####################################");
    	DT_DEBUG_PRINTLN_INFO("##  Restarting device...");
    	DT_DEBUG_PRINTLN_INFO("#####################################");
        //signal restart will be executed immediately
        led.toggle(10, 100);
        // processor software reset
        NVIC_SystemReset();
        while(true);
    }
}

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
    //boardConfig->setAPN("<your_APN>");
    //boardConfig->setAPNUser("<your_APN_User>");
    //boardConfig->setAPNPassword("<your_APN_Passwd>");

    /* set non-default device name in the CoT */
    //boardConfig->setDeviceId(DEVICE_NAME);

    CoTDeviceTuino096 *board = new CoTDeviceTuino096(*boardConfig);

    CotSdkMqtt *cotSDK = new CotSdkMqtt(*board);

    /* set non default MQTT-Gateway */
    //const char* MQTT_Gateway =	"nb-iot.test-ram.m2m.telekom.com";
    //cotSDK->setBootstrapHost(MQTT_Gateway);
    //cotSDK->setCloudTenantHost(MQTT_Gateway);

    /* set credentials if your device instance shall survive a re-programming of this sketch */
    //cotSDK->setCotCredentials(DEVICE_NAME, DEVICE_Passwd, DEVICE_moID);

    //initialize the device
    while(!cotDevice.setup(cotSDK));

    //register the operation functions
    cotDevice.registerOperation(operationLedOn, DTCOTOPERATIONID_C8Y_COMMAND, "led on");
    cotDevice.registerOperation(operationLedOff, DTCOTOPERATIONID_C8Y_COMMAND, "led off");
    cotDevice.registerOperation(operationLedToggle, DTCOTOPERATIONID_C8Y_COMMAND, "led toggle");
    cotDevice.registerOperation(operationLedBlinkOn, DTCOTOPERATIONID_C8Y_COMMAND, "led blink");
    cotDevice.registerOperation(operationLedBlinkOff, DTCOTOPERATIONID_C8Y_COMMAND, "led blink off");
    cotDevice.registerOperation(operationRestart, DTCOTOPERATIONID_C8Y_COMMAND, "restart");
    cotDevice.registerOperation(operationRestart, DTCOTOPERATIONID_C8Y_RESTART, "");

    //signal setup is finished
    led.toggle(4, 100);

    //timestamp of device start
    startTS = now();

    // tell the CoT Dashboard that I am online
    cotDevice.updateDevice("Tu96", "1", "1", "ime", "icc", "ims", 120);
    cotDevice.sendAlarm("online since " + CotSmartRestProcessor::createCloudTimestring(startTS),
    			"notification", AlarmSeverity::WARNING, AlarmStatus::ACTIVE);

    DT_DEBUG_PRINTLN_INFO("Setup... done.");
}


void loop() {
    loopStart = millis();

    /* the 'executeOperations()' is triggered constantly to let it react instantly on
     * incoming operations */
    cotDevice.executeOperations();
    //trigger led status, because no interrupt is in use for!
    led.trigger();
    //check the restart flag
    restartDeviceIfFlaged();

    // send the "temperature" measurement only in intervals of 'loopInterval'
    if (!lastLoopStart || loopStart > (lastLoopStart + loopInterval)) {
        lastLoopStart = loopStart;
    	loopCount++;
    	DT_DEBUG_PRINTLN_INFO("Loop[%lu]...", loopCount);

        cotDevice.sendMeasurement("c8y_Temperature", "room 1", "T", "°C", readTemp(), now());

        DT_DEBUG_PRINTLN_INFO("Loop[%ld]... done in <%lu>ms.", loopCount, millis() - loopStart);
        DT_DEBUG_PRINT("|");
    }

}
