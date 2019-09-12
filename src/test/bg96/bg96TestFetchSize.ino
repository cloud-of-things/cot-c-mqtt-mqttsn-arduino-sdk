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

#include "devices/DTCoTDeviceTuino096.h"


/*************************** Sketch Code ************************************/

using namespace DTCoT;

unsigned long startTS;
unsigned long loopCount = 0;
unsigned long loopStart;
unsigned long lastLoopStart = 0;

int fetchSize = 30;

Client *tcp;

void setup() {
  	delay(100);
  	Serial.begin(115200);
    delay(5000);
    DT_DEBUG_PRINTLN("\nSetup...");

    CoTConfigDeviceTuino096 *boardConfig = new CoTConfigDeviceTuino096();

    // Changing default device configuration.
    // !! only changeable before create device instance !!

    //if necessary set APN credentials
//    boardConfig->setAPN("???");
//    boardConfig->setAPNUser("???");
//    boardConfig->setAPNPassword("???");

    //switch ssl on(1)/off(0)
    boardConfig->setSsl(0);
    //set modem fetch size
    boardConfig->setFetchSize(fetchSize);


    CoTDeviceTuino096 *board = new CoTDeviceTuino096(*boardConfig);

    long initTO = 60000;
    board->init(initTO);

    tcp = board->getClient();

    startTS = now();

    DT_DEBUG_PRINTLN("Setup... done.");
}

const char *tosendbuffer =
		"1234567890abcdefghijABCDEFGHIJklmnopqrstUVWXYZß!?("
		"1234567890abcdefghijABCDEFGHIJklmnopqrstUVWXYZß!?("
		"1234567890abcdefghijABCDEFGHIJklmnopqrstUVWXYZß!?("
		"1234567890abcdefghijABCDEFGHIJklmnopqrstUVWXYZß!?("
		"1234567890abcdefghijABCDEFGHIJklmnopqrstUVWXYZß!?("
		"1234567890abcdefghijABCDEFGHIJklmnopqrstUVWXYZß!?("
		"1234567890abcdefghijABCDEFGHIJklmnopqrstUVWXYZß!?("
		"1234567890abcdefghijABCDEFGHIJklmnopqrstUVWXYZß!?("
		"1234567890abcdefghijABCDEFGHIJklmnopqrstUVWXYZß!?("
		"1234567890abcdefghijABCDEFGHIJklmnopqrstUVWXYZß!?("
;

char responseBuffer[2000];


unsigned long loopInterval = 20000; //ms
unsigned int sendCount = 0;
int readCount = 0;
int available = 0;

void loop() {
    loopStart = millis();

    if (tcp->connected()) {

		available = tcp->available();
		if (available > 0) {
			memset(responseBuffer, 0, sizeof(responseBuffer));

			readCount = tcp->read((uint8_t*)responseBuffer, available);

			DT_DEBUG_PRINTLN("#######################################");
			DT_DEBUG_PRINTLN("available<%d> readCount<%d>", available, readCount);
			DT_DEBUG_PRINTBUFFER("RX", (uint8_t*)responseBuffer, readCount);
			DT_DEBUG_PRINTLN("#######################################");
		}

		if (!lastLoopStart || loopStart > (lastLoopStart + loopInterval)) {
			lastLoopStart = loopStart;
			loopCount++;

			sendCount = tcp->write((uint8_t *)tosendbuffer, (unsigned int)strlen(tosendbuffer));

			DT_DEBUG_PRINTLN("---------------------------------------");
			DT_DEBUG_PRINTLN("sendCount<%u>", sendCount);
			DT_DEBUG_PRINTBUFFER("TX", (uint8_t*)(char*)tosendbuffer, sendCount);
			DT_DEBUG_PRINTLN("---------------------------------------");
			DT_DEBUG_PRINTLN("fetchSize<%d>", fetchSize);
		}

    } else {

    	// Connect mirror server
    	int rcConnect = tcp->connect("220.180.239.212", 8009);
        DT_DEBUG_PRINTLN("tcp connect <%d>", rcConnect);

    }
}
